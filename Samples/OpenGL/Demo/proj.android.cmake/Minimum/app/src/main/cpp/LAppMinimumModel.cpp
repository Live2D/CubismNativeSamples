/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppMinimumModel.hpp"
#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppTextureManager.hpp"
#include "LAppMinimumDelegate.hpp"
#include "Motion/CubismBreathUpdater.hpp"
#include "Motion/CubismLookUpdater.hpp"
#include "Motion/CubismExpressionUpdater.hpp"
#include "Motion/CubismEyeBlinkUpdater.hpp"
#include "Motion/CubismPhysicsUpdater.hpp"
#include "Motion/CubismPoseUpdater.hpp"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace LAppDefine;

LAppMinimumModel::LAppMinimumModel()
    : LAppModel_Common()
    , _modelJson(nullptr)
    , _userTimeSeconds(0.0f)
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

LAppMinimumModel::LAppMinimumModel(const std::string modelDirectoryName,const std::string currentModelDirectory)
        : LAppModel_Common(),_modelDirName(modelDirectoryName), _currentModelDirectory(currentModelDirectory), _modelJson(nullptr), _userTimeSeconds(0.0f), _motionUpdated(false)
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

LAppMinimumModel::~LAppMinimumModel()
{
    ReleaseMotions();
    ReleaseExpressions();

    for (csmInt32 i = 0; i < _modelJson->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelJson->GetMotionGroupName(i);
        ReleaseMotionGroup(group);
    }
    delete _modelJson;
}

std::string LAppMinimumModel::MakeAssetPath(const std::string &assetFileName)
{
    return _currentModelDirectory + assetFileName;
}

void LAppMinimumModel::LoadAssets(const std::string & fiileName, const std::function<void(Csm::csmByte*, Csm::csmSizeInt)>& afterLoadCallback)
{
    Csm::csmSizeInt bufferSize = 0;
    Csm::csmByte* buffer = nullptr;

    if (fiileName.empty())
    {
        return;
    }

    // バッファの設定
    buffer = LAppPal::LoadFileAsBytes(MakeAssetPath(fiileName).c_str(), &bufferSize);

    // コールバック関数の呼び出し
    afterLoadCallback(buffer, bufferSize);

    // バッファの解放
    LAppPal::ReleaseBytes(buffer);
}


void LAppMinimumModel::SetupModel()
{
    _updating = true;
    _initialized = false;

    // モデルの設定データをJsonファイルから読み込み
    LoadAssets(_modelDirName + ".model3.json", [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) { _modelJson = new Csm::CubismModelSettingJson(buffer, bufferSize); });
    // モデルの設定データからモデルデータを読み込み
    LoadAssets(_modelJson->GetModelFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) { LoadModel(buffer, bufferSize); });

    // 表情データの読み込み
    for (auto expressionIndex = 0; expressionIndex < _modelJson->GetExpressionCount(); ++expressionIndex)
    {
        LoadAssets(_modelJson->GetExpressionFileName(expressionIndex), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
            auto expressionName = _modelJson->GetExpressionName(expressionIndex);
            ACubismMotion* motion = LoadExpression(buffer, bufferSize, expressionName);

            if (motion)
            {
                if (_expressions[expressionName])
                {
                    ACubismMotion::Delete(_expressions[expressionName]);
                    _expressions[expressionName] = nullptr;
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
    LoadAssets(_modelJson->GetPoseFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
        LoadPose(buffer, bufferSize);
    });
    if (_pose != nullptr)
    {
        CubismPoseUpdater* pose = CSM_NEW CubismPoseUpdater(*_pose);
        _updateScheduler.AddUpdatableList(pose);
    }

    // 物理演算データの読み込み
    LoadAssets(_modelJson->GetPhysicsFileName(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
        LoadPhysics(buffer, bufferSize);
    });
    if (_physics != nullptr)
    {
        CubismPhysicsUpdater* physics = CSM_NEW CubismPhysicsUpdater(*_physics);
        _updateScheduler.AddUpdatableList(physics);
    }

    // モデルに付属するユーザーデータの読み込み
    LoadAssets(_modelJson->GetUserDataFile(), [=](Csm::csmByte* buffer, Csm::csmSizeInt bufferSize) {
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
    _modelJson->GetLayoutMap(layout);
    // レイアウト情報から位置を設定
    _modelMatrix->SetupFromLayout(layout);

    // パラメータを保存
    _model->SaveParameters();

    // モーションデータの読み込み
    for (csmInt32 i = 0; i < _modelJson->GetMotionGroupCount(); i++)
    {
        const csmChar* group = _modelJson->GetMotionGroupName(i);
        // モーションデータをグループ名から一括でロードする
        PreloadMotionGroup(group);
    }

    _motionManager->StopAllMotions();

    // レンダラの作成
    CreateRenderer(LAppMinimumDelegate::GetInstance()->GetWindowWidth(), LAppMinimumDelegate::GetInstance()->GetWindowHeight());

    // テクスチャのセットアップ
    SetupTextures();

    _updating = false;
    _initialized = true;
}

void LAppMinimumModel::PreloadMotionGroup(const csmChar* group)
{
    const csmInt32 count = _modelJson->GetMotionCount(group);

    for (csmInt32 i = 0; i < count; i++)
    {
        //ex) idle_0
        csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, i);
        csmString path = _modelJson->GetMotionFileName(group, i);
        path = csmString(_currentModelDirectory.c_str()) + path;

        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]load motion: %s => [%s_%d] ", path.GetRawString(), group, i);
        }

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        CubismMotion* tmpMotion = static_cast<CubismMotion*>(LoadMotion(buffer, size, name.GetRawString(), NULL, NULL, _modelJson, group, i));

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

    CreateRenderer(LAppMinimumDelegate::GetInstance()->GetWindowWidth(), LAppMinimumDelegate::GetInstance()->GetWindowHeight());

    SetupTextures();
}

void LAppMinimumModel::ReleaseMotionGroup(const csmChar* group) const
{
    const csmInt32 count = _modelJson->GetMotionCount(group);
    for (csmInt32 i = 0; i < count; i++)
    {
        csmString voice = _modelJson->GetMotionSoundFileName(group, i);
        if (strcmp(voice.GetRawString(), "") != 0)
        {
            csmString path = voice;
            path = csmString(_currentModelDirectory.c_str()) + path;
        }
    }
}

/**
* @brief すべてのモーションデータの解放
*
* すべてのモーションデータを解放する。
*/
void LAppMinimumModel::ReleaseMotions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _motions.Begin(); iter != _motions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _motions.Clear();
}

/**
* @brief すべての表情データの解放
*
* すべての表情データを解放する。
*/
void LAppMinimumModel::ReleaseExpressions()
{
    for (csmMap<csmString, ACubismMotion*>::const_iterator iter = _expressions.Begin(); iter != _expressions.End(); ++iter)
    {
        ACubismMotion::Delete(iter->Second);
    }

    _expressions.Clear();
}

void LAppMinimumModel::Update()
{
    const csmFloat32 deltaTimeSeconds = LAppPal::GetDeltaTime();
    _userTimeSeconds += deltaTimeSeconds;

    // モーションによるパラメータ更新の有無
    _motionUpdated = false;

    //-----------------------------------------------------------------
    _model->LoadParameters(); // 前回セーブされた状態をロード
    if (_motionManager->IsFinished())
    {
        // モーションの再生がない場合、始めに登録されているモーションを再生する
        StartMotion(LAppDefine::MotionGroupIdle, 0, LAppDefine::PriorityIdle);
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

CubismMotionQueueEntryHandle LAppMinimumModel::StartMotion(const csmChar* group, csmInt32 no, csmInt32 priority)
{
    if (priority == PriorityForce)
    {
        _motionManager->SetReservePriority(priority);
    }
    else if (!_motionManager->ReserveMotion(priority))
    {
        if (_debugMode)
        {
            LAppPal::PrintLogLn("[APP]can't start motion.");
        }
        return InvalidMotionQueueEntryHandleValue;
    }

    const csmString fileName = _modelJson->GetMotionFileName(group, no);

    //ex) idle_0
    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);
    CubismMotion* motion = static_cast<CubismMotion*>(_motions[name.GetRawString()]);
    csmBool autoDelete = false;

    if (!motion)
    {
        csmString path = fileName;
        path = csmString(_currentModelDirectory.c_str()) + path;

        csmByte* buffer;
        csmSizeInt size;
        buffer = CreateBuffer(path.GetRawString(), &size);
        motion = static_cast<CubismMotion*>(LoadMotion(buffer, size, nullptr, NULL, NULL, _modelJson, group, no));

        if (motion)
        {
            autoDelete = true; // 終了時にメモリから削除
        }

        DeleteBuffer(buffer, path.GetRawString());
    }

    //voice
    csmString voice = _modelJson->GetMotionSoundFileName(group, no);
    if (strcmp(voice.GetRawString(), "") != 0)
    {
        csmString path = voice;
        path = csmString(_currentModelDirectory.c_str()) + path;
    }

    if (_debugMode)
    {
        LAppPal::PrintLogLn("[APP]start motion: [%s_%d]", group, no);
    }
    return  _motionManager->StartMotionPriority(motion, autoDelete, priority);
}

void LAppMinimumModel::Draw(CubismMatrix44& matrix)
{
    if (!_model)
    {
        return;
    }

    matrix.MultiplyByMatrix(_modelMatrix);

    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(&matrix);
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void LAppMinimumModel::SetExpression(const csmChar* expressionID)
{
    ACubismMotion* motion = _expressions[expressionID];
    if (_debugMode)
    {
        LAppPal::PrintLogLn("[APP]expression: [%s]", expressionID);
    }

    if (motion)
    {
        _expressionManager->StartMotion(motion, false);
    }
    else
    {
        if (_debugMode) LAppPal::PrintLogLn("[APP]expression[%s] is null ", expressionID);
    }
}

void LAppMinimumModel::ReloadRenderer()
{
    DeleteRenderer();

    CreateRenderer(LAppMinimumDelegate::GetInstance()->GetWindowWidth(), LAppMinimumDelegate::GetInstance()->GetWindowHeight());

    SetupTextures();
}

void LAppMinimumModel::SetupTextures()
{
    for (csmInt32 modelTextureNumber = 0; modelTextureNumber < _modelJson->GetTextureCount(); modelTextureNumber++)
    {
        // テクスチャ名が空文字だった場合はロード・バインド処理をスキップ
        if (strcmp(_modelJson->GetTextureFileName(modelTextureNumber), "") == 0)
        {
            continue;
        }

        //OpenGLのテクスチャユニットにテクスチャをロードする
        csmString texturePath = _modelJson->GetTextureFileName(modelTextureNumber);
        texturePath = csmString(_currentModelDirectory.c_str()) + texturePath;

        LAppTextureManager::TextureInfo* texture = LAppMinimumDelegate::GetInstance()->GetTextureManager()->CreateTextureFromPngFile(texturePath.GetRawString());
        const csmInt32 glTextueNumber = texture->id;

        //OpenGL
        GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTextureNumber, glTextueNumber);
    }

#ifdef PREMULTIPLIED_ALPHA_ENABLE
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(true);
#else
    GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);
#endif
}

void LAppMinimumModel::MotionEventFired(const csmString& eventValue)
{
    CubismLogInfo("%s is fired on LAppMinimumModel!!", eventValue.GetRawString());
}

Csm::Rendering::CubismRenderTarget_OpenGLES2 &LAppMinimumModel::GetRenderBuffer()
{
    return _renderBuffer;
}
