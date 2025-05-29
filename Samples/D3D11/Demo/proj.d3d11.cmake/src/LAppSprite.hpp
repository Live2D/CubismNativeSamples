/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>

#include <CubismFramework.hpp>
#include <Type/CubismBasicType.hpp>

#include "LAppSprite_Common.hpp"

class LAppSpriteShader;

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

    LAppSprite();

    /**
     * @brief コンストラクタ
     *
     * @param[in]       x            x座標
     * @param[in]       y            y座標
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     * @param[in]       textureId    テクスチャID
     * @param[in]       shader       シェーダー設定
     * @param[in]       device       D3Dデバイス
     */
    LAppSprite(float x, float y, float width, float height, Csm::csmUint32 textureId, LAppSpriteShader* shader, ID3D11Device* device);

    /**
     * @brief デストラクタ
     */
    ~LAppSprite();

    /**
     * @brief テクスチャを指定しての描画
     * @param[in]        width   幅
     * @param[in]        height  高さ
     * @param[in]        resourceView   使用テクスチャ
     * @param[in]        renderContext  デバイスコンテキスト
     */
    void RenderImmidiate(int width, int height, ID3D11ShaderResourceView* resourceView, ID3D11DeviceContext* renderContext) const;

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
    Rect _rect;                 ///< 矩形

    ID3D11Buffer* _vertexBuffer;    ///< 頂点バッファ
    ID3D11Buffer* _indexBuffer;     ///< インデックスバッファ
    ID3D11Buffer* _constantBuffer;  ///< 定数バッファ

    DirectX::XMFLOAT4   _color;     ///< スプライトカラー

    LAppSpriteShader* _shader;
};
