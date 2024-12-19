/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D9/CubismOffscreenSurface_D3D9.hpp>
#include "CubismFramework.hpp"

/**
* @brief スプライト用のシェーダー設定を保持するクラス。
*/
class LAppSpriteShader
{
public:

    /**
     * @brief   コンストラクタ
     */
    LAppSpriteShader();

    /**
     * @brief   デストラクタ
     */
    ~LAppSpriteShader();

    /**
     * @brief   シェーダーの作成を行う
     */
    bool CreateShader();

    /**
     * @brief   シェーダーの解放を行う
     */
    void ReleaseShader();

    /**
     * @brief   スプライトの描画直前に呼び出される
     *          シェーダーのセットを行う
     */
    void SetShader(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 texture, D3DXVECTOR4 color);

    /**
     * @brief   スプライトの描画後に呼び出される
     *          シェーダーの後処理を行う
     */
    void UnsetShader();

private:

    ID3DXEffect*                    _shaderEffect;  ///< スプライト描画用シェーダ
    IDirect3DVertexDeclaration9*    _vertexFormat;  ///< スプライト描画用型宣言
};
