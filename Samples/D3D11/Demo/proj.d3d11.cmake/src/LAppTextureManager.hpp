/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <wincodec.h> // for IWICImagingFactory
#include <WICTextureLoader.h>

#include <string>

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>
#include <Type/CubismBasicType.hpp>
#include <Type/csmVector.hpp>

/**
* @brief テクスチャ管理クラス
*
* 画像読み込み、管理を行うクラス。
*/
class LAppTextureManager
{
public:

    /**
    * @brief 画像情報構造体
    */
    struct TextureInfo
    {
        Csm::csmUint64 id;      ///< テクスチャID
        int width;              ///< 横幅
        int height;             ///< 高さ
        std::string fileName;   ///< ファイル名
    };

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
    * @brief プリマルチプライ処理
    *
    * @param[in] red  画像のRed値
    * @param[in] green  画像のGreen値
    * @param[in] blue  画像のBlue値
    * @param[in] alpha  画像のAlpha値
    *
    * @return プリマルチプライ処理後のカラー値
    */
    inline unsigned int Premultiply(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    {
        return static_cast<unsigned>(\
            (red * (alpha + 1) >> 8) | \
            ((green * (alpha + 1) >> 8) << 8) | \
            ((blue * (alpha + 1) >> 8) << 16) | \
            (((alpha)) << 24)   \
            );
    }

    /**
     * @brief 画像読み込み
     *
     * @param[in]   fileName    読み込む画像ファイルパス名
     * @param[in]   isPreMult   ロード時にピクセルのα値を計算して格納しなおす 基本的にfalseで良い
     * @param[in]   maxSize     テクスチャの最大サイズ 0だと画像の幅と高さを採用する
     * @return 画像情報。読み込み失敗時はNULLを返す
     */
    TextureInfo* CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT maxSize = 0);

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
    void ReleaseTexture(Csm::csmUint64 textureId);

    /**
    * @brief 画像の解放
    *
    * 指定した名前の画像を解放する
    * @param[in] fileName  解放する画像ファイルパス名
    **/
    void ReleaseTexture(std::string fileName);

    /**
     * @brief テクスチャ・サンプラーの取得
     *
     * @param   textureId[in]       取得したいテクスチャID CreateTextureFromGnfFileで返ってきたinfoのものを指定すると良い
     * @param   retTexture[out]    成功時、IDirect3DTexture9へのポインタが返る
     * @return  テクスチャが存在していればtrueが返る
     */
    bool GetTexture(Csm::csmUint64 textureId, ID3D11ShaderResourceView*& retTexture) const;

    /**
     * @brief ファイル名からテクスチャ情報を得る
     *
     * @param   fileName[in]       取得したいテクスチャファイル名
     * @return  テクスチャが存在していればTextureInfoが返る
     */
    TextureInfo* GetTextureInfoByName(std::string& fileName) const;

    /**
     * @brief ファイル名からテクスチャ情報を得る
     *
     * @param   textureId[in]       取得したいテクスチャID
     * @return  テクスチャが存在していればTextureInfoが返る
     */
    TextureInfo* GetTextureInfoById(Csm::csmUint64 textureId) const;

private:

    Csm::csmVector<ID3D11Resource*>             _textures;          ///< DX11テクスチャ
    Csm::csmVector<ID3D11ShaderResourceView*>   _textureView;       ///< DX11テクスチャ
    Csm::csmVector<TextureInfo*>                _texturesInfo;      ///< テクスチャ情報

    Csm::csmUint64   _sequenceId;    ///< 付与するための通しID
};
