/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <Foundation/Foundation.h>
#import "MinLAppModel.h"
#import <fstream>
#import <vector>
#import <CubismModelSettingJson.hpp>
#import <Motion/CubismMotion.hpp>
#import <Physics/CubismPhysics.hpp>
#import <CubismDefaultParameterId.hpp>
#import <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#import <Utils/CubismString.hpp>
#import <Id/CubismIdManager.hpp>
#import <Motion/CubismMotionQueueEntry.hpp>
#import "MinLAppDefine.h"
#import "MinLAppPal.h"
#import "MinLAppTextureManager.h"
#import "MinAppDelegate.h"
#import "MinSceneDelegate.h"
#import "MinViewController.h"
#import "Motion/CubismBreathUpdater.hpp"
#import "Motion/CubismLookUpdater.hpp"
#import "Motion/CubismExpressionUpdater.hpp"
#import "Motion/CubismEyeBlinkUpdater.hpp"
#import "Motion/CubismPhysicsUpdater.hpp"
#import "Motion/CubismPoseUpdater.hpp"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace MinLAppDefine;

namespace {
    csmByte* CreateBuffer(const csmChar* path, csmSizeInt* size)
    {
        if (DebugLogEnable)
        {
            MinLAppPal::PrintLog("[APP]create buffer: %s ", path);
        }
        return MinLAppPal::LoadFileAsBytes(path,size);
    }

    void DeleteBuffer(csmByte* buffer, const csmChar* path = "")
    {
        if (DebugLogEnable)
        {
            MinLAppPal::PrintLog("[APP]delete buffer: %s", path);
        }
        MinLAppPal::ReleaseBytes(buffer);
    }
}

MinLAppModel::MinLAppModel(const std::string modelDirectryName,const std::string currentModelDirectry)
: CubismUserModel()
, _modelSetting(NULL)
, _userTimeSeconds(0.0f)
,_modelDirName(modelDirectryName)
,_currentModelDirectory(currentModelDirectry)
, _motionUpdated(false)
{
    if (DebugLogEnable)
    {
        _debugMode = true;
    }

    _idParamAngleX = CubismFramework::GetIdManager()->GetId(ParamAngleX);
    _idParamAngleY = CubismFramework::GetIdManager()->GetId(ParamAngleY);
    _idParamAngleZ = CubismFramework::GetIdManager()->GetId(ParamAngleZ);
    _idParamBodyAngleX = CubismFramework::GetIdManager()->GetId(ParamBodyAngleX);
    _idParamEyeBallX = CubismFramework::GetIdManager()->GetId(ParamEyeBallX);
    _idParamEyeBallY = CubismFramework::GetIdManager()->GetId(ParamEyeBallY);
}

MinLAppModel::~MinLAppModel()
{
    _renderBuffer.DestroyRenderTarget();

    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }
    delete _modelSetting;
}

std::string MinLAppModel::MakeAssetPath(const std::string &assetFileName)
{
    return _currentModelDirectory + assetFileName;
}

void MinLAppModel::LoadAssets(const std::string &fileName, const std::function<void (Csm::csmByte *, Csm::csmSizeInt)> &afterLoadCallback)
{
    Csm::csmSizeInt bufferSize = 0;
    Csm::csmByte* buffer = nil;

    if (fileName.empty())
    {
        return;
    }

    // バッファの設定
    buffer = MinLAppPal::LoadFileAsBytes(MakeAssetPath(fileName).c_str(),&bufferSize);

    // コールバック関数の呼び出し
    afterLoadCallback(buffer,bufferSize);

    // バッファの解放
    MinLAppPal::ReleaseBytes(buffer);
}


void MinLAppModel::SetupModel()
{
    _updating = true;
    _initialized = false;

    csmByte* buffer;
    csmSizeInt size;

    // モデルの設定データをJsonファイルから読み込み
    LoadAssets(_modelDirName + ".model3.json", [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) { _modelSetting = new Csm::CubismModelSettingJson(buffer, bufferSize); });
    // モデルの設定データからモデルデータを読み込み
    LoadAssets(_modelSetting->GetModelFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) { LoadModel(buffer, bufferSize); });

    // 表情データの読み込み
    for (auto expressionIndex = 0; expressionIndex < _modelSetting->GetExpressionCount(); ++expressionIndex)
    {
        LoadAssets(_modelSetting->GetExpressionFileName(expressionIndex), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
            auto expressionName = _modelSetting->GetExpressionName(expressionIndex);
            ACubismMotion* motion = LoadExpression(buffer, bufferSize, expressionName);

            if (motion)
            {
                if (_expressions[expressionName])
                {
                    ACubismMotion::Delete(_expressions[expressionName]);
                    _expressions[expressionName] = NULL;
                }
                _expressions[expressionName] = motion;
            }
        });
    }
    {
        CubismExpressionUpdater* expression = CSM_NEW CubismExpressionUpdater(*_expressionManager);
        _updateScheduler.AddUpdatableList(expression);
    }

    //ポーズデータの読み込み
    LoadAssets(_modelSetting->GetPoseFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
        LoadPose(buffer, bufferSize);
    });
    if (_pose != NULL)
    {
        CubismPoseUpdater* pose = CSM_NEW CubismPoseUpdater(*_pose);
        _updateScheduler.AddUpdatableList(pose);
    }

    // 物理演算データの読み込み
    LoadAssets(_modelSetting->GetPhysicsFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
        LoadPhysics(buffer, bufferSize);
    });
    if (_physics != NULL)
    {
        CubismPhysicsUpdater* physics = CSM_NEW CubismPhysicsUpdater(*_physics);
        _updateScheduler.AddUpdatableList(physics);
    }

    // モデルに付属するユーザーデータの読み込み
    LoadAssets(_modelSetting->GetUserDataFile(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
        LoadUserData(buffer, bufferSize);
    });

    // Look
    {
        _look = CubismLook::Create();

        csmVector<CubismLook::LookParameterData> lookParameters;

        lookParameters.PushBack(CubismLook::LookParameterData(_idParamAngleX, 30.0f));
        lookParameters.PushBack(CubismLook::LookParameterData(_idParamAngleY, 0.0f, 30.0f));
        lookParameters.PushBack(CubismLook::LookParameterData(_idParamAngleZ, 0.0f, 0.0f, -30.0f));
        lookParameters.PushBack(CubismLook::LookParameterData(_idParamBodyAngleX, 10.0f));
        lookParameters.PushBack(CubismLook::LookParameterData(_idParamEyeBallX, 1.0f));
        lookParameters.PushBack(CubismLook::LookParameterData(_idParamEyeBallY, 0.0f, 1.0f));

        _look->SetParameters(lookParameters);

        CubismLookUpdater* look = CSM_NEW CubismLookUpdater(*_look, *_dragManager);
        _updateScheduler.AddUpdatableList(look);
    }

    _updateScheduler.SortUpdatableList();

    // Layout
    csmMap<csmString, csmFloat32> layout;
    _modelSetting->GetLayoutMap(layout);
    // レイアウト情報から位置を設定
    _modelMatrix->SetupFromLayout(layout);

    // パラメータを保存
    _model->SaveParameters();

    // モーションデータの読み込み
    for (csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelSetting->GetMotionGroupName(i);
        // モーションデータをグループ名から一括でロードする
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    MinAppDelegate *appDelegate = (MinAppDelegate *) [[UIApplication sharedApplication] delegate];
    MinSceneDelegate* sceneDelegate = [appDelegate getActiveMinSceneDelegate];
    MinViewController* view = [sceneDelegate viewController];
    int width = [view GetWindowWidth];
    int height = [view GetWindowHeight];

    // レンダラの作成
    CreateRenderer(width, height);

    // テクスチャのセットアップ
    SetupTextures();

    _updating = false;
    _initialized = true;
}

void MinLAppModel::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        //ex) idle_0
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = _modelSetting->GetMotionFileName(group, i);
        path = Csm::csmString(_currentModelDirectory.c_str()) + path;

        if (_debugMode)
        {
            MinLAppPal::PrintLog("[APP]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
        }

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString(), NULL, NULL, _modelSetting, group, i));

        if (tmpMotion)
        {
            if (_motions[name] != NULL)
            {
                ACubismMotion::Delete(_motions[name]);
            }
            _motions[name] = tmpMotion;
        }

        DeleteBuffer(buffer, path.GetRawString());
    }
}

void MinLAppModel::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = _modelSetting->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = _modelSetting->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = Csm::csmString(_currentModelDirectory.c_str()) + path;
        }
    }
}

void MinLAppModel::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _motions.Clear();
}

void MinLAppModel::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _expressions.Clear();
}

void MinLAppModel::Update()
{
    const csmFloat32 deltaTimeSeconds = MinLAppPal::GetDeltaTime();
    _userTimeSeconds += deltaTimeSeconds;

    // モーションによるパラメータ更新の有無
    _motionUpdated = false;

    //-----------------------------------------------------------------
    _model->LoadParameters(); // 前回セーブされた状態をロード
    if (_motionManager->IsFinished())
    {
        // モーションの再生がない場合、最初に登録されているモーションを再生する
        StartMotion(MotionGroupIdle, 0, PriorityIdle);
    }
    else
    {
        _motionUpdated = _motionManager->UpdateMotion(_model, deltaTimeSeconds); // モーションを更新
    }
    _model->SaveParameters(); // 状態を保存
    //-----------------------------------------------------------------

    _updateScheduler.OnLateUpdate(_model, deltaTimeSeconds);

    _model->Update();
}

CubismMotionQueueEntryHandle MinLAppModel::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority)
{
    if (priority == PriorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        if (_debugMode)
        {
            MinLAppPal::PrintLog("[APP]can't start motion.");
        }
        return InvalidMotionQueueEntryHandleValue;
    }

    const csmString fileName = _modelSetting->GetMotionFileName(group, no);

    //ex) idle_0
    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (motion == NULL)
    {
        csmString path = fileName;
        path = Csm::csmString(_currentModelDirectory.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, NULL, NULL, NULL, _modelSetting, group, no));

        if (motion)
        {
            autoDelete = true; // 終了時にメモリから削除
        }

        DeleteBuffer(buffer, path.GetRawString());
    }

    if (_debugMode)
    {
        MinLAppPal::PrintLog("[APP]start motion: [%s_%d]", group, no);
    }
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

void MinLAppModel::Draw(CubismMatrix44& matrix)
{
    if (_model == NULL)
    {
        return;
    }

    matrix.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&matrix);
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void MinLAppModel::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = _expressions[expressionID];
    if (_debugMode)
    {
        MinLAppPal::PrintLog("[APP]expression: [%s]", expressionID);
    }

    if (motion != NULL)
    {
        _expressionManager->StartMotion(motion, false);
    }
    else
    {
        if (_debugMode)
        {
            MinLAppPal::PrintLog("[APP]expression[%s] is null ", expressionID);
        }
    }
}
void MinLAppModel::ReloadRenderer()
{
    DeleteRenderer();

    MinAppDelegate *appDelegate = (MinAppDelegate *) [[UIApplication sharedApplication] delegate];
    MinSceneDelegate* sceneDelegate = [appDelegate getActiveMinSceneDelegate];
    MinViewController* view = [sceneDelegate viewController];
    int width = [view GetWindowWidth];
    int height = [view GetWindowHeight];

    CreateRenderer(width, height);

    SetupTextures();
}

void MinLAppModel::SetupTextures()
{
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelSetting->GetTextureCount(); modelTextureNumber++)
    {
        // テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
        if (strcmp(_modelSetting->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        //OpenGLのテクスチャユニットにテクスチャをロードする
        csmString texturePath = _modelSetting->GetTextureFileName(modelTextureNumber);
        texturePath = Csm::csmString(_currentModelDirectory.c_str()) + texturePath;

        MinAppDelegate *appDelegate = (MinAppDelegate *) [[UIApplication sharedApplication] delegate];
        MinSceneDelegate* sceneDelegate = [appDelegate getActiveMinSceneDelegate];
        TextureInfo* texture = [[sceneDelegate getTextureManager] createTextureFromPngFile:texturePath.GetRawString()];
        csmInt32 glTextueNumber = texture->id;

        //OpenGL
        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
    }

#ifdef PREMULTIPLIED_ALPHA_ENABLE
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif
}

void MinLAppModel::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on MinLAppModel!!", eventValue.GetRawString());
}

Csm::Rendering::CubismRenderTarget_OpenGLES2& MinLAppModel::GetRenderBuffer()
{
    return _renderBuffer;
}
