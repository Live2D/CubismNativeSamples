/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "MouseActionManager_Common.hpp"

#include "LAppDefine.hpp"

namespace {
    MouseActionManager_Common* instance = NULL;
}

MouseActionManager_Common* MouseActionManager_Common::GetInstance()
{
    if (instance == NULL)
    {
        instance = new MouseActionManager_Common();
    }

    return instance;
}

void MouseActionManager_Common::ReleaseInstance()
{
    if (instance != NULL)
    {
        delete instance;
    }

    instance = NULL;
}

MouseActionManager_Common::MouseActionManager_Common()
{
}

MouseActionManager_Common::~MouseActionManager_Common()
{
    // 行列データの解放
    delete _viewMatrix;

    delete _TouchManager;
}

void MouseActionManager_Common::Initialize(int windowWidth, int windowHeight)
{
    // 行列の初期化
    ViewInitialize(windowWidth, windowHeight);

    // タッチ関係のイベント管理
    _TouchManager = new TouchManager_Common();

    _captured = false;
    _mouseX = 0.0f;
    _mouseY = 0.0f;
}

void MouseActionManager_Common::ViewInitialize(int windowWidth, int windowHeight)
{
    _deviceToScreen = new Csm::CubismMatrix44();
    _viewMatrix = new CubismSampleViewMatrix_Common(_deviceToScreen, windowWidth, windowHeight);
}

void MouseActionManager_Common::OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y)
{
    _userModel->SetDragging(x, y);
}

void MouseActionManager_Common::OnTouchesBegan(float px, float py)
{
    _TouchManager->TouchesBegan(px, py);
}

void MouseActionManager_Common::OnTouchesMoved(float px, float py)
{
    float screenX = _deviceToScreen->TransformX(_TouchManager->GetX()); // 論理座標変換した座標を取得。
    float viewX = _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。

    float screenY = _deviceToScreen->TransformY(_TouchManager->GetY()); // 論理座標変換した座標を取得。
    float viewY = _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。

    _TouchManager->TouchesMoved(px, py);

    // ドラッグ情報を設定
    _userModel->SetDragging(viewX, viewY);
}

void MouseActionManager_Common::OnTouchesEnded(float px, float py)
{
    // タッチ終了
    OnDrag(0.0f, 0.0f);
}

CubismSampleViewMatrix_Common * MouseActionManager_Common::GetViewMatrix()
{
    return _viewMatrix;
}

void MouseActionManager_Common::SetUserModel(Csm::CubismUserModel * userModel)
{
    _userModel = userModel;
}
