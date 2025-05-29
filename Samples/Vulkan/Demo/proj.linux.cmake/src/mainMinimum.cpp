/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include <functional>

#include <sstream>
#include <unistd.h>
#include <libgen.h>
#include <GLFW/glfw3.h>

#include "LAppDefine.hpp"
#include "LAppAllocator_Common.hpp"
#include "LAppTextureManager.hpp"
#include "LAppPal.hpp"
#include "CubismUserModelExtend.hpp"
#include "MouseActionManager.hpp"
#include "VulkanManager.hpp"

#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>
#include <Model/CubismUserModel.hpp>
#include <Physics/CubismPhysics.hpp>
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>
#include <Utils/CubismString.hpp>

/**
*@brief モデルデータのディレクトリ名
* このディレクトリ名と同名の.model3.jsonを読み込む
*/
static const Csm::csmChar* _modelDirectoryName = "Hiyori";

static Csm::CubismUserModel* _userModel; ///< ユーザーが実際に使用するモデル
static Csm::CubismFramework::Option _cubismOption; ///< CubismFrameworkに関するオプション
static LAppAllocator_Common _cubismAllocator; ///< メモリのアロケーター

static std::string _executeAbsolutePath; ///< アプリケーションの実行パス
static std::string _currentModelDirectory; ///< 現在のモデルのディレクトリ名

static GLFWwindow* _window; ///< ウィンドウオブジェクト
int _windowWidth, _windowHeight; ///< ウィンドウサイズの保存

/**
* @brief Cubism SDKの初期化
*
* Cubism SDKの初期化処理を行う
*/
static void InitializeCubism()
{
    //Initialize cubism
    Csm::CubismFramework::Initialize();

    LAppPal::UpdateTime();
}

/**
* @brief アプリケーションの実行パスの設定
*
* Linuxのアプリケーションの実行パスを確認し、パスを取得する
*/
void SetExecuteAbsolutePath()
{
    const int maximumPathBufferSize = 1024;
    char path[maximumPathBufferSize];
    ssize_t len = readlink("/proc/self/exe", path, maximumPathBufferSize - 1);

    if (len != -1)
    {
        path[len] = '\0';
    }

    _executeAbsolutePath = dirname(path);
    _executeAbsolutePath += "/";
}

/**
* @brief システムの初期化
*
* 基盤システムの初期化処理を行う
*/
static bool InitializeSystem()
{
    LAppPal::PrintLogLn("START");

    // GLFWの初期化
    if (glfwInit() == GL_FALSE)
    {
        LAppPal::PrintLogLn("Can't initilize GLFW");

        return GL_FALSE;
    }

    // Windowの生成
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(LAppDefine::RenderTargetWidth, LAppDefine::RenderTargetHeight, "SIMPLE_SAMPLE", NULL, NULL);
    if (_window == NULL)
    {
        LAppPal::PrintLogLn("Can't create GLFW window.");

        glfwTerminate();
        return GL_FALSE;
    }

    glfwSetWindowUserPointer(_window, VulkanManager::GetInstance());

    // Windowのコンテキストをカレントに設定
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    // コールバック関数の登録
    glfwSetMouseButtonCallback(_window, EventHandler::OnMouseCallBack);
    glfwSetCursorPosCallback(_window, EventHandler::OnMouseCallBack);

    // ウィンドウサイズ記憶
    glfwGetWindowSize(_window, &_windowWidth, &_windowHeight);

    // Cubism SDK の初期化
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = Csm::CubismFramework::Option::LogLevel_Verbose;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    InitializeCubism();

    LAppPal::UpdateTime();

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

    // ドラッグ入力管理クラスの初期化
    MouseActionManager::GetInstance()->Initialize(_windowWidth, _windowHeight);

    SetExecuteAbsolutePath();

    return GL_TRUE;
}

/**
* @brief 解放
*
* 解放の処理を行う
*/
void Release()
{
    // レンダラの解放
    _userModel->DeleteRenderer();

    // モデルデータの解放
    delete _userModel;

    // Windowの削除
    glfwDestroyWindow(_window);

    // OpenGLの処理を終了
    glfwTerminate();

    // MouseActionManagerの解放
    MouseActionManager::ReleaseInstance();

    // Cubism SDK の解放
    Csm::CubismFramework::Dispose();
}

/**
* @brief モデルの読み込み
*
* モデルデータの読み込み処理を行う
*
* @param[in] modelDirectory モデルのディレクトリ名
*/
void LoadModel(const std::string modelDirectoryName)
{
    // モデルのディレクトリを指定
    _currentModelDirectory = _executeAbsolutePath + LAppDefine::ResourcesPath + modelDirectoryName + "/";

    // モデルデータの新規生成
    _userModel = new CubismUserModelExtend(modelDirectoryName, _currentModelDirectory);

    // モデルデータの読み込み及び生成とセットアップを行う
    std::string json = ".model3.json";
    std::string fileName = _modelDirectoryName + json;
    static_cast<CubismUserModelExtend*>(_userModel)->LoadAssets(fileName.c_str());

    // ユーザーモデルをMouseActionManagerへ渡す
    MouseActionManager::GetInstance()->SetUserModel(_userModel);
}

bool RecreateSwapchain()
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

        // サイズを保存しておく
        _windowWidth = width;
        _windowHeight = height;
        VulkanManager::GetInstance()->SetIsWindowSizeChanged(false);

        return true;
    }
    return false;
}

/**
* @brief モデル描画前処理
*
* スワップチェーンのクリアとメモリーレイアウトの変更を行う
*/
void PreModelDraw()
{
    VulkanManager* vkManager = VulkanManager::GetInstance();
    VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();

    // レイアウト変更
    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = 0;
    memoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    memoryBarrier.image = vkManager->GetSwapchainImage();
    memoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

    VkRenderingAttachmentInfoKHR colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachment.imageView = vkManager->GetSwapchainImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = {0.0, 0.0, 0.0, 1.0};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, {vkManager->GetSwapchainManager()->GetExtent()}};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    // 画面クリア
    vkCmdBeginRendering(commandBuffer, &renderingInfo);
    vkCmdEndRendering(commandBuffer);
    vkManager->SubmitCommand(commandBuffer, true);

    Live2D::Cubism::Framework::Rendering::CubismRenderer_Vulkan::SetRenderTarget(
        VulkanManager::GetInstance()->GetSwapchainImage(),
        VulkanManager::GetInstance()->GetSwapchainImageView(),
        VulkanManager::GetInstance()->GetSwapchainManager()->GetSwapchainImageFormat(),
        VulkanManager::GetInstance()->GetSwapchainManager()->GetExtent()
    );
}

/**
* @brief モデル描画後処理
*
* スワップチェーンのレイアウトを描画用へ変更
*/
void PostModelDraw()
{
    VulkanManager* vkManager = VulkanManager::GetInstance();

    VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();

    // スワップチェーン描画終了バリア
    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    memoryBarrier.dstAccessMask = 0;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    memoryBarrier.image = vkManager->GetSwapchainImage();
    memoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

    vkManager->SubmitCommand(commandBuffer);
}

void Run()
{
    //メインループ
    while (glfwWindowShouldClose(_window) == GL_FALSE)
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

        // View の PreModelDraw
        PreModelDraw();

        // モデルの更新及び描画
        static_cast<CubismUserModelExtend*>(_userModel)->ModelOnUpdate(_window);

        // View の PostModelDraw
        PostModelDraw();

        // バッファの入れ替え
        VulkanManager::GetInstance()->PostDraw();
        RecreateSwapchain();
    }
}

int main()
{
    // 初期化
    InitializeSystem();

    // モデルの読み込み
    LoadModel(_modelDirectoryName);

    // メインループ
    Run();

    // 解放処理
    Release();

    return 0;
}
