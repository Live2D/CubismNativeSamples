/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"

#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppTextureManager.hpp"

using namespace LAppDefine;

LAppSprite::LAppSprite()
    : LAppSprite_Common(),
    _rect(),
    _vertexStore(NULL),
    _indexStore(NULL),
    _shader(NULL)
{
    _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
}

LAppSprite::LAppSprite(float x, float y, float width, float height, Csm::csmUint64 textureId, LAppSpriteShader* shader)
    : LAppSprite_Common(textureId),
    _rect(),
    _vertexStore(NULL),
    _indexStore(NULL),
    _shader(shader)
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);

    _color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

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
    if(_indexStore)
    {
        memcpy(_indexStore, idx, sizeof(WORD) * INDEX_NUM);
    }
}

LAppSprite::~LAppSprite()
{
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
    _indexStore = NULL;
    _vertexStore = NULL;
    _shader = NULL;
}

void LAppSprite::RenderImmidiate(LPDIRECT3DDEVICE9 device, int maxWidth, int maxHeight, LPDIRECT3DTEXTURE9 texture) const
{
    if (!_vertexStore)
    {
        return;
    }

    if (maxWidth == 0 || maxHeight == 0)
    {
        return; // この際は描画できず
    }

    if (device == NULL || _shader == NULL)
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

bool LAppSprite::IsHit(float pointX, float pointY, int clientWidth, int clientHeight) const
{
    if (clientWidth == 0 || clientHeight == 0)
    {// この際はヒットしない
        return false;
    }

    // フルスクリーン座標に変換
    float coordX = 0.0f, coordY = 0.0f;
    LAppPal::CoordinateWindowToFullScreen(static_cast<float>(clientWidth), static_cast<float>(clientHeight), pointX, pointY, coordX, coordY);

    coordX = (clientWidth+coordX)/(2.0f*clientWidth) * clientWidth;
    coordY = (clientHeight+coordY)/(2.0f*clientHeight) * clientHeight;

    return (coordX >= _rect.left && coordX <= _rect.right && coordY <= _rect.up && coordY >= _rect.down);
}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _color = D3DXVECTOR4(r, g, b, a);
}
