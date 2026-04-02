/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "MinViewController.h"
#import <math.h>
#import <string>
#import <QuartzCore/QuartzCore.h>
#import "CubismFramework.hpp"
#import <Math/CubismMatrix44.hpp>
#import <Math/CubismViewMatrix.hpp>
#import <GLKit/GLKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "MinAppDelegate.h"
#import "MinSceneDelegate.h"
#import "MinLAppSprite.h"
#import "MinTouchManager.h"
#import "MinLAppDefine.h"
#import "MinLAppLive2DManager.h"
#import "MinLAppTextureManager.h"
#import "MinLAppPal.h"
#import "MinLAppModel.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

using namespace std;
using namespace MinLAppDefine;

@interface MinViewController ()
@property (nonatomic) MinLAppSprite *renderSprite; //レンダリングターゲット描画用
@property (nonatomic) MinTouchManager *touchManager; ///< タッチマネージャー
@property (nonatomic) Csm::CubismMatrix44 *deviceToScreen;///< デバイスからスクリーンへの行列
@property (nonatomic) Csm::CubismViewMatrix *viewMatrix;
@property (nonatomic) int windowWidth;
@property (nonatomic) int windowHeight;

@end

@implementation MinViewController
@synthesize mOpenGLRun;

- (void)releaseView
{
    _renderBuffer.DestroyRenderTarget();

    _renderSprite = nil;

    GLKView *view = (GLKView*)self.view;

    view = nil;

    delete(_viewMatrix);
    _viewMatrix = nil;
    delete(_deviceToScreen);
    _deviceToScreen = nil;
    _touchManager = nil;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    mOpenGLRun = true;

    _anotherTarget = false;
    _spriteColorR = _spriteColorG = _spriteColorB = _spriteColorA = 1.0f;
    _clearColorR = _clearColorG = _clearColorB = 1.0f;
    _clearColorA = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = [[MinTouchManager alloc]init];

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();

    [self initializeScreen];

    GLKView *view = (GLKView*)self.view;

    // OpenGL ES2を指定
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    // set context
    [EAGLContext setCurrentContext:view.context];

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


    glGenBuffers(1, &_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);

    glGenBuffers(1, &_fragmentBufferId);
    glBindBuffer(GL_ARRAY_BUFFER,  _fragmentBufferId);

    [self initializeSprite];
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    [self resizeScreen];
}

- (void)initializeScreen
{
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;
    if (!windowScene)
    {
        return;
    }

    if (windowScene.screen.bounds.size.width <= 0 || windowScene.screen.bounds.size.height <= 0)
    {
        return;
    }

    UIEdgeInsets insets = windowScene.windows.firstObject.safeAreaInsets;
    int width = windowScene.screen.bounds.size.width - insets.left - insets.right;
    int height = windowScene.screen.bounds.size.height - insets.top - insets.bottom;

    if (width <= 0 || height <= 0)
    {
        return;
    }

    const CGFloat retinaScale = self.traitCollection.displayScale;
    _windowWidth = width * retinaScale;
    _windowHeight = height * retinaScale;

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->SetScreenRect(left, right, bottom, top);
    _viewMatrix->Scale(ViewScale, ViewScale);

    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
                                  ViewLogicalMaxLeft,
                                  ViewLogicalMaxRight,
                                  ViewLogicalMaxBottom,
                                  ViewLogicalMaxTop
                                  );
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    //時間更新
    MinLAppPal::UpdateTime();

    if(mOpenGLRun)
    {
        // 画面クリア
        glClear(GL_COLOR_BUFFER_BIT);

        MinLAppLive2DManager* Live2DManager = [MinLAppLive2DManager getInstance];
        [Live2DManager SetViewMatrix:_viewMatrix];
        [Live2DManager onUpdate];

        // 各モデルが持つ描画ターゲットをテクスチャとする場合はスプライトへの描画はここ
        if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
        {
            float uvVertex[] =
            {
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
            };

            float a = [self GetSpriteAlpha:2]; // サンプルとしてαに適当な差をつける
            [_renderSprite SetColor:1.0f * a g:1.0f * a b:1.0f * a a:a];

            MinLAppModel* model = [Live2DManager getModel];
            if (model)
            {
                Csm::Rendering::CubismRenderTarget_OpenGLES2& useTarget = model->GetRenderBuffer();
                GLuint id = useTarget.GetColorBuffer();
                [_renderSprite renderImmidiate:_vertexBufferId fragmentBufferID:_fragmentBufferId TextureId:id uvArray:uvVertex];
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

- (void)initializeSprite
{
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    int width = screenRect.size.width;
    int height = screenRect.size.height;

    MinLAppTextureManager* textureManager = [self.sceneDelegate getTextureManager];
    const string resourcesPath = ResourcesPath;

    string imageName = BackImageName;
    TextureInfo* backgroundTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
    float x = static_cast<float>(width) * 0.5f;
    float y = static_cast<float>(height) * 0.5f;
    float fWidth = static_cast<float>(width*2);
    float fHeight = static_cast<float>(height*2);

    _renderSprite = [[MinLAppSprite alloc] initWithMyVar:x Y:y Width:fWidth/2 Height:fHeight/2 MaxWidth:width MaxHeight:height TextureId:0];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

    [_touchManager touchesBegan:point.x DeciveY:point.y];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

    float viewX = [self transformViewX:[_touchManager getX]];
    float viewY = [self transformViewY:[_touchManager getY]];

    [_touchManager touchesMoved:point.x DeviceY:point.y];
    [[MinLAppLive2DManager getInstance] onDrag:viewX floatY:viewY];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    NSLog(@"%@", touch.view);

    CGPoint point = [touch locationInView:self.view];
    float pointY = [self transformTapY:point.y];

    // タッチ終了
    MinLAppLive2DManager* live2DManager = [MinLAppLive2DManager getInstance];
    [live2DManager onDrag:0.0f floatY:0.0f];
    {
        // シングルタップ
        float getX = [_touchManager getX];// 論理座標変換した座標を取得。
        float getY = [_touchManager getY]; // 論理座標変換した座標を取得。
        float x = _deviceToScreen->TransformX(getX);
        float y = _deviceToScreen->TransformY(getY);

        if (DebugTouchLogEnable)
        {
            MinLAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
        [live2DManager onTap:x floatY:y];
    }
}

- (float)transformViewX:(float)deviceX
{
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

- (float)transformViewY:(float)deviceY
{
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

- (float)transformScreenX:(float)deviceX
{
    return _deviceToScreen->TransformX(deviceX);
}

- (float)transformScreenY:(float)deviceY
{
    return _deviceToScreen->TransformY(deviceY);
}

- (float)transformTapY:(float)deviceY
{
    float height = self.view.frame.size.height;
    return deviceY * -1 + height;
}

- (void)PreModelDraw:(MinLAppModel&)refModel
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismRenderTarget_OpenGLES2* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 透過設定
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {// 描画ターゲット内部未作成の場合はここで作成
            CGFloat scale = self.traitCollection.displayScale;
            int width = self.view.frame.size.width * scale;
            int height = self.view.frame.size.height * scale;

            // モデル描画キャンバス
            // PadとPhoneで縦横を変えている
            if([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
            {
                useTarget->CreateRenderTarget(height, width);
            }
            else
            {
                useTarget->CreateRenderTarget(width, height);
            }
        }

        // レンダリング開始
        useTarget->BeginDraw();
        useTarget->Clear(_clearColorR, _clearColorG, _clearColorB, _clearColorA); // 背景クリアカラー
    }
}

- (void)PostModelDraw:(MinLAppModel&)refModel
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismRenderTarget_OpenGLES2* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw();

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            float uvVertex[] =
            {
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
            };

            float a = [self GetSpriteAlpha:0];
            [_renderSprite SetColor:1.0f * a g:1.0f * a b:1.0f * a a:a];
            [_renderSprite renderImmidiate:_vertexBufferId fragmentBufferID:_fragmentBufferId TextureId:useTarget->GetColorBuffer() uvArray:uvVertex];
        }
    }
}

- (void)SwitchRenderingTarget:(SelectTarget)targetType
{
    _renderTarget = targetType;
}

- (void)SetRenderTargetClearColor:(float)r g:(float)g b:(float)b
{
    _clearColorR = r;
    _clearColorG = g;
    _clearColorB = b;
}

- (float)GetSpriteAlpha:(int)assign
{
    // assignの数値に応じて適当に決定
    float alpha = 0.4f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    if (alpha < 0.1f)
    {
        alpha = 0.1f;
    }

    return alpha;
}

- (int)GetWindowWidth
{
    return _windowWidth;
}

- (int)GetWindowHeight;
{
    return _windowHeight;
}

- (void)resizeScreen
{
    int width = self.view.frame.size.width;
    int height = self.view.frame.size.height;

    if (width == 0 || height == 0)
    {
        return;
    }

    const CGFloat retinaScale = self.traitCollection.displayScale;
    int newWindowWidth = width * retinaScale;
    int newWindowHeight = height * retinaScale;
    if (newWindowWidth == _windowWidth && newWindowHeight == _windowHeight)
    {
        return;
    }
    _windowWidth = newWindowWidth;
    _windowHeight = newWindowHeight;

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->SetScreenRect(left, right, bottom, top);
    _viewMatrix->Scale(ViewScale, ViewScale);

    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
                                  ViewLogicalMaxLeft,
                                  ViewLogicalMaxRight,
                                  ViewLogicalMaxBottom,
                                  ViewLogicalMaxTop
                                  );

    [self resizeSprite:width height:height];
}

- (void)resizeSprite:(float)width height:(float)height
{
    // レンダリングスプライト
    if (_renderSprite) {
        float x = static_cast<float>(width) * 0.5f;
        float y = static_cast<float>(height) * 0.5f;
        [_renderSprite resizeImmidiate:x Y:y Width:width Height:height MaxWidth:width MaxHeight:height];
    }
}

@end
