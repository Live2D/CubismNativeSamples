/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"
#include <math.h>
#include <string>
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "TouchManager_Common.hpp"
#include "LAppSprite.hpp"
#include "LAppSpritePipeline.hpp"
#include "LAppModelSpritePipeline.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace LAppDefine;

LAppView::LAppView():
                    LAppView_Common()
                    , _back(NULL)
                    , _gear(NULL)
                    , _power(NULL)
                    , _renderSprite(NULL)
                    , _renderTarget(SelectTarget_None)
                    , _modelSpritePipeline(NULL)
                    , _spritePipeline(NULL)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager_Common();
}

LAppView::~LAppView()
{
    VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();
    _renderBuffer.DestroyOffscreenSurface(device);

    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_back->GetTextureId());
    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_gear->GetTextureId());
    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_power->GetTextureId());
    _back->Release(device);
    _gear->Release(device);
    _power->Release(device);
    _renderSprite->Release(device);

    if (_renderSprite)
    {
        delete _renderSprite;
    }
    if (_spritePipeline)
    {
        delete _spritePipeline;
    }
    if (_modelSpritePipeline)
    {
        delete _modelSpritePipeline;
    }
    if (_touchManager)
    {
        delete _touchManager;
    }
    if (_back)
    {
        delete _back;
    }
    if (_gear)
    {
        delete _gear;
    }
    if (_power)
    {
        delete _power;
    }
}

void LAppView::Initialize(int width, int height)
{
    LAppView_Common::Initialize(width, height);
}

void LAppView::BeginRendering(VkCommandBuffer commandBuffer, float r, float g, float b, float a, bool isClear)
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkRenderingAttachmentInfoKHR colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachment.imageView = vkManager->GetSwapchainImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    if (isClear)
    {
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    else
    {
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    }
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = {r, g, b, a};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, {vkManager->GetSwapchainManager()->GetExtent()}};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void LAppView::ChangeEndLayout(VkCommandBuffer commandBuffer)
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
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
}

void LAppView::EndRendering(VkCommandBuffer commandBuffer)
{
    vkCmdEndRendering(commandBuffer);
}

void LAppView::Render()
{
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    if (!live2DManager)
    {
        return;
    }

    //スプライト描画
    int width, height;
    glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();
    BeginRendering(commandBuffer, 0.0, 0.0, 0.0, 1.0, true);
    if(_spritePipeline->GetPipeline() != NULL && _spritePipeline->GetPipelineLayout() != NULL)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _spritePipeline->GetPipeline());
        _back->Render(commandBuffer, vkManager, width, height);
        _gear->Render(commandBuffer, vkManager, width, height);
        _power->Render(commandBuffer, vkManager, width, height);
    }
    EndRendering(commandBuffer);
    vkManager->SubmitCommand(commandBuffer, true);

    live2DManager->SetViewMatrix(_viewMatrix);

    // Cubism更新・描画
    live2DManager->OnUpdate();

    // 各モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    {
        if(_modelSpritePipeline->GetPipeline() != NULL && _modelSpritePipeline->GetPipelineLayout() != NULL)
        {
            for (csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
            {
                LAppModel* model = live2DManager->GetModel(i);
                commandBuffer = vkManager->BeginSingleTimeCommands();
                _renderSprite->SetDescriptorUpdated(false);
                _renderSprite->UpdateDescriptorSet(vkManager->GetDevice(),
                                                            model->GetRenderBuffer().GetTextureView(),
                                                            model->GetRenderBuffer().GetTextureSampler());
                BeginRendering(commandBuffer, 0.f, 0.f, 0.3, 1.f, false);
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _modelSpritePipeline->GetPipeline());

                float alpha = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
                _renderSprite->SetColor(alpha, alpha, alpha, alpha);
                if (model)
                {
                    _renderSprite->Render(commandBuffer, vkManager, width, height);
                }
                EndRendering(commandBuffer);
                vkManager->SubmitCommand(commandBuffer);
            }
        }
    }

    commandBuffer = vkManager->BeginSingleTimeCommands();
    ChangeEndLayout(commandBuffer);
    vkManager->SubmitCommand(commandBuffer);
}

void LAppView::InitializeSprite()
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkDevice device = vkManager->GetDevice();
    VkPhysicalDevice physicalDevice = vkManager->GetPhysicalDevice();
    SwapchainManager* swapchainManager = vkManager->GetSwapchainManager();
    _spritePipeline = new LAppSpritePipeline(device, swapchainManager->GetExtent(), vkManager->GetSwapchainManager()->GetSwapchainImageFormat());
    _modelSpritePipeline = new LAppModelSpritePipeline(device, swapchainManager->GetExtent(), vkManager->GetSwapchainManager()->GetSwapchainImageFormat());

    int width, height;
    glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    const string resourcesPath = ResourcesPath;

    string imageName;
    CubismImageVulkan textureImage;
    float x;
    float y;
    float fWidth;
    float fHeight;

    imageName = BackImageName;
    LAppTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(
        resourcesPath + imageName, vkManager->GetImageFormat(), VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0.0);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(backgroundTexture->id, textureImage);

    x = width * 0.5f;
    y = height * 0.5f;
    fWidth = static_cast<float>(backgroundTexture->width * 2.0f);
    fHeight = static_cast<float>(height * 0.95f);
    _back = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, backgroundTexture->id,
                           _spritePipeline, textureImage.GetView(), textureImage.GetSampler());

    imageName = GearImageName;
    LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(
        resourcesPath + imageName, vkManager->GetImageFormat(), VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0.0);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(gearTexture->id, textureImage);

    //X：右向き正、Y：下向き正
    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    _gear = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, gearTexture->id,
                           _spritePipeline, textureImage.GetView(), textureImage.GetSampler());

    imageName = PowerImageName;
    LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(
        resourcesPath + imageName, vkManager->GetImageFormat(), VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0.0);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(powerTexture->id, textureImage);

    //X：右向き正、Y：下向き正
    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(height - powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    _power = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, powerTexture->id,
                            _spritePipeline, textureImage.GetView(), textureImage.GetSampler());

    // 画面全体を覆うサイズ
    x = width * 0.5f;
    y = height * 0.5f;
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    _renderSprite = new LAppSprite(device, physicalDevice, vkManager, x, y, static_cast<float>(width), static_cast<float>(height),0, _modelSpritePipeline, NULL, NULL);
}

void LAppView::OnTouchesBegan(float px, float py) const
{
    _touchManager->TouchesBegan(px, py);
}

void LAppView::OnTouchesMoved(float px, float py) const
{
    float viewX = this->TransformViewX(_touchManager->GetX());
    float viewY = this->TransformViewY(_touchManager->GetY());

    _touchManager->TouchesMoved(px, py);

    LAppLive2DManager* Live2DManager = LAppLive2DManager::GetInstance();
    Live2DManager->OnDrag(viewX, viewY);
}

void LAppView::OnTouchesEnded(float px, float py) const
{
    // タッチ終了
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    live2DManager->OnDrag(0.0f, 0.0f);
    {
        // シングルタップ
        float x = _deviceToScreen->TransformX(_touchManager->GetX()); // 論理座標変換した座標を取得。
        float y = _deviceToScreen->TransformY(_touchManager->GetY()); // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable)
        {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
        live2DManager->OnTap(x, y);

        // 画面サイズの取得
        int windowWidth, windowHeight;
        glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &windowWidth, &windowHeight);

        // 歯車にタップしたか
        if (_gear->IsHit(windowWidth, windowHeight, px, py))
        {
            live2DManager->NextScene();
        }

        // 電源ボタンにタップしたか
        if (_power->IsHit(windowWidth, windowHeight, px, py))
        {
            LAppDelegate::GetInstance()->AppEnd();
        }
    }
}

void LAppView::PreModelDraw(LAppModel& refModel)
{
    if (_renderTarget == SelectTarget_None)
    {
        // 通常のスワップチェーンイメージへと描画
        VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
        Csm::Rendering::CubismRenderer_Vulkan::SetRenderTarget(vkManager->GetSwapchainImage(),
                                                                vkManager->GetSwapchainImageView(),
                                                                vkManager->GetSwapchainManager()->GetSwapchainImageFormat(),
                                                                vkManager->GetSwapchainManager()->GetExtent());
    }
    else
    {
        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenSurface_Vulkan* useTarget = NULL;
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer                // LAppView が持つバッファ
                                                                    : &refModel.GetRenderBuffer();  // Model が持つバッファ

        // 別のレンダリングターゲットへ向けて描画する場合
        if (!useTarget->IsValid())
        {
            // 描画ターゲット内部未作成の場合はここで作成
            int width, height;
            glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);
            VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
            if (width != 0 && height != 0)
            {
                useTarget->CreateOffscreenSurface(vkManager->GetDevice(), vkManager->GetPhysicalDevice(),
                    static_cast<csmUint32>(width), static_cast<csmUint32>(height),
                    vkManager->GetImageFormat(),
                    vkManager->GetDepthFormat()
                );
                _renderSprite->SetDescriptorUpdated(false);
            }
        }
        // 描画先を別のレンダリングターゲットへ指定
        LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
        Csm::Rendering::CubismRenderer_Vulkan::SetRenderTarget(useTarget->GetTextureImage(), useTarget->GetTextureView(),
                                                               LAppDelegate::GetInstance()->GetVulkanManager()->GetImageFormat(),
                                                               VkExtent2D{useTarget->GetBufferWidth(), useTarget->GetBufferHeight()});
    }
}

void LAppView::PostModelDraw(LAppModel& refModel, csmInt32 modelIndex)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_Vulkan* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {
        // 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            int width, height;
            glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);
            VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
            VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();
            _renderSprite->UpdateDescriptorSet(vkManager->GetDevice(), useTarget->GetTextureView(),
                                                        useTarget->GetTextureSampler());
            BeginRendering(commandBuffer, 0.f, 0.f, 0.3, 1.f, false);
            if(_modelSpritePipeline->GetPipeline() != NULL && _modelSpritePipeline->GetPipelineLayout() != NULL)
            {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _modelSpritePipeline->GetPipeline());
                float alpha = GetSpriteAlpha(0);
                _renderSprite->SetColor(alpha, alpha, alpha, alpha);
                _renderSprite->Render(commandBuffer, vkManager, width, height);
            }
            EndRendering(commandBuffer);
            vkManager->SubmitCommand(commandBuffer);
        }
    }

    // レンダーターゲットをスワップチェーンのものへと戻す
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    Csm::Rendering::CubismRenderer_Vulkan::SetRenderTarget(vkManager->GetSwapchainImage(),
                                                            vkManager->GetSwapchainImageView(),
                                                            vkManager->GetSwapchainManager()->GetSwapchainImageFormat(),
                                                            vkManager->GetSwapchainManager()->GetExtent());
}

void LAppView::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void LAppView::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

float LAppView::GetSpriteAlpha(int assign) const
{
    // assignの数値に応じて適当に決定
    float alpha = 0.4f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    if (alpha < 0.1f)
    {
        alpha = 0.1f;
    }

    return alpha;
}

void LAppView::ResizeSprite(int width, int height)
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkDevice device = vkManager->GetDevice();
    SwapchainManager* swapchainManager = vkManager->GetSwapchainManager();
    if (_spritePipeline)
    {
        delete _spritePipeline;
    }
    if (_modelSpritePipeline)
    {
        delete _modelSpritePipeline;
    }
    _spritePipeline = new LAppSpritePipeline(device, swapchainManager->GetExtent(), vkManager->GetSwapchainManager()->GetSwapchainImageFormat());
    _modelSpritePipeline = new LAppModelSpritePipeline(device, swapchainManager->GetExtent(), vkManager->GetSwapchainManager()->GetSwapchainImageFormat());

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    if (!textureManager)
    {
        return;
    }

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    if (_back)
    {
        _back->SetPipeline(_spritePipeline);
        uint32_t id = _back->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = width * 0.5f;
            y = height * 0.5f;
            fWidth = static_cast<float>(texInfo->width * 2);
            fHeight = static_cast<float>(height) * 0.95f;
            _back->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_power)
    {
        _power->SetPipeline(_spritePipeline);
        uint32_t id = _power->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(height - texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _power->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_gear)
    {
        _gear->SetPipeline(_spritePipeline);
        uint32_t id = _gear->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _gear->ResetRect(x, y, fWidth, fHeight);
        }
    }
    if (_renderSprite)
    {
        _renderSprite->SetPipeline(_modelSpritePipeline);
        x = width * 0.5f;
        y = height * 0.5f;
        _renderSprite->ResetRect(x, y, static_cast<float>(width), static_cast<float>(height));
    }
}

void LAppView::DestroyOffscreenSurface()
{
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    if (_renderTarget == SelectTarget_ViewFrameBuffer)
    {
        _renderBuffer.DestroyOffscreenSurface(LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice());
    }
    else if (_renderTarget == SelectTarget_ModelFrameBuffer)
    {
        for (csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
        {
            LAppModel* model = live2DManager->GetModel(i);
            model->GetRenderBuffer().
                   DestroyOffscreenSurface(LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice());
        }
    }
}
