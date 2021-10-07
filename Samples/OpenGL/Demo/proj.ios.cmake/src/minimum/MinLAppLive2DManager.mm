/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import "MinLAppLive2DManager.h"
#import "MinAppDelegate.h"
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
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    int width = screenRect.size.width;
    int height = screenRect.size.height;

    MinAppDelegate* delegate = (MinAppDelegate*) [[UIApplication sharedApplication] delegate];
    MinViewController* view = [delegate viewController];

    Csm::CubismMatrix44 projection;

    MinLAppModel* model = [self getModel];
    if (model->GetModel()->GetCanvasWidth() > 1.0f && width < height)
    {
      // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
      model->GetModelMatrix()->SetWidth(2.0f);
      projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
    }
    else
    {
      projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
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

