/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>
#include "LAppLive2DManager.hpp"
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppTextureManager.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
LAppDelegate* s_instance = NULL;
VulkanManager* s_vulkanManager = NULL;
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
    window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "SAMPLE", NULL, NULL);
    s_vulkanManager = new VulkanManager(window);

    if (window == NULL)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLogLn("Can't create GLFW window.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    glfwSetWindowUserPointer(window, this);

    // Windowのコンテキストをカレントに設定
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //コールバック関数の登録
    glfwSetMouseButtonCallback(window, EventHandler::OnMouseCallBack);
    glfwSetCursorPosCallback(window, EventHandler::OnMouseCallBack);
    glfwSetFramebufferSizeCallback(window, s_vulkanManager->framebufferResizeCallback);

    // ウィンドウサイズ記憶
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    _windowWidth = width;
    _windowHeight = height;

    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    // vulkanデバイスの作成
    s_vulkanManager->Initialize();
    SwapchainManager* swapchainManager = s_vulkanManager->GetSwapchainManager();
    // レンダラにvulkanManagerの変数を渡す
    Live2D::Cubism::Framework::Rendering::CubismRenderer_Vulkan::InitializeConstantSettings(
        s_vulkanManager->GetDevice(), s_vulkanManager->GetPhysicalDevice(),
        s_vulkanManager->GetCommandPool(), s_vulkanManager->GetGraphicQueue(),
        swapchainManager->GetExtent(), swapchainManager->GetSwapchainImageFormat(),
        s_vulkanManager->GetImageFormat(),
        s_vulkanManager->GetSwapchainImage(), s_vulkanManager->GetSwapchainImageView(),
        s_vulkanManager->GetDepthFormat()
    );

    _textureManager = new LAppTextureManager();

    //AppViewの初期化
    _view->Initialize();

    // Cubism SDK の初期化
    InitializeCubism();

    return GL_TRUE;
}

void LAppDelegate::Release()
{
    // Windowの削除
    glfwDestroyWindow(window);
    glfwTerminate();

    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    delete _view;
    delete _textureManager;
    _view = NULL;
    _textureManager = NULL;

    //Cubism SDK の解放
    CubismFramework::Dispose();

    s_vulkanManager->Destroy();
}

bool LAppDelegate::RecreateSwapchain()
{
    int width = 0, height = 0;
    if (s_vulkanManager->GetIsWindowSizeChanged())
    {
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        s_vulkanManager->RecreateSwapchain();
        Live2D::Cubism::Framework::Rendering::CubismRenderer_Vulkan::UpdateSwapchainVariable(
            s_vulkanManager->GetSwapchainManager()->GetExtent(),
            s_vulkanManager->GetSwapchainImage(),
            s_vulkanManager->GetSwapchainImageView()
        );

        // AppViewの初期化
        _view->Initialize();
        // スプライトサイズを再設定
        _view->ResizeSprite(width, height);
        // オフスクリーンを再作成する
        _view->DestroyOffscreenSurface();
        // サイズを保存しておく
        _windowWidth = width;
        _windowHeight = height;
        s_vulkanManager->SetIsWindowSizeChanged(false);
        return true;
    }
    return false;
}

void LAppDelegate::Run()
{
    //メインループ
    while (glfwWindowShouldClose(window) == GL_FALSE && !_isEnd)
    {
        //Poll for and process events
        glfwPollEvents();

        // 時間更新
        LAppPal::UpdateTime();
        s_vulkanManager->UpdateDrawFrame();

        if (RecreateSwapchain())
        {
            continue;
        }

        _view->Render();
        s_vulkanManager->PostDraw();
        RecreateSwapchain();
    }

    Release();

    LAppDelegate::ReleaseInstance();
}

LAppDelegate::LAppDelegate():
                            _cubismOption()
                            , _captured(false)
                            , _mouseX(0.0f)
                            , _mouseY(0.0f)
                            , _isEnd(false)
                            , _windowWidth(0)
                            , _windowHeight(0)
                            , _textureManager()
                            , window()
{
    _view = new LAppView();
}

LAppDelegate::~LAppDelegate()
{}

void LAppDelegate::InitializeCubism()
{
    //Initialize cubism
    CubismFramework::Initialize();

    //load model
    LAppLive2DManager::GetInstance();

    //アイコン
    _view->InitializeSprite();

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

GLFWwindow* LAppDelegate::GetWindow()
{
    return window;
}

VulkanManager* LAppDelegate::GetVulkanManager()
{
    return s_vulkanManager;
}
