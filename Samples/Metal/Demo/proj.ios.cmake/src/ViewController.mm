/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "ViewController.h"
#import <math.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <string>
#import "CubismFramework.hpp"
#import "AppDelegate.h"
#import "SceneDelegate.h"
#import "LAppSprite.h"
#import "LAppDefine.h"
#import "LAppLive2DManager.h"
#import "LAppTextureManager.h"
#import "LAppPal.h"
#import "LAppModel.h"
#import "TouchManager.h"
#import "MetalUIView.h"
#import <Math/CubismMatrix44.hpp>
#import <Math/CubismViewMatrix.hpp>
#import <Rendering/Metal/CubismRenderer_Metal.hpp>

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

using namespace std;
using namespace LAppDefine;

@interface ViewController ()
@property (nonatomic) id<MTLDevice> device; //デバイス
@property (nonatomic) LAppSprite *back; //背景画像
@property (nonatomic) LAppSprite *gear; //歯車画像
@property (nonatomic) LAppSprite *power; //電源画像
@property (nonatomic) LAppModelSprite *renderSprite; //レンダリングターゲット描画用
@property (nonatomic) TouchManager *touchManager; ///< タッチマネージャー
@property (nonatomic) Csm::CubismMatrix44 *deviceToScreen;///< デバイスからスクリーンへの行列
@property (nonatomic) Csm::CubismViewMatrix *viewMatrix;
@property (nonatomic) int windowWidth;
@property (nonatomic) int windowHeight;

@end

@implementation ViewController

- (void)releaseView
{
    _renderSprite = nil;
    [_gear release];
    [_back release];
    [_power release];
    _gear = nil;
    _back = nil;
    _power = nil;

    [_commandQueue release];
    _commandQueue = nil;
    [_depthTexture release];
    _depthTexture = nil;

    MetalUIView *view = (MetalUIView*)self.view;

    view = nil;

    [_depthTexture release];
    _depthTexture = nil;
    Csm::Rendering::CubismDeviceInfo_Metal::ReleaseDeviceInfo(_device);
    delete(_viewMatrix);
    _viewMatrix = nil;
    delete(_deviceToScreen);
    _deviceToScreen = nil;
    [_touchManager release];
    _touchManager = nil;
}

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView
{
    MetalUIView *metalUiView = [[MetalUIView alloc] init];
    [self setView:metalUiView];
}

- (void)viewDidLoad
{
    [super viewDidLoad];

#if TARGET_OS_MACCATALYST
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;
    if (windowScene)
    {
        windowScene.titlebar.titleVisibility = UITitlebarTitleVisibilityHidden;
    }
#endif

    _device = MTLCreateSystemDefaultDevice();

    MetalUIView *view = (MetalUIView*)self.view;

    // Set the device for the layer so the layer can create drawable textures that can be rendered to
    // on this device.
    view.metalLayer.device = _device;

    // Set this class as the delegate to receive resize and render callbacks.
    view.delegate = self;

    view.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    _commandQueue = [_device newCommandQueue];

    _anotherTarget = false;
    _clearColorR = _clearColorG = _clearColorB = 1.0f;
    _clearColorA = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = [[TouchManager alloc]init];

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();

    // モデルロード前に必ず呼び出す必要がある
    Csm::Rendering::CubismRenderer_Metal::SetConstantSettings(_device);

    [self initializeScreen];
}

- (void)initializeScreen
{
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;

    if (!windowScene)
    {
        return;
    }

#if TARGET_OS_MACCATALYST
    int width = self.view.bounds.size.width;
    int height = self.view.bounds.size.height;
#else
    UIEdgeInsets insets = windowScene.windows.firstObject.safeAreaInsets;

    int width = windowScene.screen.bounds.size.width - insets.left - insets.right;
    int height = windowScene.screen.bounds.size.height - insets.top - insets.bottom;
#endif

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

- (void)resizeScreen
{
#if TARGET_OS_MACCATALYST
    int width = self.view.frame.size.width;
    int height = self.view.frame.size.height;
#else
    UIEdgeInsets insets = self.view.safeAreaInsets;
    int width = self.view.frame.size.width - insets.left - insets.right;
    int height = self.view.frame.size.height - insets.top - insets.bottom;
#endif

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

- (void)initializeSprite
{
    UIWindowScene *windowScene = (UIWindowScene *)[UIApplication sharedApplication].connectedScenes.anyObject;

    if (!windowScene)
    {
        return;
    }

#if TARGET_OS_MACCATALYST
    float width = self.view.bounds.size.width;
    float height = self.view.bounds.size.height;
#else
    UIEdgeInsets insets = windowScene.windows.firstObject.safeAreaInsets;
    float width = windowScene.screen.bounds.size.width - insets.left - insets.right;
    float height = windowScene.screen.bounds.size.height - insets.top - insets.bottom;
#endif

    LAppTextureManager* textureManager = [self.sceneDelegate getTextureManager];
    const string resourcesPath = ResourcesPath;

    string imageName;
    float x;
    float y;
    float fWidth;
    float fHeight;
    float ratio;

    //背景
    if (!_back)
    {
        imageName = BackImageName;
        TextureInfo* backgroundTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = width * 0.5f;
        y = height * 0.5f;
        fHeight = static_cast<float>(height) * 0.95f;
        ratio = fHeight / static_cast<float>(backgroundTexture->height);
        fWidth = static_cast<float>(backgroundTexture->width) * ratio;
        _back = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height Texture:backgroundTexture->id];
    }

    //モデル変更ボタン
    if (!_gear)
    {
        imageName = GearImageName;
        TextureInfo* gearTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = static_cast<float>(width - gearTexture->width * 0.5f);
        y = static_cast<float>(height - gearTexture->height * 0.5f);
        fWidth = static_cast<float>(gearTexture->width);
        fHeight = static_cast<float>(gearTexture->height);
        _gear = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height Texture:gearTexture->id];
    }

    //電源ボタン
    if (!_power)
    {
        imageName = PowerImageName;
        TextureInfo* powerTexture = [textureManager createTextureFromPngFile:resourcesPath+imageName];
        x = static_cast<float>(width - powerTexture->width * 0.5f);
        y = static_cast<float>(powerTexture->height * 0.5f);
        fWidth = static_cast<float>(powerTexture->width);
        fHeight = static_cast<float>(powerTexture->height);
        _power = [[LAppSprite alloc] initWithMyVar:x Y:y Width:fWidth Height:fHeight MaxWidth:width MaxHeight:height Texture:powerTexture->id];
    }
}

- (void)resizeSprite:(float)width height:(float)height
{
#if TARGET_OS_MACCATALYST
    float maxWidth = self.view.frame.size.width;
    float maxHeight = self.view.frame.size.height;
#else
    UIEdgeInsets insets = self.view.safeAreaInsets;
    float maxWidth = self.view.frame.size.width - insets.left - insets.right;
    float maxHeight = self.view.frame.size.height - insets.top - insets.bottom;
#endif

    //背景
    float x = width * 0.5f;
    float y = height * 0.5f;
    float fHeight = static_cast<float>(height) * 0.95f;
    float ratio = fHeight / static_cast<float>(_back.GetTextureId.height);
    float fWidth = static_cast<float>(_back.GetTextureId.width) * ratio;
    [_back resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];

    //モデル変更ボタン
    x = static_cast<float>(width - _gear.GetTextureId.width * 0.5f);
    y = static_cast<float>(height - _gear.GetTextureId.height * 0.5f);
    fWidth = static_cast<float>(_gear.GetTextureId.width);
    fHeight = static_cast<float>(_gear.GetTextureId.height);
    [_gear resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];

    //電源ボタン
    x = static_cast<float>(width - _power.GetTextureId.width * 0.5f);
    y = static_cast<float>(_power.GetTextureId.height * 0.5f);
    fWidth = static_cast<float>(_power.GetTextureId.width);
    fHeight = static_cast<float>(_power.GetTextureId.height);
    [_power resizeImmidiate:x Y:y Width:fWidth Height:fHeight MaxWidth:maxWidth MaxHeight:maxHeight];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

#if !TARGET_OS_MACCATALYST
    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;
#endif

    [_touchManager touchesBegan:point.x DeciveY:point.y];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:self.view];

#if !TARGET_OS_MACCATALYST
    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;
#endif

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
#if !TARGET_OS_MACCATALYST
    UIEdgeInsets insets = self.view.safeAreaInsets;
    point.x -= insets.left;
    point.y -= insets.top;
#endif
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
#if TARGET_OS_MACCATALYST
    float height = self.view.frame.size.height;
#else
    UIEdgeInsets insets = self.view.safeAreaInsets;
    float height = self.view.frame.size.height - insets.top - insets.bottom;
#endif
    return deviceY * -1 + height;
}

- (id <MTLDevice>)getDevice
{
    return _device;
}

- (void)drawableResize:(CGSize)size
{
    MTLTextureDescriptor* depthTextureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float width:size.width height:size.height mipmapped:false];
    depthTextureDescriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    depthTextureDescriptor.storageMode = MTLStorageModePrivate;

    if (_depthTexture)
    {
        [_depthTexture release];
    }
    _depthTexture = [_device newTextureWithDescriptor:depthTextureDescriptor];

    [self resizeScreen];
}


- (void)renderSprite:(id<MTLRenderCommandEncoder>)renderEncoder
{
    [_back renderImmidiate:renderEncoder];

    [_gear renderImmidiate:renderEncoder];

    [_power renderImmidiate:renderEncoder];
}

- (void)renderToMetalLayer:(nonnull CAMetalLayer *)layer
{
    LAppPal::UpdateTime();

    id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];
    id<CAMetalDrawable> currentDrawable = [layer nextDrawable];

    MTLRenderPassDescriptor *renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
    renderPassDescriptor.colorAttachments[0].texture = currentDrawable.texture;
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);

    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

    // セーフエリアのみ
    MTLViewport viewport = [self getSafeAreaViewport];
    [renderEncoder setViewport:viewport];

    //モデル以外の描画
    [self renderSprite:renderEncoder];

    [renderEncoder endEncoding];

    LAppLive2DManager* Live2DManager = [LAppLive2DManager getInstance];
    [Live2DManager SetViewMatrix:_viewMatrix];
    [Live2DManager onUpdate:commandBuffer currentDrawable:currentDrawable depthTexture:_depthTexture];

    [commandBuffer presentDrawable:currentDrawable];
    [commandBuffer commit];
}

- (void)dealloc
{
    [self releaseView];
    [super dealloc];
}

- (int)getWindowWidth
{
    return _windowWidth;
}

- (int)getWindowHeight;
{
    return _windowHeight;
}

- (MTLViewport) getSafeAreaViewport
{
    CGFloat scale = self.traitCollection.displayScale;
#if TARGET_OS_MACCATALYST
    MTLViewport viewport =  {0,0,
                             self.view.frame.size.width * scale,
                             self.view.frame.size.height * scale,
                             0.0,1.0};
#else
    UIEdgeInsets insets = self.view.safeAreaInsets;
    MTLViewport viewport =  {insets.left * scale, insets.top * scale,
                             (self.view.frame.size.width - insets.left - insets.right) * scale,
                             (self.view.frame.size.height - insets.top - insets.bottom) * scale,
                             0.0,1.0};
#endif
    return viewport;
}
@end
