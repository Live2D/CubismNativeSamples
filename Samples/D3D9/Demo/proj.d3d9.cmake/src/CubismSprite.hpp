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

 /**
 * @brief スプライトを実装するクラス。
 *
 * テクスチャID、Rectの管理。
 *
 */
class CubismSprite
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

    /**
    * @brief コンストラクタ
    */
    CubismSprite();

    /**
    * @brief コンストラクタ
    *
    * @param[in]       x            x座標
    * @param[in]       y            y座標
    * @param[in]       width        横幅
    * @param[in]       height       高さ
    * @param[in]       textureId    テクスチャID
    */
    CubismSprite(float x, float y, float width, float height, Csm::csmUint64 textureId);

    /**
    * @brief デストラクタ
    */
    ~CubismSprite();

    /**
    * @brief Getter テクスチャID
    * @return テクスチャIDを返す
    */
    Csm::csmUint64 GetTextureId() const { return _textureId; }

    /**
    * @brief 描画する
    * @param[in]        device 描画デバイス
    * @param[in]        width   幅
    * @param[in]        height  高さ
    */
    void Render(LPDIRECT3DDEVICE9 device, int width, int height) const;

    /**
    * @brief テクスチャを指定しての描画
    * @param[in]        width   幅
    * @param[in]        height  高さ
    * @param[in]        resourceView   使用テクスチャ
    */
    void RenderImmidiate(LPDIRECT3DDEVICE9 device, int maxWidth, int maxHeight, LPDIRECT3DTEXTURE9 texture) const;

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

    Csm::csmUint64 _textureId;  ///< テクスチャID
    Rect _rect;                 ///< 矩形

    SpriteVertex* _vertexStore;   ///< 頂点をストアしておく領域
    Live2D::Cubism::Framework::csmUint16* _indexStore;    ///< インデックスをストアしておく領域
    D3DXVECTOR4 _color;     ///< スプライトカラー
};

