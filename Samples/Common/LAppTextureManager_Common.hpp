/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <string>

#include <Type/CubismBasicType.hpp>
#include <Type/csmVector.hpp>

/**
* @brief テクスチャ管理クラス
*
* 画像読み込み、管理を行うクラス。
*
*/
class LAppTextureManager_Common
{
public:
    /**
     * @brief 画像情報構造体
     */
    struct TextureInfo
    {
        Csm::csmUint32 id;      ///< テクスチャID
        int width;              ///< 横幅
        int height;             ///< 高さ
        std::string fileName;   ///< ファイル名
    };

    /**
     * @brief コンストラクタ
     */
    LAppTextureManager_Common();

    /**
     * @brief デストラクタ
     */
    virtual ~LAppTextureManager_Common();

    /**
     * @brief texturesInfo の解放
     */
    virtual void ReleaseTexturesInfo();

    /**
     * @brief プリマルチプライ処理
     *
     * @param[in] red  画像のRed値
     * @param[in] green  画像のGreen値
     * @param[in] blue  画像のBlue値
     * @param[in] alpha  画像のAlpha値
     *
     * @return プリマルチプライ処理後のカラー値
     */
    virtual inline unsigned int Premultiply(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    {
        return static_cast<unsigned>(\
            (red * (alpha + 1) >> 8) | \
            ((green * (alpha + 1) >> 8) << 8) | \
            ((blue * (alpha + 1) >> 8) << 16) | \
            (((alpha)) << 24)   \
            );
    }

    /**
     * @brief ファイル名からテクスチャ情報を得る
     *
     * @param[in] fileName  テクスチャのファイル名
     *
     * @return  テクスチャが存在していればtrueが返る
     */
    virtual TextureInfo* GetTextureInfoByName(std::string& fileName) const;

    /**
     * @brief テクスチャIDからテクスチャ情報を得る
     *
     * @param[in] textureId  取得したいテクスチャID
     *
     * @return テクスチャが存在していればTextureInfoが返る
     **/
    virtual TextureInfo* GetTextureInfoById(Csm::csmUint32 textureId) const;

protected:
    Csm::csmVector<TextureInfo*> _texturesInfo;         ///< テクスチャ情報
};
