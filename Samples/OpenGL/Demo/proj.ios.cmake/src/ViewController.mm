/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "ViewController.h"
#import <math.h>
#import <string>
#import <QuartzCore/QuartzCore.h>
#import "CubismFramework.hpp"
#import <Math/CubismMatrix44.hpp>
#import <Math/CubismViewMatrix.hpp>
#import <GLKit/GLKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "AppDelegate.h"
#import "SceneDelegate.h"
#import "LAppSprite.h"
#import "TouchManager.h"
#import "LAppDefine.h"
#import "LAppLive2DManager.h"
#import "LAppTextureManager.h"
#import "LAppPal.h"
#import "LAppModel.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

using namespace std;
using namespace LAppDefine;

@interface ViewController ()
@property (nonatomic) LAppSprite *back; //背景画像
@property (nonatomic) LAppSprite *gear; //歯車画像
@property (nonatomic) LAppSprite *power; //電源画像
@property (nonatomic) LAppSprite *renderSprite; //レンダリングターゲット描画用
@property (nonatomic) TouchManager *touchManager; ///< タッチマネージャー
@property (nonatomic) Csm::CubismMatrix44 *deviceToScreen;///< デバイスからスクリーンへの行列
@property (nonatomic) Csm::CubismViewMatrix *viewMatrix;
@property (nonatomic) int windowWidth;
@property (nonatomic) int windowHeight;

@end

@implementation ViewController
@synthesize mOpenGLRun;

- (void)releaseView
{
    _renderBuffer.DestroyRenderTarget();

    _renderSprite = nil;
    _gear = nil;
    _back = nil;
    _power = nil;

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
    _touchManager = [[TouchManager alloc]init];

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();

    [self initializeScreen];

    GLKView *view = (GLKView*)self.view;

    // GL描画周期を60FPSに設定
    self.preferredFramesPerSecond = 60;

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

- (void)initializeScreen
{
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;
    if (!windowScene)
    {
        return;
    }

    UIEdgeInsets insets = windowScene.windows.firstObject.safeAreaInsets;

    if (windowScene.screen.bounds.size.width <= 0 || windowScene.screen.bounds.size.height <= 0)
    {
        return;
    }

    int width = windowScene.screen.bounds.size.width - insets.left - insets.right;
    int height = windowScene.screen.bounds.size.height - insets.top - insets.bottom;

    CGFloat retinaScale = self.traitCollection.displayScale;
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

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    [self resizeScreen];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    //時間更新
    LAppPal::UpdateTime();

    if(mOpenGLRun)
    {
        // 画面クリア
        glClear(GL_COLOR_BUFFER_BIT);

        // セーフエリアのみ
        UIEdgeInsets insets = self.view.safeAreaInsets;
        CGFloat scale = self.traitCollection.displayScale;
        GLint vpX = (GLint)(insets.left * scale);
        GLint vpY = (GLint)(insets.bottom * scale);
        GLsizei vpW = (GLsizei)((self.view.frame.size.width - insets.left - insets.right) * scale);
        GLsizei vpH = (GLsizei)((self.view.frame.size.height - insets.top - insets.bottom) * scale);
        glViewport(vpX, vpY, vpW, vpH);

        [_back render:_vertexBufferId fragmentBufferID:_fragmentBufferId];

        [_gear render:_vertexBufferId fragmentBufferID:_fragmentBufferId];

        [_power render:_vertexBufferId fragmentBufferID:_fragmentBufferId];

        LAppLive2DManager* Live2DManager = [LAppLive2DManager getInstance];
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

            for(csmUint32 i=0; i<[Live2DManager GetModelNum]; i++)
            {
                LAppModel* model = [Live2DManager getModel:i];
                float a = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
                [_renderSprite SetColor:1.0f * a g:1.0f * a b:1.0f * a a:a];

                if (model)
                {
                    Csm::Rendering::CubismRenderTarget_OpenGLES2& useTarget = model->GetRenderBuffer();
                    GLuint id = useTarget.GetColorBuffer();
                    [_renderSprite renderImmidiate:_vertexBufferId fragmentBufferID:_fragmentBufferId TextureId:id uvArray:uvVertex];
                }
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

- (void)initializeSprite
{
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;
    if (!windowScene)
    {
        return;
    }
    UIEdgeInsets insets = windowScene.windows.firstObject.safeAreaInsets;
    int width = windowScene.screen.bounds.size.width - insets.left - insets.right;
    int height = windowScene.screen.bounds.size.height - insets.top - insets.bottom;

    LAppTextureManager* textureManager = [self.sceneDelegate getTextureManager];
    const string resourcesPath = ResourcesPath;

    float x;
    float y;
    float fWidth;
    float fHeight;
    float ratio;

    string imageName = BackImageName;
    if (!_back)
    {
        TextureInfo* backgroundTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = width * 0.5f;
        y = height * 0.5f;
        fHeight = static_cast<float>(height * 0.95f);
        ratio = fHeight / static_cast<float>(backgroundTexture->height);
        fWidth = static_cast<float>(backgroundTexture->width * ratio);
        _back = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height TextureId:backgroundTexture->id];
    }

    imageName = GearImageName;
    if (!_gear)
    {
        TextureInfo* gearTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = static_cast<float>(width - gearTexture->width * 0.5f);
        y = static_cast<float>(height - gearTexture->height * 0.5f);
        fWidth = static_cast<float>(gearTexture->width);
        fHeight = static_cast<float>(gearTexture->height);
        _gear = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height TextureId:gearTexture->id];
    }

    imageName = PowerImageName;
    if (!_power)
    {
        TextureInfo* powerTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = static_cast<float>(width - powerTexture->width * 0.5f);
        y = static_cast<float>(powerTexture->height * 0.5f);
        fWidth = static_cast<float>(powerTexture->width);
        fHeight = static_cast<float>(powerTexture->height);
        _power = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height TextureId:powerTexture->id];
    }

    if (!_renderSprite)
    {
        x = static_cast<float>(width) * 0.5f;
        y = static_cast<float>(height) * 0.5f;
        fWidth = static_cast<float>(width*2);
        fHeight = static_cast<float>(height*2);
        _renderSprite = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth/2 Height:fHeight/2 MaxWidth:width MaxHeight:height TextureId:0];
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;

    [_touchManager touchesBegan:point.x DeciveY:point.y];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;

    float viewX = [self transformViewX:[_touchManager getX]];
    float viewY = [self transformViewY:[_touchManager getY]];

    [_touchManager touchesMoved:point.x DeviceY:point.y];
    [[LAppLive2DManager getInstance] onDrag:viewX floatY:viewY];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    NSLog(@"%@", touch.view);

    CGPoint point = [touch locationInView:self.view];
    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;
    float pointY = [self transformTapY:point.y];

    // タッチ終了
    LAppLive2DManager* live2DManager = [LAppLive2DManager getInstance];
    [live2DManager onDrag:0.0f floatY:0.0f];
    {
        // シングルタップ
        float getX = [_touchManager getX];// 論理座標変換した座標を取得。
        float getY = [_touchManager getY]; // 論理座標変換した座標を取得。
        float x = _deviceToScreen->TransformX(getX);
        float y = _deviceToScreen->TransformY(getY);

        if (DebugTouchLogEnable)
        {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
        [live2DManager onTap:x floatY:y];

        // 歯車にタップしたか
        if ([_gear isHit:point.x PointY:pointY])
        {
            [live2DManager nextScene];
        }

        // 電源ボタンにタップしたか
        if ([_power isHit:point.x PointY:pointY])
        {
            AppDelegate *delegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
            [delegate finishApplication];

        }
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
    UIEdgeInsets insets = self.view.safeAreaInsets;
    float height = self.view.frame.size.height - insets.top - insets.bottom;
    return deviceY * -1 + height;
}

- (void)PreModelDraw:(LAppModel&)refModel
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismRenderTarget_OpenGLES2* useTarget = NULL;

    // 透過設定
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        int width = _windowWidth;
        int height = _windowHeight;

        if (!useTarget->IsValid() || useTarget->GetBufferWidth() != width || useTarget->GetBufferHeight() != height)
        {// 描画ターゲット内部未作成の場合はここで作成

            // モデル描画キャンバス
            useTarget->CreateRenderTarget(width, height);
        }

        // レンダリング開始
        useTarget->BeginDraw();
        glViewport(0, 0, width, height);
        useTarget->Clear(_clearColorR, _clearColorG, _clearColorB, _clearColorA); // 背景クリアカラー
    }
}

- (void)PostModelDraw:(LAppModel&)refModel
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
    UIEdgeInsets insets = self.view.safeAreaInsets;
    int width = self.view.frame.size.width - insets.left - insets.right;
    int height = self.view.frame.size.height - insets.top - insets.bottom;

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

    LAppLive2DManager* Live2DManager = [LAppLive2DManager getInstance];

    [Live2DManager setRenderTargetSize:newWindowWidth height:newWindowHeight];

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
    UIEdgeInsets insets = self.view.safeAreaInsets;
    float maxWidth = self.view.frame.size.width - insets.left - insets.right;
    float maxHeight = self.view.frame.size.height - insets.top - insets.bottom;

    LAppTextureManager* textureManager = [self.sceneDelegate getTextureManager];

    const string resourcesPath = ResourcesPath;

    // 背景
    float x;
    float y;
    float fHeight;
    float ratio;
    float fWidth;
    if (_back)
    {
        TextureInfo* backgroundTexture = [textureManager createTextureFromPngFile:resourcesPath+BackImageName];
        x = width * 0.5f;
        y = height * 0.5f;
        fHeight = static_cast<float>(height * 0.95f);
        ratio = fHeight / static_cast<float>(backgroundTexture->height);
        fWidth = static_cast<float>(backgroundTexture->width * ratio);
        [_back resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];
    }

    // 歯車（右上）
    if (_gear)
    {
        TextureInfo* gearTexture = [textureManager createTextureFromPngFile:resourcesPath+GearImageName];
        int gearWidth = gearTexture->width;
        int gearHeight = gearTexture->height;
        x = static_cast<float>(width - gearWidth * 0.5f);
        y = static_cast<float>(height - gearHeight * 0.5f);
        fHeight = static_cast<float>(gearHeight);
        fWidth = static_cast<float>(gearWidth);
        [_gear resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];
    }

    // 電源（右下）
    if (_power)
    {
        TextureInfo* powerTexture = [textureManager createTextureFromPngFile:resourcesPath+PowerImageName];
        int powerWidth = powerTexture->width;
        int powerHeight = powerTexture->height;
        x = static_cast<float>(width - powerWidth * 0.5f);
        y = static_cast<float>(powerHeight * 0.5f);
        fWidth = static_cast<float>(powerWidth);
        fHeight = static_cast<float>(powerHeight);
        [_power resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];
    }

    // レンダリングスプライト
    if (_renderSprite) {
        x = static_cast<float>(width) * 0.5f;
        y = static_cast<float>(height) * 0.5f;
        [_renderSprite resizeImmidiate:x Y:y Width:width Height:height MaxWidth:maxWidth MaxHeight:maxHeight];
    }
}

- (void)dealloc
{
    [self releaseView];
}

@end
