/*
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>

#include <CubismFramework.hpp>
#include <Type/CubismBasicType.hpp>



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
        float left;     ///< 左辺
        float right;    ///< 右辺
        float up;       ///< 上辺
        float down;     ///< 下辺
    };

    struct SpriteVertex
    {
        float x, y;     // Position
        float u, v;     // UVs
    };

    LAppSprite();

    /**
     * @brief コンストラクタ
     *
     * @param[in]       x            x座標
     * @param[in]       y            y座標
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     * @param[in]       textureId    テクスチャID
     */
    LAppSprite(float x, float y, float width, float height, Csm::csmUint64 textureId);

    /**
     * @brief デストラクタ
     */
    ~LAppSprite();

    /**
     * @brief Getter テクスチャID
     * @return テクスチャIDを返す
     */
    Csm::csmUint64 GetTextureId() { return _textureId; }

    /**
     * @brief 描画する
     * @param[in]        width   幅
     * @param[in]        height  高さ
     */
    void Render(int width, int height) const;

    /**
     * @brief ヒットチェック
     *
     * @param[in]       pointX    x座標
     * @param[in]       pointY    y座標
     */
    bool IsHit(float pointX, float pointY) const;

    /**
     * @brief サイズ再設定
     *
     * @param[in]       x            x座標
     * @param[in]       y            y座標
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     */
    void ResetRect(float x, float y, float width, float height);

    // 4頂点・6インデックス 単なる四角描画に付きこれは不変 
    static const int VERTEX_NUM = 4;
    static const int INDEX_NUM = 6;

private:

    Csm::csmUint64 _textureId;  ///< テクスチャID 
    Rect _rect;                 ///< 矩形 

    ID3D11Buffer* _vertexBuffer;    ///< 頂点バッファ 
    ID3D11Buffer* _indexBuffer;     ///< インデックスバッファ 
    ID3D11Buffer* _constantBuffer;  ///< 定数バッファ 
};

