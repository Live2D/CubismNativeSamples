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

MouseActionManager::MouseActionManager() : MouseActionManager_Common()
{
}

MouseActionManager::~MouseActionManager()
{
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
