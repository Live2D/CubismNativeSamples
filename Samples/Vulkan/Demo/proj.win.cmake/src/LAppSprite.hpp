/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once
#include <Rendering/Vulkan/CubismClass_Vulkan.hpp>

// 前方宣言
class VulkanManager;

/**
* @brief スプライトを実装するクラス。
*
* テクスチャID、Rectの管理。
*
*/
class LAppSprite
{
public:
    /**
    * @brief Rect 構造体。
    */
    struct Rect
    {
    public:
        float left; ///< 左辺
        float right; ///< 右辺
        float up; ///< 上辺
        float down; ///< 下辺
    };

    /**
     * @brief   頂点情報を保持する構造体
     *
     */
    struct SpriteVertex
    {
        float x, y; // Position
        float u, v; // UVs

        /**
         * @brief   頂点入力のバインド設定を指定する
         */
        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(SpriteVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        /**
         * @brief   頂点入力の構造を指定する
         */
        static void GetAttributeDescriptions(VkVertexInputAttributeDescription attributeDescriptions[2])
        {
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(SpriteVertex, x);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(SpriteVertex, u);
        }
    };

    /**
     * @brief   スプライト用ユニフォームバッファオブジェクトの中身を保持する構造体
     *
     */
    struct SpriteUBO
    {
        float spriteColor[4];
    };

    /**
    * @brief コンストラクタ
    *
    * @param[in]       device                  デバイス
    * @param[in]       physicalDevice          物理デバイス
    * @param[in]       vkManager               Vulkanリソースマネージャ
    * @param[in]       x                       x座標
    * @param[in]       y                       y座標
    * @param[in]       width                   横幅
    * @param[in]       height                  高さ
    * @param[in]       textureId               テクスチャID
    * @param[in]       view                    テクスチャビュー
    * @param[in]       sampler                 テクスチャサンプラー
    * @param[in]       descriptorSetLayout     ディスクリプタセット
    */
    LAppSprite(
        VkDevice device, VkPhysicalDevice physicalDevice, VulkanManager* vkManager, float x, float y, float width, float height,
        uint32_t textureId, VkImageView view, VkSampler sampler, VkDescriptorSetLayout descriptorSetLayout
    );

    /**
    * @brief デストラクタ
    */
    ~LAppSprite();

    /**
    * @brief リソースを開放する
    *
    * @param[in]       device                  デバイス
    */
   void Release(VkDevice device);

    /**
    * @brief ディスクリプタセットを作成する
    *
    * @param[in]       device                  デバイス
    * @param[in]       descriptorSetLayout     ディスクリプタセットレイアウト
    */
    void CreateDescriptorSet(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);

    /**
    * @brief ユニフォームバッファを更新する
    *
    * @param[in]       vkManager           Vulkanリソースマネージャ
    * @param[in]       maxWidth            最大幅
    * @param[in]       maxHeight           最大高さ
    */
    void UpdateData(VulkanManager* vkManager, int maxWidth, int maxHeight) const;

    /**
     * @brief ディスクリプタセットを更新する
     *
     * @param[in]       device                  デバイス
     * @param[in]       view                    テクスチャビュー
     * @param[in]       sampler                 テクスチャサンプラー
     */
    void UpdateDescriptorSet(VkDevice device, VkImageView view, VkSampler sampler);

    /**
     * @brief ディスクリプタセットの更新フラグを設定する
     *
     * @param[in]       frag                  フラグ
     */
    void SetDescriptorUpdated(bool frag);

    /**
    * @brief 描画する
    *
     * @param[in]       commandBuffer          コマンドバッファ
     * @param[in]       pipelineLayout         パイプラインレイアウト
     * @param[in]       vkManager              Vulkanリソースマネージャ
     * @param[in]       windowWidth            ウィンドウ幅
     * @param[in]       windowHeight           ウィンドウ高さ
    */
    void Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, VulkanManager* vkManager, int windowWidth, int windowHeight);

    /**
    * @brief コンストラクタ
    *
    * @param[in]       windowWidth        ウィンドウ幅
    * @param[in]       windowHeight       ウィンドウ高さ
    * @param[in]       pointX             x座標
    * @param[in]       pointY             y座標
    */
    bool IsHit(int windowWidth, int windowHeight, float pointX, float pointY) const;

    /**
     * @brief 色設定
     *
     * @param[in]       r (0.0~1.0)
     * @param[in]       g (0.0~1.0)
     * @param[in]       b (0.0~1.0)
     * @param[in]       a (0.0~1.0)
     */
    void SetColor(float r, float g, float b, float a);

    /**
     * @brief サイズ再設定
     *
     * @param[in]       x            x座標
     * @param[in]       y            y座標
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     */
    void ResetRect(float x, float y, float width, float height);

    /**
    * @brief Getter テクスチャID
    * @return テクスチャIDを返す
    */
    uint32_t GetTextureId() { return _textureId; }


private:
    static const uint16_t VertexNum = 4;
    static const uint16_t IndexNum = 6;
    Csm::csmUint32 _textureId; ///< テクスチャID
    Rect _rect; ///< 矩形
    float _spriteColor[4]; ///< 表示カラー
    Live2D::Cubism::Framework::CubismBufferVulkan _vertexBuffer; ///< 頂点バッファ
    Live2D::Cubism::Framework::CubismBufferVulkan _stagingBuffer; ///< ステージングバッファ
    Live2D::Cubism::Framework::CubismBufferVulkan _indexBuffer; ///< インデックスバッファ
    Live2D::Cubism::Framework::CubismBufferVulkan _uniformBuffer; ///< ユニフォームバッファ
    VkDescriptorPool _descriptorPool; ///< ディスクリプタプール
    VkDescriptorSet _descriptorSet; ///< ディスクリプタセット モデルの数
    bool isDescriptorUpdated;
};
