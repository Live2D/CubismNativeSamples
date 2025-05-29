/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <string>

#include <Rendering/D3D9/CubismNativeInclude_D3D9.hpp>
#include <Type/CubismBasicType.hpp>
#include <Type/csmVector.hpp>

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
     * @brief 画像読み込み
     *
     * @param[in]   fileName    読み込む画像ファイルパス名
     * @param[in]   isPreMult   ロード時にピクセルのα値を計算して格納しなおす 基本的にfalseで良い
     * @param[in]   width       テクスチャ幅 D3DX_DEFAULTで画像からの自動取得
     * @param[in]   height      テクスチャ高 D3DX_DEFAULTで画像からの自動取得
     * @param[in]   mipLevel    ミップ生成LV この値が 0 または D3DX_DEFAULT の場合は、完全なミップマップ チェーンが作成される
     * @param[in]   filter      フィルタ設定 D3DX_FILTER
     * @return 画像情報。読み込み失敗時はNULLを返す
     */
    TextureInfo* CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT width, UINT height, UINT mipLevel, DWORD filter);

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
     * @brief テクスチャ・サンプラーの取得
     *
     * @param   textureId[in]       取得したいテクスチャID CreateTextureFromGnfFileで返ってきたinfoのものを指定すると良い
     * @param   retTexture[out]     成功時、IDirect3DTexture9へのポインタが返る
     * @return  テクスチャが存在していればtrueが返る
     */
    bool GetTexture(Csm::csmUint32 textureId, IDirect3DTexture9*& retTexture) const;

private:

    Csm::csmVector<IDirect3DTexture9*>  _textures;      ///< DX9テクスチャ

    Csm::csmUint32   _sequenceId;    ///< 付与するための通しID
};
