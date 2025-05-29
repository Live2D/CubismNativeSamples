/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <libgen.h>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>

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

bool LAppDelegate::Initialize()
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("START");
    }

    // GLFWの初期化
    if (glfwInit() == GL_FALSE)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("Can't initilize GLFW");
        }
        return GL_FALSE;
    }
    // Windowの生成_
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "SAMPLE", NULL, NULL);

    if (_window == NULL)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("Can't create GLFW window.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    glfwSetWindowUserPointer(_window, VulkanManager::GetInstance());

     // Windowのコンテキストをカレントに設定
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    //コールバック関数の登録
    glfwSetMouseButtonCallback(_window, EventHandler::OnMouseCallBack);
    glfwSetCursorPosCallback(_window, EventHandler::OnMouseCallBack);
    glfwSetFramebufferSizeCallback(_window, EventHandler::FrameBufferResizeCallback);

    // ウィンドウサイズ記憶
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    _windowWidth = width;
    _windowHeight = height;

    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    // CubismSDKの初期化
    InitializeCubism();

    // vulkanデバイスの作成
    VulkanManager::GetInstance()->Initialize(_window);
    SwapchainManager* swapchainManager = VulkanManager::GetInstance()->GetSwapchainManager();
    // レンダラにvulkanManagerの変数を渡す
    Live2D::Cubism::Framework::Rendering::CubismRenderer_Vulkan::InitializeConstantSettings(
        VulkanManager::GetInstance()->GetDevice(), VulkanManager::GetInstance()->GetPhysicalDevice(),
        VulkanManager::GetInstance()->GetCommandPool(), VulkanManager::GetInstance()->GetGraphicQueue(),
        swapchainManager->GetImageCount(), swapchainManager->GetExtent(),
        VulkanManager::GetInstance()->GetSwapchainImageView(), swapchainManager->GetSwapchainImageFormat(),
        VulkanManager::GetInstance()->GetDepthFormat()
    );

    //AppViewの初期化
    _view->Initialize(width, height);

    SetExecuteAbsolutePath();

    //load model
    LAppLive2DManager::GetInstance();

    //load sprite
    _view->InitializeSprite();

    return GL_TRUE;
}

void LAppDelegate::Release()
{
    delete _textureManager;
    delete _view;

    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    //Cubism3の解放
    CubismFramework::Dispose();

    VulkanManager::GetInstance()->Destroy();

    // Windowの削除
    glfwDestroyWindow(_window);
    glfwTerminate();
}

bool LAppDelegate::RecreateSwapchain()
{
    int width = 0, height = 0;
    if (VulkanManager::GetInstance()->GetIsWindowSizeChanged())
    {
        glfwGetFramebufferSize(_window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(_window, &width, &height);
            glfwWaitEvents();
        }

        VulkanManager::GetInstance()->RecreateSwapchain();
        Live2D::Cubism::Framework::Rendering::CubismRenderer_Vulkan::SetRenderTarget(
            VulkanManager::GetInstance()->GetSwapchainImage(),
            VulkanManager::GetInstance()->GetSwapchainImageView(),
            VulkanManager::GetInstance()->GetSwapchainManager()->GetSwapchainImageFormat(),
            VulkanManager::GetInstance()->GetSwapchainManager()->GetExtent()
        );

        // AppViewの初期化
        _view->Initialize(width, height);
        // スプライトサイズを再設定
        _view->ResizeSprite(width, height);
        // オフスクリーンを再作成する
        _view->DestroyOffscreenSurface();
        // サイズを保存しておく
        _windowWidth = width;
        _windowHeight = height;
        VulkanManager::GetInstance()->SetIsWindowSizeChanged(false);
        return true;
    }
    return false;
}

void LAppDelegate::Run()
{
    //メインループ
    while (glfwWindowShouldClose(_window) == GL_FALSE && !_isEnd)
    {
        // Poll for and process events
        glfwPollEvents();

        // 時間更新
        LAppPal::UpdateTime();
        VulkanManager::GetInstance()->UpdateDrawFrame();

        if (RecreateSwapchain())
        {
            continue;
        }

        //描画更新
        _view->Render();

        VulkanManager::GetInstance()->PostDraw();
        RecreateSwapchain();
    }

    Release();

    LAppDelegate::ReleaseInstance();
}

LAppDelegate::LAppDelegate():
    _cubismOption(),
    _window(NULL),
    _captured(false),
    _mouseX(0.0f),
    _mouseY(0.0f),
    _isEnd(false),
    _windowWidth(0),
    _windowHeight(0)
{
    _executeAbsolutePath = "";
    _view = new LAppView();
    _textureManager = new LAppTextureManager();
}

LAppDelegate::~LAppDelegate()
{}

void LAppDelegate::InitializeCubism()
{
    //Initialize cubism
    CubismFramework::Initialize();

    //default proj
    CubismMatrix44 projection;

    LAppPal::UpdateTime();
}

void LAppDelegate::OnMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{
    if (_view == NULL)
    {
        return;
    }
    if (GLFW_MOUSE_BUTTON_LEFT != button)
    {
        return;
    }

    if (GLFW_PRESS == action)
    {
        _captured = true;
        _view->OnTouchesBegan(_mouseX, _mouseY);
    }
    else if (GLFW_RELEASE == action)
    {
        if (_captured)
        {
            _captured = false;
            _view->OnTouchesEnded(_mouseX, _mouseY);
        }
    }
}

void LAppDelegate::OnMouseCallBack(GLFWwindow* window, double x, double y)
{
    _mouseX = static_cast<float>(x);
    _mouseY = static_cast<float>(y);

    if (!_captured)
    {
        return;
    }
    if (_view == NULL)
    {
        return;
    }

    _view->OnTouchesMoved(_mouseX, _mouseY);
}

void LAppDelegate::SetExecuteAbsolutePath()
{
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, 1024 - 1);

    if (len != -1)
    {
        path[len] = '\0';
    }

    this->_executeAbsolutePath = dirname(path);
    this->_executeAbsolutePath += "/";
}
