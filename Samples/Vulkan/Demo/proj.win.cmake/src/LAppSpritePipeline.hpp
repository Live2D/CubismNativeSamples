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

/**
* @brief スプライト用パイプラインクラス
*/
class LAppSpritePipeline
{
protected:
    /**
     * @brief   コンストラクタ
     *
     * @param[in]  device             論理デバイス
     */
    LAppSpritePipeline(VkDevice device);

public:
    /**
     * @brief   コンストラクタ
     *
     * @param[in]  device             論理デバイス
     * @param[in]  extent             フレームバッファのサイズ
     * @param[in]  swapchainFormat    スワップチェーンフォーマット
     */
    LAppSpritePipeline(VkDevice device, VkExtent2D extent, VkFormat swapchainFormat);

    /**
     * @brief   デストラクタ
     */
    ~LAppSpritePipeline();

    /**
     * @brief パイプラインレイアウトを取得する
     */
    VkPipelineLayout GetPipelineLayout() const;

    /**
     * @brief パイプラインを取得する
     */
    VkPipeline GetPipeline() const;

    /**
     * @brief ディスクリプタセットレイアウトを取得する
     */
    VkDescriptorSetLayout GetDescriptorSetLayout() const;

private:
    /**
     * @brief   スプライト用のディスクリプタセットのレイアウトを作成する。
     *
     * @param[in]       device            論理デバイス
     */
    void CreateDescriptorSetLayout(VkDevice device);

    /**
     * @brief   パイプラインレイアウトを作成する。
     *
     * @param[in]       device                 論理デバイス
     * @param[in]       descriptorSetLayout    ディスクリプタセットレイアウト
     */
    void CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

protected:
    /**
     * @brief スプライト用のパイプラインを作成する。
     *
     * @param[in]       device                  論理デバイス
     * @param[in]       extent                  フレームバッファのサイズ
     * @param[in]       swapchainFormat         スワップチェーンフォーマット
     */
    void CreatePipeline(VkDevice device, VkExtent2D extent, VkFormat swapchainFormat, const VkPipelineColorBlendAttachmentState& colorBlendAttachment);

    /**
     * @brief   シェーダーモジュ―ルを作成する
     *
     * @param[in]  device      論理デバイス
     * @param[in]  filename    シェーダーのファイル名
     */
    VkShaderModule CreateShaderModule(VkDevice device, Csm::csmString filename);

    VkDescriptorSetLayout _descriptorSetLayout;                ///< ディスクリプタセットのレイアウト, UBOが1つとテクスチャが1つずつ
    VkPipelineLayout _pipelineLayout;                          ///< スプライト描画に使うパイプラインのレイアウト
    VkPipeline _pipeline;                                      ///< スプライト描画に使うパイプライン
};
