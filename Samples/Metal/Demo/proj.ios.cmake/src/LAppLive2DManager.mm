/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "LAppLive2DManager.h"
#import <string.h>
#import <stdlib.h>
#import <Foundation/Foundation.h>
#import "AppDelegate.h"
#import "SceneDelegate.h"
#import "ViewController.h"
#import "LAppModel.h"
#import "LAppDefine.h"
#import "LAppPal.h"
#import <Rendering/Metal/CubismRenderer_Metal.hpp>

@interface LAppLive2DManager()

- (id)init;
- (void)dealloc;
@end

@implementation LAppLive2DManager

static LAppLive2DManager* s_instance = nil;

void BeganMotion(Csm::ACubismMotion* self)
{
    LAppPal::PrintLogLn("Motion began: %x", self);
}

void FinishedMotion(Csm::ACubismMotion* self)
{
    LAppPal::PrintLogLn("Motion Finished: %x", self);
}

int CompareCsmString(const void* a, const void* b)
{
    return strcmp(reinterpret_cast<const Csm::csmString*>(a)->GetRawString(),
        reinterpret_cast<const Csm::csmString*>(b)->GetRawString());
}

Csm::csmString GetPath(CFURLRef url)
{
  CFStringRef cfstr = CFURLCopyFileSystemPath(url, CFURLPathStyle::kCFURLPOSIXPathStyle);
  CFIndex size = CFStringGetLength(cfstr) * 4 + 1; // Length * UTF-16 Max Character size + null-terminated-byte
  char* buf = new char[size];
  CFStringGetCString(cfstr, buf, size, CFStringBuiltInEncodings::kCFStringEncodingUTF8);
  Csm::csmString result(buf);
  delete[] buf;
  return result;
}

+ (LAppLive2DManager*)getInstance
{
    @synchronized(self)
    {
        if (s_instance == nil)
        {
            s_instance = [[LAppLive2DManager alloc] init];
        }
    }
    return s_instance;
}

+ (void)releaseInstance
{
    if (s_instance != nil)
    {
        [s_instance release];
        s_instance = nil;
    }
}

- (id)init
{
    self = [super init];
    if ( self ) {
        _renderBuffer = nil;
        _modelSprite = nil;
        _sprite = nil;
        _viewMatrix = nil;
        _sceneIndex = 0;

        _viewMatrix = new Csm::CubismMatrix44();

        _renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
        _renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        _renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.f, 0.f, 0.f, 0.f);
        _renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        _renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
        _renderPassDescriptor.depthAttachment.clearDepth = 1.0;

        [self setUpModel];

        [self changeScene:_sceneIndex];
    }
    return self;
}

- (void)dealloc
{
    if (_renderBuffer)
    {
        _renderBuffer->DestroyRenderTarget();
        delete _renderBuffer;
        _renderBuffer = NULL;
    }

    if (_renderPassDescriptor != nil)
    {
        [_renderPassDescriptor release];
        _renderPassDescriptor = nil;
    }

    if (_modelSprite != nil)
    {
        [_modelSprite release];
        _modelSprite = nil;
    }

    if (_sprite != nil)
    {
       [_sprite release];
        _sprite = nil;
    }

    delete _viewMatrix;
    _viewMatrix = nil;

    Csm::Rendering::CubismDeviceInfo_Metal::ReleaseAllDeviceInfo();

    [self releaseAllModel];
    [super dealloc];
}

- (void)releaseAllModel
{
    for (Csm::csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        delete _models[i];
    }

    _models.Clear();
}

- (void)setUpModel
{
    _modelDir.Clear();

    NSBundle* bundle = [NSBundle mainBundle];
    NSString* resPath = [NSString stringWithUTF8String:LAppDefine::ResourcesPath];
    NSArray* resArr = [bundle pathsForResourcesOfType:NULL inDirectory:resPath];
    NSUInteger cnt = [resArr count];

    for (NSUInteger i = 0; i < cnt; i++)
    {
        NSString* modelName = [[resArr objectAtIndex:i] lastPathComponent];
        NSMutableString* modelDirPath = [NSMutableString stringWithString:resPath];
        [modelDirPath appendString:@"/"];
        [modelDirPath appendString:modelName];
        NSArray* model3json = [bundle pathsForResourcesOfType:@".model3.json" inDirectory:modelDirPath];
        if ([model3json count] == 1)
        {
            _modelDir.PushBack(Csm::csmString([modelName UTF8String]));
        }
    }
    qsort(_modelDir.GetPtr(), _modelDir.GetSize(), sizeof(Csm::csmString), CompareCsmString);
}

- (LAppModel*)getModel:(Csm::csmUint32)no
{
    if (no < _models.GetSize())
    {
        return _models[no];
    }
    return nil;
}

- (void)setRenderTargetSize:(Csm::csmUint32)width height:(Csm::csmUint32)height
{
    for(Csm::csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel* model = [self getModel:i];
        model->SetRenderTargetSize(width, height);
    }
}

- (void)onDrag:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y
{
    for (Csm::csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        Csm::CubismUserModel* model = static_cast<Csm::CubismUserModel*>([self getModel:i]);
        model->SetDragging(x,y);
    }
}

- (void)onTap:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y;
{
    AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
    SceneDelegate* sceneDelegate = [appDelegate getActiveSceneDelegate];
    ViewController* view = [sceneDelegate viewController];
    int width = [view getWindowWidth];
    int height = [view getWindowHeight];
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float displayRatio = static_cast<float>(height) / static_cast<float>(width);

    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }

    for (Csm::csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        float canvasRatio = _models[i]->GetModel()->GetCanvasHeight() / _models[i]->GetModel()->GetCanvasWidth();

        float adjustedX = x;
        float adjustedY = y;

        if (canvasRatio < displayRatio)
        {
            // OnUpdateでのプロジェクションスケールを打ち消してモデル座標系に変換
            adjustedX = x / aspectRatio;
            adjustedY = y / aspectRatio;
        }

        if (_models[i]->HitTest(LAppDefine::HitAreaNameHead,adjustedX,adjustedY))
        {
            if (LAppDefine::DebugLogEnable)
            {
                LAppPal::PrintLogLn("[APP]hit area: [%s]", LAppDefine::HitAreaNameHead);
            }
            _models[i]->SetRandomExpression();
        }
        else if (_models[i]->HitTest(LAppDefine::HitAreaNameBody, adjustedX, adjustedY))
        {
            if (LAppDefine::DebugLogEnable)
            {
                LAppPal::PrintLogLn("[APP]hit area: [%s]", LAppDefine::HitAreaNameBody);
            }
            _models[i]->StartRandomMotion(LAppDefine::MotionGroupTapBody, LAppDefine::PriorityNormal, FinishedMotion, BeganMotion);
        }
    }
}

- (void)onUpdate:(id <MTLCommandBuffer>)commandBuffer currentDrawable:(id<CAMetalDrawable>)drawable depthTexture:(id<MTLTexture>)depthTarget;
{
    AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
    SceneDelegate* sceneDelegate = [appDelegate getActiveSceneDelegate];
    ViewController* view = [sceneDelegate viewController];

    int width = [view getWindowWidth];
    int height = [view getWindowHeight];

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float displayRatio = static_cast<float>(height) / static_cast<float>(width);

    Csm::csmUint32 modelCount = _models.GetSize();

    id<MTLDevice> device = [view getDevice];
    Csm::Rendering::CubismDeviceInfo_Metal* deviceInfo = Csm::Rendering::CubismDeviceInfo_Metal::GetDeviceInfo(device);

    // モデルで使用するオフスクリーン管理の開始処理
    deviceInfo->GetOffscreenManager()->BeginFrameProcess();

    _renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
    _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
    _renderPassDescriptor.depthAttachment.texture = depthTarget;

    if (_renderTarget != SelectTarget_None)
    {
        if (!_renderBuffer)
        {
            _renderBuffer = new Csm::Rendering::CubismRenderTarget_Metal;
            _renderBuffer->SetMTLPixelFormat(MTLPixelFormatBGRA8Unorm);
            _renderBuffer->CreateRenderTarget(device, static_cast<LAppDefine::csmUint32>(width), static_cast<LAppDefine::csmUint32>(height));

            if (_renderTarget == SelectTarget_ViewFrameBuffer)
            {
                _sprite = [[LAppSprite alloc] initWithMyVar:width * 0.5f Y:height * 0.5f Width:width Height:height
                                                   MaxWidth:width MaxHeight:height Texture:_renderBuffer->GetColorBuffer()];
                _modelSprite = [[LAppModelSprite alloc] initWithMyVar:width * 0.5f Y:height * 0.5f Width:width Height:height
                                                   MaxWidth:width MaxHeight:height Texture:_renderBuffer->GetColorBuffer()];
            }
        }
        else if (_renderBuffer->GetBufferWidth() != static_cast<LAppDefine::csmUint32>(width) ||
                 _renderBuffer->GetBufferHeight() != static_cast<LAppDefine::csmUint32>(height))
        {
            // 画面サイズが変わった場合はレンダーバッファを再生成する
            _renderBuffer->CreateRenderTarget(device, static_cast<LAppDefine::csmUint32>(width), static_cast<LAppDefine::csmUint32>(height));

            if (_renderTarget == SelectTarget_ViewFrameBuffer)
            {
                // スプライトをレンダーバッファの新しいテクスチャで再生成する
                [_sprite release];
                _sprite = nil;
                [_modelSprite release];
                _modelSprite = nil;
                _sprite = [[LAppSprite alloc] initWithMyVar:width * 0.5f Y:height * 0.5f Width:width Height:height
                                                   MaxWidth:width MaxHeight:height Texture:_renderBuffer->GetColorBuffer()];
                _modelSprite = [[LAppModelSprite alloc] initWithMyVar:width * 0.5f Y:height * 0.5f Width:width Height:height
                                                   MaxWidth:width MaxHeight:height Texture:_renderBuffer->GetColorBuffer()];
            }
        }

        if (_renderTarget == SelectTarget_ViewFrameBuffer)
        {
            _renderPassDescriptor.colorAttachments[0].texture = _renderBuffer->GetColorBuffer();
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        }

        //画面クリア
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:_renderBuffer->GetRenderPassDescriptor()];
        [renderEncoder endEncoding];

        if (_renderTarget == SelectTarget_ViewFrameBuffer)
        {
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
        }
    }

    for (Csm::csmUint32 i = 0; i < modelCount; ++i)
    {
        LAppModel* model = [self getModel:i];

        if (model->GetModel() == NULL)
        {
            LAppPal::PrintLogLn("Failed to model->GetModel().");
            continue;
        }

        Csm::CubismMatrix44 projection;

        model->GetRenderer<Csm::Rendering::CubismRenderer_Metal>()->StartFrame(commandBuffer, _renderPassDescriptor);
        if (_renderTarget != SelectTarget_None)
        {
            MTLViewport viewport = {0, 0, static_cast<double>(width), static_cast<double>(height), 0.0, 1.0};
            model->GetRenderer<Csm::Rendering::CubismRenderer_Metal>()->SetRenderViewport(viewport);
        }
        else
        {
            MTLViewport safeAreaViewport = [view getSafeAreaViewport];
            model->GetRenderer<Csm::Rendering::CubismRenderer_Metal>()->SetRenderViewport(safeAreaViewport);
        }

        float canvasRatio = model->GetModel()->GetCanvasHeight() / model->GetModel()->GetCanvasWidth();
        if (canvasRatio < displayRatio)
        {
            // 横長モデルを幅に合わせて縦方向のスケールを調整
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, aspectRatio);
        }
        else
        {
            // 縦長モデルを高さに合わせて横方向のスケールを調整
            model->GetModelMatrix()->SetHeight(2.0f);
            projection.Scale(1.0f / aspectRatio, 1.0f);
        }

        // 必要があればここで乗算
        if (_viewMatrix != NULL)
        {
            projection.MultiplyByMatrix(_viewMatrix);
        }

        if (_renderTarget == SelectTarget_ModelFrameBuffer)
        {
            Csm::Rendering::CubismRenderTarget_Metal& useTarget = model->GetRenderBuffer();

            if (!useTarget.IsValid())
            {// 描画ターゲット内部未作成の場合はここで作成
                // モデル描画キャンバス
                useTarget.SetMTLPixelFormat(MTLPixelFormatBGRA8Unorm);
                useTarget.CreateRenderTarget(device, static_cast<LAppDefine::csmUint32>(width), static_cast<LAppDefine::csmUint32>(height));
            }
            else if (useTarget.GetBufferWidth() != static_cast<LAppDefine::csmUint32>(width) || useTarget.GetBufferHeight() != static_cast<LAppDefine::csmUint32>(height))
            {
                useTarget.CreateRenderTarget(device, static_cast<LAppDefine::csmUint32>(width), static_cast<LAppDefine::csmUint32>(height));
            }

            _renderPassDescriptor.colorAttachments[0].texture = useTarget.GetColorBuffer();
            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;

            //画面クリア
            id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:_renderPassDescriptor];
            [renderEncoder endEncoding];

            _renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
        }

        model->Update();
        model->Draw(projection);///< 参照渡しなのでprojectionは変質する

        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderBuffer && _modelSprite)
        {
            MTLRenderPassDescriptor *renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
            id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

            MTLViewport safeAreaViewport = [view getSafeAreaViewport];
            [renderEncoder setViewport:safeAreaViewport];

            float alpha = 0.4f;
            [_modelSprite SetColor:1.0f * alpha g:1.0f * alpha b:1.0f * alpha a:alpha];
            [_modelSprite renderImmidiate:renderEncoder];
            [renderEncoder endEncoding];
        }

        // 各モデルが持つ描画ターゲットをテクスチャとする場合はスプライトへの描画はここ
        if (_renderTarget == SelectTarget_ModelFrameBuffer)
        {
            if (!model)
            {
                return;
            }

            MTLRenderPassDescriptor *renderPassDescriptor = [[[MTLRenderPassDescriptor alloc] init] autorelease];
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
            id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

            MTLViewport safeAreaViewport = [view getSafeAreaViewport];
            [renderEncoder setViewport:safeAreaViewport];

            Csm::Rendering::CubismRenderTarget_Metal& useTarget = model->GetRenderBuffer();
            LAppModelSprite* depthSprite = [[LAppModelSprite alloc] initWithMyVar:width * 0.5f Y:height * 0.5f Width:width Height:height
                                                               MaxWidth:width MaxHeight:height Texture:useTarget.GetColorBuffer()];
            float a = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
            [depthSprite SetColor:1.0f * a g:1.0f * a b:1.0f * a a:a];
            [depthSprite renderImmidiate:renderEncoder];
            [renderEncoder endEncoding];
            [depthSprite release];
        }
    }

    // モデルで使用するオフスクリーン管理の終了処理
    deviceInfo->GetOffscreenManager()->EndFrameProcess();
    // もし余っているオフスクリーンのリソースを解放したい場合行う処理
    deviceInfo->GetOffscreenManager()->ReleaseStaleRenderTextures();
}

- (void)nextScene;
{
    Csm::csmInt32 no = (_sceneIndex + 1) % _modelDir.GetSize();
    [self changeScene:no];
}

- (void)changeScene:(Csm::csmInt32)index;
{
    _sceneIndex = index;
    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]model index: %d", _sceneIndex);
    }

    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    const Csm::csmString& model = _modelDir[index];

    Csm::csmString modelPath(LAppDefine::ResourcesPath);
    modelPath += model;
    modelPath.Append(1, '/');

    Csm::csmString modelJsonName(model);
    modelJsonName += ".model3.json";

    [self releaseAllModel];
    _models.PushBack(new LAppModel());
    _models[0]->LoadAssets(modelPath.GetRawString(), modelJsonName.GetRawString());

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // LAppViewの持つターゲットに描画を行う場合、こちらを選択
        SelectTarget useRenderTarget = SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        SelectTarget useRenderTarget = SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        SelectTarget useRenderTarget = SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new LAppModel());
        _models[1]->LoadAssets(modelPath.GetRawString(), modelJsonName.GetRawString());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        float clearColorR = 0.0f;
        float clearColorG = 0.0f;
        float clearColorB = 0.0f;

        AppDelegate *appDelegate = (AppDelegate *) [[UIApplication sharedApplication] delegate];
        SceneDelegate* sceneDelegate = [appDelegate getActiveSceneDelegate];
        ViewController* view = [sceneDelegate viewController];

        [self SwitchRenderingTarget:useRenderTarget];
        [self SetRenderTargetClearColor:clearColorR g:clearColorG b:clearColorB];
    }
}

- (Csm::csmUint32)GetModelNum;
{
    return _models.GetSize();
}

- (void)SetViewMatrix:(Csm::CubismMatrix44*)m;
{
    for (int i = 0; i < 16; i++) {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
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
@end
