/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppTextureManager.hpp"
#include <iostream>
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"

using namespace Live2D::Cubism::Framework;

LAppTextureManager::LAppTextureManager()
{
    _sequenceId = 0;
}

LAppTextureManager::~LAppTextureManager()
{
    ReleaseTextures();
}

void LAppTextureManager::CopyBufferToImage(VkCommandBuffer commandBuffer, const VkBuffer& buffer, VkImage image,
                                           uint32_t width, uint32_t height)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void LAppTextureManager::GenerateMipmaps(CubismImageVulkan image, uint32_t texWidth, uint32_t texHeight,
                                         uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = LAppDelegate::GetInstance()->GetVulkanManager()->BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image.GetImage();
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    LAppDelegate::GetInstance()->GetVulkanManager()->SubmitCommand(commandBuffer);
}

LAppTextureManager::TextureInfo* LAppTextureManager::CreateTextureFromPngFile(
    std::string fileName, VkFormat format,
    VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags imageProperties)
{
    VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();
    VkPhysicalDevice physicalDevice = LAppDelegate::GetInstance()->GetVulkanManager()->GetPhysicalDevice();

    //search loaded texture already.
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            return _texturesInfo[i];
        }
    }

    int width, height, channels;
    unsigned int size;
    unsigned char* png;
    unsigned char* address;

    address = LAppPal::LoadFileAsBytes(fileName, &size);

    // png情報を取得する
    png = stbi_load_from_memory(
        address,
        static_cast<int>(size),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);
    {
#ifdef PREMULTIPLIED_ALPHA_ENABLE
        unsigned int* fourBytes = reinterpret_cast<unsigned int*>(png);
        for (int i = 0; i < width * height; i++)
        {
            unsigned char* p = png + i * 4;
            fourBytes[i] = Premultiply(p[0], p[1], p[2], p[3]);
        }
#endif
    }

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(width * height * 4);
    if (!png)
    {
        LAppPal::PrintLogLn("could'nt load texture image.");
    }

    CubismBufferVulkan stagingBuffer;
    stagingBuffer.CreateBuffer(device, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Map(device, imageSize);
    stagingBuffer.MemCpy(png, imageSize);
    stagingBuffer.UnMap(device);

    // 解放処理
    stbi_image_free(png);
    LAppPal::ReleaseBytes(address);

    _mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    CubismImageVulkan textureImage;

    textureImage.CreateImage(device, physicalDevice, width, height, _mipLevels, format, tiling, usage);
    VkCommandBuffer commandBuffer = LAppDelegate::GetInstance()->GetVulkanManager()->BeginSingleTimeCommands();
    textureImage.SetImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _mipLevels, VK_IMAGE_ASPECT_COLOR_BIT);
    CopyBufferToImage(commandBuffer, stagingBuffer.GetBuffer(), textureImage.GetImage(), width, height);
    LAppDelegate::GetInstance()->GetVulkanManager()->SubmitCommand(commandBuffer);
    GenerateMipmaps(textureImage, width, height, _mipLevels);
    textureImage.CreateView(device, format, VK_IMAGE_ASPECT_COLOR_BIT, _mipLevels);
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    textureImage.CreateSampler(device, properties.limits.maxSamplerAnisotropy, _mipLevels);
    _textures.PushBack(textureImage);

    LAppTextureManager::TextureInfo* textureInfo = new LAppTextureManager::TextureInfo();
    if (textureInfo != NULL)
    {
        _sequenceId++;
        textureInfo->fileName = fileName;
        textureInfo->width = width;
        textureInfo->height = height;
        textureInfo->id = _sequenceId;
        _texturesInfo.PushBack(textureInfo);
    }
    stagingBuffer.Destroy(device);
    return textureInfo;
}

void LAppTextureManager::ReleaseTextures()
{
    VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();

    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        _textures[i].Destroy(device);
        delete _texturesInfo[i];
    }

    _texturesInfo.Clear();
}

void LAppTextureManager::ReleaseTexture(Csm::csmUint32 textureId)
{
    VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();

    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id != textureId)
        {
            continue;
        }
        // ID一致
        // info除去
        delete _texturesInfo[i];

        // 実体除去
        VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();
        _textures[i].Destroy(device);

        _texturesInfo.Remove(i);
        _textures.Remove(i);
        break;
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }
    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
}

void LAppTextureManager::ReleaseTexture(std::string fileName)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            // info除去
            delete _texturesInfo[i];

            // 実体除去
            if (_texturesInfo[i] != VK_NULL_HANDLE)
            {
                VkDevice device = LAppDelegate::GetInstance()->GetVulkanManager()->GetDevice();
                _textures[i].Destroy(device);
                _texturesInfo[i] = VK_NULL_HANDLE;
            }
            _texturesInfo.Remove(i);
            _texturesInfo.Remove(i);
            break;
        }
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }

}

LAppTextureManager::TextureInfo* LAppTextureManager::GetTextureInfoById(uint32_t textureId) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            return _texturesInfo[i];
        }
    }

    return NULL;
}

bool LAppTextureManager::GetTexture(Csm::csmUint64 textureId, CubismImageVulkan& retTexture) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            retTexture = _textures[i];
            return true;
        }
    }

    return false;
}
