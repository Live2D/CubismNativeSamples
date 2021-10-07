/**
* Copyright(c) Live2D Inc. All rights reserved.
*
* Use of this source code is governed by the Live2D Open Software license
* that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
*/

#include "MouseActionManager.hpp"

namespace {
    MouseActionManager* instance = NULL;
}

MouseActionManager* MouseActionManager::GetInstance()
{
    if (!instance)
    {
        instance = new MouseActionManager();
    }

    return instance;
}

void MouseActionManager::ReleaseInstance()
{
    if (instance)
    {
        delete instance;
    }

    instance = NULL;
}

MouseActionManager::MouseActionManager()
{
}

MouseActionManager::~MouseActionManager()
{
    // 行列データの解放
    delete _viewMatrix;

    delete _touchManager;
}

void MouseActionManager::Initialize(int windowWidth, int windowHeight)
{
    // 行列の初期化
    ViewInitialize(windowWidth, windowHeight);

    // タッチ関係のイベント管理
    _touchManager = new TouchManager();

    _captured = false;
    _mouseX = 0.0f;
    _mouseY = 0.0f;
}

void MouseActionManager::ViewInitialize(int windowWidth, int windowHeight)
{
    _deviceToScreen = new Csm::CubismMatrix44();
    _viewMatrix = new CubismSampleViewMatrix(_deviceToScreen, windowWidth, windowHeight);
}

void MouseActionManager::OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y)
{
    _userModel->SetDragging(x, y);
}

void MouseActionManager::OnTouchesBegan(float px, float py)
{
    _touchManager->TouchesBegan(px, py);
}

void MouseActionManager::OnTouchesMoved(float px, float py)
{
    float screenX = _deviceToScreen->TransformX(_touchManager->GetX()); // 論理座標変換した座標を取得。
    float viewX = _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。

    float screenY = _deviceToScreen->TransformY(_touchManager->GetY()); // 論理座標変換した座標を取得。
    float viewY = _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。

    _touchManager->TouchesMoved(px, py);

    // ドラッグ情報を設定
    _userModel->SetDragging(viewX, viewY);
}

void MouseActionManager::OnTouchesEnded(float px, float py)
{
    // タッチ終了
    OnDrag(0.0f, 0.0f);
}

void MouseActionManager::OnMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{
    if (GLFW_MOUSE_BUTTON_LEFT != button)
    {
        return;
    }

    switch (action)
    {
    case GLFW_PRESS:
        _captured = true;
        OnTouchesBegan(_mouseX, _mouseY);
        break;
    case GLFW_RELEASE:
        if (_captured)
        {
            _captured = false;
            OnTouchesEnded(_mouseX, _mouseY);
        }
        break;
    default:
        break;
    }
}

void MouseActionManager::OnMouseCallBack(GLFWwindow* window, double x, double y)
{
    _mouseX = static_cast<float>(x);
    _mouseY = static_cast<float>(y);

    if (!_captured)
    {
        return;
    }

    OnTouchesMoved(_mouseX, _mouseY);
}

CubismSampleViewMatrix * MouseActionManager::GetViewMatrix()
{
    return _viewMatrix;
}

void MouseActionManager::SetUserModel(Csm::CubismUserModel * userModel)
{
    _userModel = userModel;
}
