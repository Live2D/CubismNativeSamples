/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"
#include "LAppTextureManager.hpp"
#include "VulkanManager.hpp"

using namespace Csm;

LAppSprite::LAppSprite(
    VkDevice device, VkPhysicalDevice physicalDevice, VulkanManager* vkManager, float x, float y, float width, float height,
    Csm::csmUint32 textureId, VkImageView view, VkSampler sampler, VkDescriptorSetLayout descriptorSetLayout)
    : _rect()
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y - height * 0.5f);
    _rect.down = (y + height * 0.5f);
    _textureId = textureId;

    _spriteColor[0] = 1.0f;
    _spriteColor[1] = 1.0f;
    _spriteColor[2] = 1.0f;
    _spriteColor[3] = 1.0f;

    if (_vertexBuffer.GetBuffer() == VK_NULL_HANDLE)
    {
        VkDeviceSize bufferSize = sizeof(SpriteVertex) * VertexNum; // 総長 構造体サイズ*個数
        _stagingBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _stagingBuffer.Map(device, bufferSize);
        _vertexBuffer.CreateBuffer(device, physicalDevice, bufferSize,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    if (_indexBuffer.GetBuffer() == VK_NULL_HANDLE)
    {
        uint16_t idx[IndexNum] = {
            0, 1, 2,
            1, 3, 2
        };

        uint64_t bufferSize = sizeof(uint16_t) * IndexNum;

        CubismBufferVulkan stagingBuffer;
        stagingBuffer.CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer.Map(device, bufferSize);
        stagingBuffer.MemCpy(idx, bufferSize);
        stagingBuffer.UnMap(device);

        _indexBuffer.CreateBuffer(device, physicalDevice, bufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();
        VkBufferCopy copyRegion{};
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(commandBuffer, stagingBuffer.GetBuffer(), _indexBuffer.GetBuffer(), 1, &copyRegion);
        vkManager->SubmitCommand(commandBuffer);

        stagingBuffer.Destroy(device);
    }

    if (_uniformBuffer.GetBuffer() == VK_NULL_HANDLE)
    {
        _uniformBuffer.CreateBuffer(device, physicalDevice, sizeof(SpriteUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _uniformBuffer.Map(device, VK_WHOLE_SIZE);
    }

    VkDescriptorPoolSize poolSizes[2]{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to create descriptor pool!");
    }

    CreateDescriptorSet(device, descriptorSetLayout);
    if (view != NULL && sampler != NULL)
    {
        SetDescriptorUpdated(false);
        UpdateDescriptorSet(device, view, sampler);
    }
}

LAppSprite::~LAppSprite()
{}

void LAppSprite::Release(VkDevice device)
{
    _vertexBuffer.Destroy(device);
    _stagingBuffer.Destroy(device);
    _indexBuffer.Destroy(device);
    _uniformBuffer.Destroy(device);
    vkDestroyDescriptorPool(device, _descriptorPool, nullptr);
}

void LAppSprite::CreateDescriptorSet(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    if (vkAllocateDescriptorSets(device, &allocInfo, &_descriptorSet) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to allocate descriptor sets!");
    }
}

void LAppSprite::UpdateData(VulkanManager* vkManager, int maxWidth, int maxHeight) const
{
    if (maxWidth == 0 || maxHeight == 0)
    {
        return; // この際は描画できず
    }

    SpriteVertex vtx[VertexNum] = {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f},
    };

    vtx[0].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f);
    vtx[0].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f);
    vtx[1].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f);
    vtx[1].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f);
    vtx[2].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f);
    vtx[2].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f);
    vtx[3].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f);
    vtx[3].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f);

    SpriteVertex vertex;
    csmVector<SpriteVertex> vertices;
    for (SpriteVertex vt : vtx)
    {
        vertex.x = vt.x;
        vertex.y = vt.y;
        vertex.u = vt.u;
        vertex.v = vt.v;
        vertices.PushBack(vertex);
    }

    VkCommandBuffer commandBuffer = vkManager->BeginSingleTimeCommands();
    csmUint32 bufferSize = sizeof(SpriteVertex) * vertices.GetSize();

    _stagingBuffer.MemCpy(vertices.GetPtr(), bufferSize);

    VkBufferCopy copyRegion{};
    copyRegion.size = bufferSize;
    vkCmdCopyBuffer(commandBuffer, _stagingBuffer.GetBuffer(), _vertexBuffer.GetBuffer(), 1, &copyRegion);

    vkManager->SubmitCommand(commandBuffer);

    //ユニフォームバッファ設定
    SpriteUBO _ubo;
    _ubo.spriteColor[0] = _spriteColor[0];
    _ubo.spriteColor[1] = _spriteColor[1];
    _ubo.spriteColor[2] = _spriteColor[2];
    _ubo.spriteColor[3] = _spriteColor[3];
    _uniformBuffer.MemCpy(&_ubo, sizeof(SpriteUBO));
}

void LAppSprite::UpdateDescriptorSet(VkDevice device, VkImageView view, VkSampler sampler)
{
    if (!isDescriptorUpdated)
    {
        csmVector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.Resize(2);

        VkDescriptorBufferInfo uniformBufferInfo{};
        uniformBufferInfo.buffer = _uniformBuffer.GetBuffer();
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = VK_WHOLE_SIZE;

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSet;
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

        VkDescriptorImageInfo imageInfo;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = view;
        imageInfo.sampler = sampler;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSet;
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, descriptorWrites.GetSize(), descriptorWrites.GetPtr(), 0, NULL);
        isDescriptorUpdated = true;
    }
}

void LAppSprite::SetDescriptorUpdated(bool frag)
{
    isDescriptorUpdated = frag;
}

void LAppSprite::Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VulkanManager* vkManager, int windowWidth, int windowHeight)
{
    UpdateData(vkManager, windowWidth, windowHeight);
    VkBuffer vertexBuffers[] = {_vertexBuffer.GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &_descriptorSet, 0,nullptr);
    vkCmdDrawIndexed(commandBuffer, IndexNum, 1, 0, 0, 0);
}

bool LAppSprite::IsHit(int windowWidth, int windowHeight, float pointX, float pointY) const
{
    if (windowWidth == 0 || windowHeight == 0)
    {
        return false; // この際は描画できず
    }

    float y = pointY;

    return (pointX >= _rect.left && pointX <= _rect.right && y >= _rect.up && y <= _rect.down);
}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _spriteColor[0] = r;
    _spriteColor[1] = g;
    _spriteColor[2] = b;
    _spriteColor[3] = a;
}

void LAppSprite::ResetRect(float x, float y, float width, float height)
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y - height * 0.5f);
    _rect.down = (y + height * 0.5f);
}
