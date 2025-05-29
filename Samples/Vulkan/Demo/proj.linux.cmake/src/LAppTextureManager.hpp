/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <Type/csmVector.hpp>
#include <Rendering/Vulkan/CubismClass_Vulkan.hpp>
#include "LAppTextureManager_Common.hpp"

/**
* @brief テクスチャ管理クラス
*
* 画像読み込み、管理を行うクラス。
*/
class LAppTextureManager : public LAppTextureManager_Common
{
public:
    /**
    * @brief コンストラクタ
    */
    LAppTextureManager();

    /**
    * @brief デストラクタ
    *
    */
    ~LAppTextureManager();

    /**
  * @brief   バッファをイメージにコピーする
    *
    * @param[in] commandBuffer  コマンドバッファ
    * @param[in] buffer         バッファ
    * @param[in] image          イメージ
    * @param[in] width          画像の横幅
    * @param[in] height         画像の縦幅
    */
    void CopyBufferToImage(VkCommandBuffer commandBuffer, const VkBuffer& buffer, VkImage image, uint32_t width,
                           uint32_t height);

    /**
    * @brief   ミップマップを作成する
    *
    * @param[in] image          イメージ
    * @param[in] texWidth       画像の幅
    * @param[in] texHeight      画像の高さ
    * @param[in] mipLevels      ミップレべル
    */
    void GenerateMipmaps(Live2D::Cubism::Framework::CubismImageVulkan image, uint32_t texWidth, uint32_t texHeight,
                         uint32_t mipLevels);

    /**
    * @brief 画像ファイルを読み込む
    *
    * @param[in] fileName       読み込む画像ファイルパス名
    * @param[in] format         画像フォーマット
    * @param[in] tiling         画像データのタイリング配置の設定
    * @param[in] usage　　　　　画像の使用フラグ
    * @param[in] imageProperties
    *
    * @return 作成したイメージのリソース。読み込み失敗時はNULLを返す。
    */
    TextureInfo* CreateTextureFromPngFile(
        std::string fileName,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags imageProperties
        );

    /**
    * @brief 画像の解放
    *
    * 配列に存在する画像全てを解放する
    */
    void ReleaseTextures();

    /**
     * @brief 画像の解放
     *
     * 指定したテクスチャIDの画像を解放する
     * @param[in] textureId  解放するテクスチャID
     **/
    void ReleaseTexture(Csm::csmUint32 textureId);

    /**
    * @brief 画像の解放
    *
    * 指定した名前の画像を解放する
    * @param[in] fileName  解放する画像ファイルパス名
    **/
    void ReleaseTexture(std::string fileName);

    /**
     * @brief テクスチャIDからテクスチャ情報を得る
     *
     * @param   textureId[in]       取得したいテクスチャID
     * @param   retTexture          取得したいテクスチャを入れる変数
     *
     * @return  テクスチャが存在していればTextureInfoが返る
     */
    bool GetTexture(Csm::csmUint32 textureId, Live2D::Cubism::Framework::CubismImageVulkan& retTexture) const;

private:
    Csm::csmVector<Live2D::Cubism::Framework::CubismImageVulkan> _textures;
    Csm::csmUint32 _sequenceId; ///< イメージのインデックス
    uint32_t _mipLevels; ///< ミップレベル
};
