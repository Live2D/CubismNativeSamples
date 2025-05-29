/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D9/CubismNativeInclude_D3D9.hpp>

#include <CubismFramework.hpp>
#include <Type/CubismBasicType.hpp>

#include "LAppSprite_Common.hpp"
#include "LAppSpriteShader.hpp"

/**
* @brief スプライトを実装するクラス。
*
* テクスチャID、Rectの管理。
*
*/
class LAppSprite : public LAppSprite_Common
{
public:
    struct SpriteVertex
    {
        float x, y;     // Position
        float u, v;     // UVs
    };

    /**
    * @brief デフォルトコンストラクタ
    *
    */
    LAppSprite();

    /**
    * @brief コンストラクタ
    *
    * @param[in]       x            x座標
    * @param[in]       y            y座標
    * @param[in]       width        横幅
    * @param[in]       height       高さ
    * @param[in]       textureId    テクスチャID
    * @param[in]       shader       シェーダー操作クラス
    */
    LAppSprite(float x, float y, float width, float height, Csm::csmUint64 textureId, LAppSpriteShader* shader);

    /**
    * @brief デストラクタ
    */
    ~LAppSprite();

    /**
     * @brief テクスチャを指定しての描画
     * @param[in]        device        D3D9デバイス
     * @param[in]        width         幅
     * @param[in]        height        高さ
     * @param[in]        texture       使用テクスチャ
     */
    void RenderImmidiate(LPDIRECT3DDEVICE9 device, int maxWidth, int maxHeight, LPDIRECT3DTEXTURE9 texture) const;

    /**
    * @brief ヒットチェック
    *
    * @param[in]       pointX    x座標
    * @param[in]       pointY    y座標
    * @param[in]       clientWidth  クライアントウィンドウ幅
    * @param[in]       clientHeight クライアントウィンドウ高さ
    */
    bool IsHit(float pointX, float pointY, int clientWidth, int clientHeight) const;

    /**
     * @brief 色設定
     *
     * @param[in]       r (0.0~1.0)
     * @param[in]       g (0.0~1.0)
     * @param[in]       b (0.0~1.0)
     * @param[in]       a (0.0~1.0)
     */
    void SetColor(float r, float g, float b, float a);

    // 4頂点・6インデックス 単なる四角描画に付きこれは不変
    static const int VERTEX_NUM = 4;
    static const int INDEX_NUM = 6;

private:

    Rect _rect;                 ///< 矩形

    SpriteVertex*                           _vertexStore;   ///< 頂点をストアしておく領域
    Live2D::Cubism::Framework::csmUint16*   _indexStore;    ///< インデックスをストアしておく領域
    D3DXVECTOR4                             _color;         ///< 描画色

    LAppSpriteShader* _shader;   ///< シェーダー
};
