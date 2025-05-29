/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "NapiBridgeC.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
    LAppDelegate* s_instance = NULL;
}

LAppDelegate* LAppDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppDelegate();
    }

    return s_instance;
}

void LAppDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}


void LAppDelegate::Run()
{
    // 時間更新
    LAppPal::UpdateTime();

    // 画面の初期化
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepthf(1.0f);

    //描画更新
    if (_view != NULL)
    {
        _view->Render();
    }
    eglSwapBuffers(_eglDisplay, _eglSurface);

    if(!_isActive)
    {
        NapiBridgeC::MoveTaskToBack();
        _isActive = true;
    }
}

void LAppDelegate::InitializeEgl()
{
    csmInt32 configAttribs[] =
    {
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLint winAttribs[] =
    {
        EGL_GL_COLORSPACE_KHR,
        EGL_GL_COLORSPACE_SRGB_KHR,
        EGL_NONE
    };
    EGLint contextAttribs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    csmInt32 configsNum;

    if (!_eglWindow)
    {
        LAppPal::PrintLogLn("Failed because `window` is null.");
        return;
    }
    _eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!eglInitialize(_eglDisplay, nullptr, nullptr))
    {
        LAppPal::PrintLogLn("Failed to initialize `_eglDisplay`.");
        _eglDisplay = EGL_NO_DISPLAY;
        return;
    }

    if (!eglChooseConfig(_eglDisplay, configAttribs, &_eglConfig, 1, &configsNum))
    {
        LAppPal::PrintLogLn("`eglChooseConfig` error.");
        return;
    }

    _eglSurface = eglCreateWindowSurface(_eglDisplay, _eglConfig, _eglWindow, winAttribs);
    if (!_eglSurface)
    {
        LAppPal::PrintLogLn("`_eglSurface` is null.");
        return;
    }

    _eglContext = eglCreateContext(_eglDisplay, _eglConfig, _eglSharedContext, contextAttribs);
    if (!eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext))
    {
        LAppPal::PrintLogLn("`eglMakeCurrent` error = %d", eglGetError());
        return;
    }

    LAppPal::PrintLogLn("Successful initialization of EGL.");
}

void LAppDelegate::ReleaseEgl()
{
    if (!eglDestroySurface(_eglDisplay, _eglSurface))
    {
        LAppPal::PrintLogLn("`eglDestroySurface` failed.");
    }

    if (!eglDestroyContext(_eglDisplay, _eglContext))
    {
        LAppPal::PrintLogLn("`eglDestroyContext` failed.");
    }

    if (!eglTerminate(_eglDisplay))
    {
        LAppPal::PrintLogLn("`eglTerminate` failed.");
    }

    _eglSurface = nullptr;
    _eglContext = EGL_NO_CONTEXT;
    _eglDisplay = EGL_NO_DISPLAY;

    LAppPal::PrintLogLn("Successful delete of EGL.");
}

void LAppDelegate::InitializeDisplay()
{
    _textureManager = new LAppTextureManager();
    _view = new LAppView();
    LAppPal::UpdateTime();

    //テクスチャサンプリング設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //透過設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //Initialize cubism
    CubismFramework::Initialize();
}

void LAppDelegate::ChangeDisplay(float width, float height)
{
    glViewport(0, 0, width, height);
    _width = width;
    _height = height;

    //AppViewの初期化
    _view->Initialize(width, height);
    _view->InitializeSprite();
}

LAppDelegate::LAppDelegate():
    _cubismOption(),
    _captured(false),
    _mouseX(0.0f),
    _mouseY(0.0f),
    _isActive(true),
    _textureManager(NULL),
    _view(NULL),
    _eglDisplay(EGL_NO_DISPLAY),
    _eglConfig(nullptr),
    _eglSurface(nullptr),
    _eglContext(EGL_NO_CONTEXT),
    _eglSharedContext(EGL_NO_CONTEXT)
{
    // Setup Cubism
    _cubismOption.LogFunction = LAppPal::PrintMessageLn;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    CubismFramework::CleanUp();
    CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);
}

LAppDelegate::~LAppDelegate()
{
}

void LAppDelegate::OnTouchBegan(double x, double y)
{
    _mouseX = static_cast<float>(x);
    _mouseY = static_cast<float>(y);

    if (_view != NULL)
    {
        _captured = true;
        _view->OnTouchesBegan(_mouseX, _mouseY);
    }
}

void LAppDelegate::OnTouchEnded(double x, double y)
{
    _mouseX = static_cast<float>(x);
    _mouseY = static_cast<float>(y);

    if (_view != NULL)
    {
        _captured = false;
        _view->OnTouchesEnded(_mouseX, _mouseY);
    }
}

void LAppDelegate::OnTouchMoved(double x, double y)
{
    _mouseX = static_cast<float>(x);
    _mouseY = static_cast<float>(y);

    if (_captured && _view != NULL)
    {
        _view->OnTouchesMoved(_mouseX, _mouseY);
    }
}
