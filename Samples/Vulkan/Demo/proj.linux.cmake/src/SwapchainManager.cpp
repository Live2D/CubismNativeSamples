/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "SwapchainManager.hpp"

using namespace Csm;

SwapchainManager::SwapchainManager(GLFWwindow* _window, VkPhysicalDevice physicalDevice, VkDevice device,
                                   VkSurfaceKHR surface, Csm::csmUint32 graphicsFamily, Csm::csmUint32 presentFamily)
    : _extent(VkExtent2D{0, 0})
    , _imageCount(0)
{
    CreateSwapchain(_window, physicalDevice, device, surface, graphicsFamily, presentFamily);
}

SwapchainManager::SwapchainSupportDetails SwapchainManager::QuerySwapchainSupport(
    VkPhysicalDevice& physicalDevice, VkSurfaceKHR surface)
{
    SwapchainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    csmUint32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.Resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.GetPtr());
    }

    csmUint32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.Resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount,
                                                  details.presentModes.GetPtr());
    }

    return details;
}

VkSurfaceFormatKHR SwapchainManager::ChooseSwapSurfaceFormat(const csmVector<VkSurfaceFormatKHR>& availableFormats)
{
    for (csmUint32 i = 0; i < availableFormats.GetSize(); i++)
    {
        if (availableFormats[i].format == _swapchainFormat && availableFormats[i].colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormats[i];
        }
    }
    //他に使えるフォーマットが無かったら最初のフォーマットを使う
    return availableFormats[0];
}

VkPresentModeKHR SwapchainManager::ChooseSwapPresentMode(const csmVector<VkPresentModeKHR>& availablePresentModes)
{
    for (csmUint32 i = 0; i < availablePresentModes.GetSize(); i++)
    {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentModes[i];
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        //for Retina display
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<csmUint32>(width),
            static_cast<csmUint32>(height)
        };

        if (actualExtent.width < capabilities.minImageExtent.width)
        {
            actualExtent.width = capabilities.minImageExtent.width;
        }
        else if (actualExtent.width > capabilities.maxImageExtent.width)
        {
            actualExtent.width = capabilities.maxImageExtent.width;
        }

        if (actualExtent.height < capabilities.minImageExtent.height)
        {
            actualExtent.height = capabilities.minImageExtent.height;
        }
        else if (actualExtent.height > capabilities.maxImageExtent.height)
        {
            actualExtent.height = capabilities.maxImageExtent.height;
        }

        return actualExtent;
    }
}

void SwapchainManager::CreateSwapchain(GLFWwindow* window, VkPhysicalDevice physicalDevice, VkDevice device,
                                       VkSurfaceKHR surface, Csm::csmUint32 graphicsFamily,
                                       Csm::csmUint32 presentFamily)
{
    SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    _extent = ChooseSwapExtent(window, swapChainSupport.capabilities);

    _imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && _imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        _imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = _imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = _extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (graphicsFamily != presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        Csm::csmUint32 queueFamilyIndices[2] = {graphicsFamily, presentFamily};
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &_swapchain) != VK_SUCCESS)
    {
        LAppPal::PrintLogLn("failed to create swap chain");
    }

    // swapchain imageを取得する
    vkGetSwapchainImagesKHR(device, _swapchain, &_imageCount, nullptr);
    _images.Resize(_imageCount);
    vkGetSwapchainImagesKHR(device, _swapchain, &_imageCount, _images.GetPtr());

    _imageViews.Resize(_imageCount);
    for (csmUint32 i = 0; i < _imageCount; i++)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _swapchainFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &_imageViews[i]) != VK_SUCCESS)
        {
            LAppPal::PrintLogLn("failed to create texture image view");
        }
    }
}

VkResult SwapchainManager::QueuePresent(VkQueue& queue, csmUint32 imageIndex)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSwapchainKHR swapChains[] = {_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    return vkQueuePresentKHR(queue, &presentInfo);
}

void SwapchainManager::Cleanup(VkDevice device)
{
    if (_swapchain != VK_NULL_HANDLE)
    {
        for (csmUint32 i = 0; i < _imageViews.GetSize(); i++)
        {
            vkDestroyImageView(device, _imageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(device, _swapchain, nullptr);
    }
}
