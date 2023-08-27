/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppMinimumLive2DManager.hpp"
#include <string>
#include <GLES2/gl2.h>
#include <Rendering/CubismRenderer.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppMinimumDelegate.hpp"
#include "LAppMinimumModel.hpp"
#include "LAppMinimumView.hpp"

using namespace Csm;
using namespace LAppDefine;
using namespace std;

namespace {
    LAppMinimumLive2DManager* s_instance = nullptr;
}

LAppMinimumLive2DManager* LAppMinimumLive2DManager::GetInstance()
{
    if (!s_instance)
    {
        s_instance = new LAppMinimumLive2DManager();
    }

    return s_instance;
}

void LAppMinimumLive2DManager::ReleaseInstance()
{
    if (s_instance)
    {
        delete s_instance;
    }

    s_instance = nullptr;
}

LAppMinimumLive2DManager::LAppMinimumLive2DManager()
{
    _viewMatrix = new CubismMatrix44();
    LoadModel(_modelDirectoryName);
}

LAppMinimumLive2DManager::~LAppMinimumLive2DManager()
{
    ReleaseModel();
    delete _viewMatrix;
}

void LAppMinimumLive2DManager::ReleaseModel()
{
    delete _model;
}

LAppMinimumModel* LAppMinimumLive2DManager::GetModel() const
{
    return _model;
}

void LAppMinimumLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    LAppMinimumModel* model = GetModel();
    model->SetDragging(x, y);
}

void LAppMinimumLive2DManager::OnUpdate() const
{
    int width = LAppMinimumDelegate::GetInstance()->GetWindowWidth();
    int height = LAppMinimumDelegate::GetInstance()->GetWindowHeight();

    CubismMatrix44 projection;

    LAppMinimumModel* model = GetModel();

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
    if (_viewMatrix)
    {
        projection.MultiplyByMatrix(_viewMatrix);
    }

    // モデル1体描画前コール
    LAppMinimumDelegate::GetInstance()->GetView()->PreModelDraw(*model);

    model->Update();
    model->Draw(projection);///< 参照渡しなのでprojectionは変質する

    // モデル1体描画前コール
    LAppMinimumDelegate::GetInstance()->GetView()->PostModelDraw(*model);
}

void LAppMinimumLive2DManager::SetAssetDirectory(const std::string &path)
{
    _currentModelDirectory = path;
}

void LAppMinimumLive2DManager::LoadModel(const std::string modelDirectoryName)
{
    // モデルのディレクトリを指定
    SetAssetDirectory(LAppDefine::ResourcesPath + modelDirectoryName + "/");

    // モデルデータの新規生成
    _model = new LAppMinimumModel(modelDirectoryName, _currentModelDirectory);

    // モデルデータの読み込み及び生成とセットアップを行う
    static_cast<LAppMinimumModel*>(_model)->SetupModel();
}


