/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>

/**
 * @brief   スプライト用のシェーダー設定を保持するクラス。
 */
class CubismSpriteShader
{
public:

    /**
     * @brief   コンストラクタ
     */
    CubismSpriteShader();

    /**
     * @brief   デストラクタ
     */
    ~CubismSpriteShader();

    /**
     * @brief   シェーダーの作成を行う
     */
    bool CreateShader();

    /**
     * @brief   スプライトの描画直前に呼び出される
     *          シェーダーのセットを行う
     */
    void SetupShader();

    /**
     * @brief   シェーダーの解放を行う
     */
    void ReleaseShader();

private:

    ID3D11RasterizerState*  _rasterizer;    ///< ラスタライザ
    ID3D11SamplerState*     _samplerState;  ///< サンプラーステート
    ID3D11BlendState*       _blendState;    ///< ブレンドステート
    ID3D11VertexShader*     _vertexShader;  ///< 頂点シェーダ
    ID3D11PixelShader*      _pixelShader;   ///< ピクセルシェーダ
    ID3D11InputLayout*      _vertexFormat;  ///< 頂点フォーマット
};
