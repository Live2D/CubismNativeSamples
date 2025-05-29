/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismSprite.hpp"



#include "CubismDirectXRenderer.hpp"
#include "CubismDirectXView.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "Rendering/D3D9/CubismType_D3D9.hpp"

CubismSprite::CubismSprite()
    : LAppSprite_Common(),
    _rect(),
    _vertexStore(nullptr),
    _indexStore(nullptr)
{
    _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
}

CubismSprite::CubismSprite(float x, float y, float width, float height, Csm::csmUint64 textureId, CubismSpriteShader* shader)
    : LAppSprite_Common(textureId),
    _rect(),
    _vertexStore(nullptr),
    _indexStore(nullptr),
    _shader(shader)
{
    _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);

    if (!(CubismDirectXRenderer::GetInstance()->_device))
    {
        return;
    }

    _vertexStore = static_cast<SpriteVertex*>(CSM_MALLOC(sizeof(SpriteVertex) * VERTEX_NUM));
    _indexStore = static_cast<csmUint16*>(CSM_MALLOC(sizeof(csmUint16) * INDEX_NUM));

    // 2    3
    // +----+
    // |\   |
    // | \  |
    // |  \ |
    // |   \|
    // +----+
    // 0    1
    // インデックスはもう不変で良いのでここでコピー完了
    WORD idx[INDEX_NUM] = {
        0, 1, 2,
        1, 3, 2,
    };
    // Indexの書き込み
    if (_indexStore)
    {
        memcpy(_indexStore, idx, sizeof(WORD) * INDEX_NUM);
    }
}

CubismSprite::~CubismSprite()
{
    CubismDirectXRenderer::GetInstance()->_textureManager->ReleaseTexture(_textureId);

    // インデックス
    if (_indexStore)
    {
        CSM_FREE(_indexStore);
    }
    // 頂点
    if (_vertexStore)
    {
        CSM_FREE(_vertexStore);
    }
    _indexStore = nullptr;
    _vertexStore = nullptr;
    _shader = nullptr;
}

void CubismSprite::Render(LPDIRECT3DDEVICE9 device, int maxWidth, int maxHeight) const
{
    if (!_vertexStore)
    {
        return;
    }

    if (maxWidth == 0 || maxHeight == 0)
    {
        return; // この際は描画できず
    }

    if (device == nullptr || _shader == nullptr)
    {
        return;
    }

    // 頂点設定
    _vertexStore[0].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[0].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[0].u = 0.0f; _vertexStore[0].v = 0.0f;
    _vertexStore[1].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[1].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[1].u = 1.0f; _vertexStore[1].v = 0.0f;
    _vertexStore[2].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[2].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[2].u = 0.0f; _vertexStore[2].v = 1.0f;
    _vertexStore[3].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[3].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[3].u = 1.0f; _vertexStore[3].v = 1.0f;

    {
        // レンダーステート
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, false);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

        // シェーダー準備
        IDirect3DTexture9* texture = NULL;
        CubismDirectXRenderer::GetInstance()->_textureManager->GetTexture(_textureId, texture);
        _shader->SetShader(device, texture, _color);

        // 描画
        device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VERTEX_NUM, 2, _indexStore, D3DFMT_INDEX16, _vertexStore, sizeof(SpriteVertex));

        // シェーダー後処理
        _shader->UnsetShader();
    }
}

void CubismSprite::RenderImmidiate(LPDIRECT3DDEVICE9 device, int maxWidth, int maxHeight, LPDIRECT3DTEXTURE9 texture) const
{
    if (!_vertexStore)
    {
        return;
    }

    if (maxWidth == 0 || maxHeight == 0)
    {
        return; // この際は描画できず
    }

    if (device == nullptr || _shader == nullptr)
    {
        return;
    }

    // 頂点設定
    _vertexStore[0].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[0].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[0].u = 0.0f; _vertexStore[0].v = 0.0f;
    _vertexStore[1].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[1].y = (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[1].u = 1.0f; _vertexStore[1].v = 0.0f;
    _vertexStore[2].x = (_rect.left - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[2].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[2].u = 0.0f; _vertexStore[2].v = 1.0f;
    _vertexStore[3].x = (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f); _vertexStore[3].y = (_rect.up - maxHeight * 0.5f) / (maxHeight * 0.5f); _vertexStore[3].u = 1.0f; _vertexStore[3].v = 1.0f;

    {
        // レンダーステート
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, false);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

        // シェーダー準備
        _shader->SetShader(device, texture, _color);

        // 描画
        device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VERTEX_NUM, 2, _indexStore, D3DFMT_INDEX16, _vertexStore, sizeof(SpriteVertex));

        // シェーダー後処理
        _shader->UnsetShader();
    }
}

void CubismSprite::ResetRect(float x, float y, float width, float height)
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);
}

void CubismSprite::SetColor(float r, float g, float b, float a)
{
    _color = D3DXVECTOR4(r, g, b, a);
}
