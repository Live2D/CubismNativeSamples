/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "MinLAppLive2DManager.h"
#import <GLKit/GLKit.h>
#import <Rendering/OpenGL/CubismOffscreenManager_OpenGLES2.hpp>
#import "MinAppDelegate.h"
#import "MinSceneDelegate.h"
#import "MinViewController.h"
#import "MinLAppModel.h"
#import "MinLAppDefine.h"
#import "MinLAppPal.h"

@interface MinLAppLive2DManager()

- (id)init;
- (void)dealloc;
@end

@implementation MinLAppLive2DManager

static MinLAppLive2DManager* s_instance = nil;


void FinishedMotion(Csm::ACubismMotion* self)
{
    MinLAppPal::PrintLog("Motion Finished: %x", self);
}

+ (MinLAppLive2DManager*)getInstance
{
    @synchronized(self)
    {
        if(s_instance == nil)
        {
            s_instance = [[MinLAppLive2DManager alloc] init];
        }
    }
    return s_instance;
}

+ (void)releaseInstance
{
    if(s_instance != nil)
    {
        s_instance = nil;
    }
}

- (id)init
{
    self = [super init];
    if ( self ) {
        _viewMatrix = nil;

        _viewMatrix = new Csm::CubismMatrix44();
        [self LoadModel];
    }
    return self;
}

- (void)dealloc
{
    [self releaseModel];
    Csm::Rendering::CubismOffscreenManager_OpenGLES2::ReleaseInstance();
}

- (void)releaseModel
{
    delete _model;
    _model=nil;
}

- (MinLAppModel*)getModel
{
    return _model;
}

- (void)onDrag:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y
{
    Csm::CubismUserModel* model = static_cast<Csm::CubismUserModel*>([self getModel]);
    model->SetDragging(x,y);
}

- (void)onTap:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y;
{
    if (MinLAppDefine::DebugLogEnable)
    {
        MinLAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }
}

- (void)onUpdate;
{
    // モデルで使用するオフスクリーン管理の開始処理
    Csm::Rendering::CubismOffscreenManager_OpenGLES2::GetInstance()->BeginFrameProcess();

    MinAppDelegate *appDelegate = (MinAppDelegate *) [[UIApplication sharedApplication] delegate];
    MinSceneDelegate* sceneDelegate = [appDelegate getActiveMinSceneDelegate];
    MinViewController* view = [sceneDelegate viewController];

    int width = [view GetWindowWidth];
    int height = [view GetWindowHeight];
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float displayRatio = static_cast<float>(height) / static_cast<float>(width);

    Csm::CubismMatrix44 projection;

    MinLAppModel* model = [self getModel];

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

    [view PreModelDraw:*model];

    model->Update();
    model->Draw(projection);///< 参照渡しなのでprojectionは変質する

    [view PostModelDraw:*model];

    // モデルで使用するオフスクリーン管理の終了処理
    Csm::Rendering::CubismOffscreenManager_OpenGLES2::GetInstance()->EndFrameProcess();
    // もし余っているオフスクリーンのリソースを解放したい場合行う処理
    Csm::Rendering::CubismOffscreenManager_OpenGLES2::GetInstance()->ReleaseStaleRenderTextures();
}

- (void)SetViewMatrix:(Csm::CubismMatrix44*)m;
{
    for (int i = 0; i < 16; i++) {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
    }
}

- (void)SetAssetDirectry:(const std::string)path
{
    _currentModelDictionary = path;
}

- (void)LoadModel
{
    // モデルのディレクトリを指定
    const std::string modelDirName = MinLAppDefine::modelDirectryName;
    [self SetAssetDirectry:(MinLAppDefine::ResourcesPath + modelDirName + "/")];

    // モデルデータの新規作成
    _model = new MinLAppModel(modelDirName,_currentModelDictionary);

    // モデルデータの読み込み及び生成とセットアップを行う
    static_cast<MinLAppModel*>(_model)->SetupModel();
}

@end

