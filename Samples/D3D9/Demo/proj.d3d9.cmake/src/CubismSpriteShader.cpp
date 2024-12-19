/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismSpriteShader.hpp"

#include <d3dcompiler.h>
#include <Utils/CubismString.hpp>
#include "CubismDirectXRenderer.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"

CubismSpriteShader::CubismSpriteShader()
    : _shaderEffect(NULL)
    , _vertexFormat(NULL)
{
    CreateShader();
}

CubismSpriteShader::~CubismSpriteShader()
{
    ReleaseShader();
}

bool CubismSpriteShader::CreateShader()
{
    LPDIRECT3DDEVICE9 device = CubismDirectXRenderer::GetInstance()->_device;
    ReleaseShader();

    // ファイル読み込み
    const char* shaderCode;
    Csm::csmSizeInt shaderSize;
    {
        // シェーダーのパスの作成
        Csm::csmString shaderFileName(LAppDefine::ShaderPath);
        shaderFileName += LAppDefine::ShaderName;

        // ファイル読み込み
        shaderCode = reinterpret_cast<const char*>(LAppPal::LoadFileAsBytes(shaderFileName.GetRawString(), &shaderSize));
    }

    ID3DXBuffer* error = 0;
    HRESULT result = S_OK;
    do
    {
        // シェーダーエフェクト作成
        if (FAILED(result = D3DXCreateEffect(device, shaderCode, shaderSize, 0, 0, 0, 0, &_shaderEffect, &error)))
        {
            LAppPal::PrintLog("Cannot load the shaders");
            break;
        }

        // この描画で使用する頂点フォーマット
        D3DVERTEXELEMENT9 elems[] = {
            { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
            { 0, sizeof(float) * 2, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            D3DDECL_END()
        };
        if (FAILED(result = device->CreateVertexDeclaration(elems, &_vertexFormat)))
        {
            LAppPal::PrintLog("CreateVertexDeclaration failed");
            break;
        }
    } while(0);

    if (FAILED(result)) {
        return false;
    }

    return true;
}

void CubismSpriteShader::ReleaseShader()
{
    if (_vertexFormat)
    {
        _vertexFormat->Release();
        _vertexFormat = nullptr;
    }
    if (_shaderEffect)
    {
        _shaderEffect->Release();
        _shaderEffect = nullptr;
    }
}

void CubismSpriteShader::SetShader(LPDIRECT3DDEVICE9 device, LPDIRECT3DTEXTURE9 texture, D3DXVECTOR4 color)
{
    D3DXMATRIX proj(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
            );

    // 頂点フォーマット
    device->SetVertexDeclaration(_vertexFormat);

    _shaderEffect->SetTechnique("ShaderNames_Normal");

    // numPassには指定のtechnique内に含まれるpassの数が返る
    UINT numPass = 0;
    _shaderEffect->Begin(&numPass, 0);
    _shaderEffect->BeginPass(0);

    _shaderEffect->SetMatrix("projectMatrix", &proj);

    _shaderEffect->SetVector("baseColor", &color);

    _shaderEffect->SetTexture("mainTexture", texture);

    _shaderEffect->CommitChanges();
}

void CubismSpriteShader::UnsetShader()
{
    _shaderEffect->EndPass();

    _shaderEffect->SetTexture("mainTexture", NULL);
    _shaderEffect->CommitChanges();

    _shaderEffect->End();
}
