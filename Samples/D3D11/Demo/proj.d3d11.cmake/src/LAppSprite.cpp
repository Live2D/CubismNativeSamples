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
#include "LAppSpriteShader.hpp"
#include "Rendering/D3D11/CubismType_D3D11.hpp"

using namespace LAppDefine;
using namespace DirectX;

LAppSprite::LAppSprite()
    : LAppSprite_Common(),
    _rect(),
    _vertexBuffer(NULL),
    _indexBuffer(NULL),
    _constantBuffer(NULL),
    _shader(NULL)
{
    _color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

LAppSprite::LAppSprite(float x, float y, float width, float height, Csm::csmUint32 textureId, LAppSpriteShader* shader, ID3D11Device* device)
    : LAppSprite_Common(textureId),
    _rect(),
    _vertexBuffer(NULL),
    _indexBuffer(NULL),
    _constantBuffer(NULL),
    _shader(shader)
{
    _color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);

    if (!device)
    {
        return;
    }

    HRESULT hr = S_OK;

    // 頂点バッファ作成
    if (!_vertexBuffer)
    {
        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(LAppSprite::SpriteVertex) * VERTEX_NUM;    // 総長 構造体サイズ*個数
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufferDesc, NULL, &_vertexBuffer);
        if (FAILED(hr))
        {
            if (DebugLogEnable)
            {
                LAppPal::PrintLogLn("Cannot allocate vertex data in LAppSprite");
            }
            return;
        }
    }

    // インデックスバッファ作成
    if (!_indexBuffer)
    {
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

        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(WORD) * INDEX_NUM;    // 総長 構造体サイズ*個数
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        // 生成と同時に設定
        D3D11_SUBRESOURCE_DATA subResourceData;
        memset(&subResourceData, 0, sizeof(subResourceData));
        subResourceData.pSysMem = idx;
        subResourceData.SysMemPitch = 0;
        subResourceData.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufferDesc, &subResourceData, &_indexBuffer);
        if (FAILED(hr))
        {
            if (DebugLogEnable)
            {
                LAppPal::PrintLogLn("Cannot allocate index data in LAppSprite");
            }
            return;
        }
    }

    // 定数バッファ作成
    if (!_constantBuffer)
    {
        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(CubismConstantBufferD3D11);    // 総長 構造体サイズ*個数
        bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 定数バッファに関しては「Map用にDynamic」にしなくともよい
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufferDesc, NULL, &_constantBuffer);
    }
}

LAppSprite::~LAppSprite()
{
    _shader = NULL;

    if(_constantBuffer)
    {
        _constantBuffer->Release();
        _constantBuffer = NULL;
    }
    if (_indexBuffer)
    {
        _indexBuffer->Release();
        _indexBuffer = NULL;
    }
    if (_vertexBuffer)
    {
        _vertexBuffer->Release();
        _vertexBuffer = NULL;
    }
}

void LAppSprite::RenderImmidiate(int width, int height, ID3D11ShaderResourceView* resourceView, ID3D11DeviceContext* renderContext) const
{
    if (!resourceView) return;

    if (width == 0 || height == 0)
    {
        return; // この際は描画できず
    }

    SpriteVertex vtx[VERTEX_NUM] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 1.0f, 1.0f },
    };

    vtx[0].x = (_rect.left - width * 0.5f) / (width * 0.5f); vtx[0].y = (_rect.down - height * 0.5f) / (height * 0.5f);
    vtx[1].x = (_rect.right - width * 0.5f) / (width * 0.5f); vtx[1].y = (_rect.down - height * 0.5f) / (height * 0.5f);
    vtx[2].x = (_rect.left - width * 0.5f) / (width * 0.5f); vtx[2].y = (_rect.up - height * 0.5f) / (height * 0.5f);
    vtx[3].x = (_rect.right - width * 0.5f) / (width * 0.5f); vtx[3].y = (_rect.up - height * 0.5f) / (height * 0.5f);

    // 頂点書き込み
    if (_vertexBuffer)
    {
        D3D11_MAPPED_SUBRESOURCE subRes;
        if (SUCCEEDED(renderContext->Map(_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
        {
            memcpy(subRes.pData, vtx, sizeof(SpriteVertex) * VERTEX_NUM);
            renderContext->Unmap(_vertexBuffer, 0);
        }
    }

    // 定数バッファ設定
    if (_constantBuffer)
    {
        CubismConstantBufferD3D11 cb;
        memset(&cb, 0, sizeof(cb));
        cb.baseColor = _color;
        DirectX::XMMATRIX proj = XMMatrixIdentity();
        XMStoreFloat4x4(&cb.projectMatrix, XMMatrixTranspose(proj));
        renderContext->UpdateSubresource(_constantBuffer, 0, NULL, &cb, 0, 0);

        renderContext->VSSetConstantBuffers(0, 1, &_constantBuffer);
        renderContext->PSSetConstantBuffers(0, 1, &_constantBuffer);
    }

    {
        UINT strides = sizeof(LAppSprite::SpriteVertex);
        UINT offsets = 0;

        renderContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &strides, &offsets);
        renderContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
        renderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // シェーダーセット
        if(!_shader->SetupShader())
        {
            return;
        }

        // テクスチャセット
        {
            renderContext->PSSetShaderResources(0, 1, &resourceView);
        }

        // 描画実行
        renderContext->DrawIndexed(INDEX_NUM, 0, 0);
    }
}

bool LAppSprite::IsHit(float pointX, float pointY, int clientWidth, int clientHeight) const
{
    if(clientWidth==0 || clientHeight==0)
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

void LAppSprite::ResetRect(float x, float y, float width, float height)
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);
}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _color = DirectX::XMFLOAT4(r, g, b, a);
}
