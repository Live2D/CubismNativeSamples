/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#include <GLES2/gl2.h>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppModel.hpp"
#include "JniBridgeC.hpp"

#include <Rendering/OpenGL/CubismShader_OpenGLES2.hpp>

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


void LAppDelegate::OnStart()
{
    _isActive = true;
}

void LAppDelegate::OnPause()
{
}

void LAppDelegate::OnStop()
{
    if (_view)
    {
        delete _view;
        _view = NULL;
    }
    if (_textureManager)
    {
        delete _textureManager;
        _textureManager = NULL;
    }

    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    CubismFramework::Dispose();
}

void LAppDelegate::OnDestroy()
{
    ReleaseInstance();
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

    if(_isActive == false)
    {
        JniBridgeC::MoveTaskToBack();
    }
}

void LAppDelegate::OnSurfaceCreate()
{
    //テクスチャサンプリング設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //透過設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (_textureManager == nullptr)
    {
        _textureManager = new LAppTextureManager();
    }
    else
    {
        // 無効になっているOpenGLリソースを破棄
        _textureManager->ReleaseInvalidTextures();
    }
    if (_view != NULL)
    {
        delete _view;
    }
    _view = new LAppView();
    LAppPal::UpdateTime();

    //Initialize cubism
    if (!CubismFramework::IsInitialized())
    {
        CubismFramework::Initialize();
    }

    // 無効になっているOpenGLリソースを破棄
    Live2D::Cubism::Framework::Rendering::CubismShader_OpenGLES2::GetInstance()->ReleaseInvalidShaderProgram();

    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    for (Csm::csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
    {
        live2DManager->GetModel(i)->ReloadRenderer();
    }
}

void LAppDelegate::OnSurfaceChanged(float width, float height)
{
    glViewport(0, 0, width, height);
    _width = width;
    _height = height;

    //AppViewの初期化
    _view->Initialize(width, height);
    _view->InitializeSprite();
}

void LAppDelegate::SetSceneIndex(int index)
{
    _SceneIndex = index;
}

LAppDelegate::LAppDelegate():
    _cubismOption(),
    _captured(false),
    _SceneIndex(0),
    _mouseX(0.0f),
    _mouseY(0.0f),
    _isActive(true),
    _textureManager(NULL),
    _view(NULL)
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
