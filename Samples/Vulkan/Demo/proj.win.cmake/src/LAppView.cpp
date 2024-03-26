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
#include "TouchManager.hpp"
#include "LAppSprite.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace LAppDefine;

LAppView::LAppView():
                    _back(NULL)
                    , _gear(NULL)
                    , _power(NULL)
                    , _renderSprite(NULL)
                    , _renderTarget(SelectTarget_None)
                    , _fragShaderModule(VK_NULL_HANDLE)
                    , _vertShaderModule(VK_NULL_HANDLE)
                    , _descriptorSetLayout(VK_NULL_HANDLE)
                    , _pipeline(VK_NULL_HANDLE)
                    , _pipelineLayout(VK_NULL_HANDLE)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager();

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();
}

LAppView::~LAppView()
{
    VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();
    Cleanup(device);
    vkDestroyDescriptorSetLayout(device, _descriptorSetLayout, nullptr);
    vkDestroyShaderModule(device, _vertShaderModule, nullptr);
    vkDestroyShaderModule(device, _fragShaderModule, nullptr);
    _renderBuffer.DestroyOffscreenSurface(device);
    delete _renderSprite;
    delete _viewMatrix;
    delete _deviceToScreen;
    delete _touchManager;

    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_back->GetTextureId());
    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_gear->GetTextureId());
    LAppDelegate::GetInstance()->GetTextureManager()->ReleaseTexture(_power->GetTextureId());
    _back->Release(device);
    _gear->Release(device);
    _power->Release(device);
    delete _back;
    delete _gear;
    delete _power;
}

void LAppView::Initialize()
{
    int width, height;
    glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);

    if (width == 0 || height == 0)
    {
        return;
    }

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    _viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->Scale(ViewScale, ViewScale);

    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
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
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
    _back->Render(commandBuffer, _pipelineLayout, vkManager, width, height);
    _gear->Render(commandBuffer, _pipelineLayout, vkManager, width, height);
    _power->Render(commandBuffer, _pipelineLayout, vkManager, width, height);
    EndRendering(commandBuffer);
    vkManager->SubmitCommand(commandBuffer, true);

    live2DManager->SetViewMatrix(_viewMatrix);

    // Cubism更新・描画
    live2DManager->OnUpdate();

    // 各モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
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
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

            float alpha = i < 1 ? 1.0f : model->GetOpacity(); // サンプルとしてαに適当な差をつける
            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);
            if (model)
            {
                _renderSprite->Render(commandBuffer, _pipelineLayout, vkManager, width, height);
            }
            EndRendering(commandBuffer);
            vkManager->SubmitCommand(commandBuffer);
        }
    }
}

VkShaderModule LAppView::CreateShaderModule(VkDevice device, std::string filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        CubismLogError("failed to open file!");
    }

    csmInt32 fileSize = (csmInt32)file.tellg();
    csmVector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.GetPtr(), fileSize);
    file.close();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<const csmUint32*>(buffer.GetPtr());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        CubismLogError("failed to create shader module!");
    }

    return shaderModule;
}

void LAppView::CreateDescriptorSetLayout(VkDevice device)
{
    VkDescriptorSetLayoutBinding bindings[2];

    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].pImmutableSamplers = nullptr;
    bindings[0].stageFlags = VK_SHADER_STAGE_ALL;

    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].pImmutableSamplers = nullptr;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = sizeof(bindings) / sizeof(bindings[0]);
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to create descriptor set layout!");
    }
}

void LAppView::CreateSpriteGraphicsPipeline(VkDevice device, VkExtent2D extent, VkShaderModule vertShaderModule,
                                            VkShaderModule fragShaderModule, VkFormat swapchainFormat)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription = LAppSprite::SpriteVertex::GetBindingDescription();
    VkVertexInputAttributeDescription attributeDescriptions[2]{};
    LAppSprite::SpriteVertex::GetAttributeDescriptions(attributeDescriptions);
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = sizeof(attributeDescriptions) / sizeof(attributeDescriptions[0]);
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)scissor.extent.width;
    viewport.height = (float)scissor.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to create pipeline layout!");
    }

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &swapchainFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pNext = &renderingInfo;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to create graphics pipeline!");
    }
}

void LAppView::InitializeSprite()
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkDevice device = vkManager->GetDevice();
    VkPhysicalDevice physicalDevice = vkManager->GetPhysicalDevice();
    SwapchainManager* swapchainManager = vkManager->GetSwapchainManager();
    _vertShaderModule = CreateShaderModule(device, "SampleShaders/VertSprite.spv");
    _fragShaderModule = CreateShaderModule(device, "SampleShaders/FragSprite.spv");
    CreateDescriptorSetLayout(device);
    CreateSpriteGraphicsPipeline(device, swapchainManager->GetExtent(), _vertShaderModule, _fragShaderModule,
                                 vkManager->GetSwapchainManager()->GetSwapchainImageFormat());

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
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(backgroundTexture->id, textureImage);

    x = width * 0.5f;
    y = height * 0.5f;
    fWidth = static_cast<float>(backgroundTexture->width * 2.0f);
    fHeight = static_cast<float>(height * 0.95f);
    _back = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, backgroundTexture->id,
                           textureImage.GetView(), textureImage.GetSampler(), _descriptorSetLayout);

    imageName = GearImageName;
    LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(
        resourcesPath + imageName, vkManager->GetImageFormat(), VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(gearTexture->id, textureImage);

    //X：右向き正、Y：下向き正
    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    _gear = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, gearTexture->id,
                           textureImage.GetView(), textureImage.GetSampler(), _descriptorSetLayout);

    imageName = PowerImageName;
    LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(
        resourcesPath + imageName, vkManager->GetImageFormat(), VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(powerTexture->id, textureImage);

    //X：右向き正、Y：下向き正
    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(height - powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    _power = new LAppSprite(device, physicalDevice, vkManager, x, y, fWidth, fHeight, powerTexture->id,
                            textureImage.GetView(), textureImage.GetSampler(), _descriptorSetLayout);

    // 画面全体を覆うサイズ
    x = width * 0.5f;
    y = height * 0.5f;
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    _renderSprite = new LAppSprite(device, physicalDevice, vkManager, x, y, static_cast<float>(width), static_cast<float>(height),0, NULL, NULL, _descriptorSetLayout);
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

float LAppView::TransformViewX(float deviceX) const
{
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float LAppView::TransformViewY(float deviceY) const
{
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float LAppView::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float LAppView::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}


void LAppView::PreModelDraw(LAppModel& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_Vulkan* useTarget = NULL;
    useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

    if (_renderTarget != SelectTarget_None)
    {
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
        Csm::Rendering::CubismRenderer_Vulkan::SetRenderTarget(useTarget->GetTextureImage(), useTarget->GetTextureView());
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
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            _renderSprite->Render(commandBuffer, _pipelineLayout, vkManager, width, height);
            EndRendering(commandBuffer);
            vkManager->SubmitCommand(commandBuffer);
        }
    }
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
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
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

void LAppView::Cleanup(VkDevice device)
{
    vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
    vkDestroyPipeline(device, _pipeline, nullptr);
}

void LAppView::ResizeSprite(int width, int height)
{
    VulkanManager* vkManager = LAppDelegate::GetInstance()->GetVulkanManager();
    VkDevice device = vkManager->GetDevice();
    SwapchainManager* swapchainManager = vkManager->GetSwapchainManager();
    Cleanup(device);
    CreateSpriteGraphicsPipeline(device, swapchainManager->GetExtent(), _vertShaderModule, _fragShaderModule,
                                 vkManager->GetSwapchainManager()->GetSwapchainImageFormat());

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
