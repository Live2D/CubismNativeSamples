/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/Vulkan/CubismOffscreenSurface_Vulkan.hpp>
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>
#include "CubismFramework.hpp"
#include "LAppSpritePipeline.hpp"

/**
* @brief Modelスプライト用パイプラインクラス
*/
class LAppModelSpritePipeline : public LAppSpritePipeline
{
public:
    /**
     * @brief   コンストラクタ
     *
     * @param[in]  device             論理デバイス
     * @param[in]  extent             フレームバッファのサイズ
     * @param[in]  swapchainFormat    スワップチェーンフォーマット
     */
    LAppModelSpritePipeline(VkDevice device, VkExtent2D extent, VkFormat swapchainFormat);
};
