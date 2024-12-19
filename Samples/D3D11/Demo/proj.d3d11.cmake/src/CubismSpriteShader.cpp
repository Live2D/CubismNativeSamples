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
    : _rasterizer(NULL)
    , _samplerState(NULL)
    , _vertexShader(NULL)
    , _pixelShader(NULL)
    , _blendState(NULL)
    , _vertexFormat(NULL)
{
}

CubismSpriteShader::~CubismSpriteShader()
{
}

bool CubismSpriteShader::CreateShader()
{
    ReleaseShader();

    ID3D11Device* device = CubismDirectXRenderer::GetInstance()->GetD3dDevice();
    HRESULT result = S_OK;

    ID3DBlob* vertexError = NULL;
    ID3DBlob* pixelError = NULL;

    ID3DBlob* vertexBlob = NULL;
    ID3DBlob* pixelBlob = NULL;

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

    do
    {
        // ラスタライザ
        D3D11_RASTERIZER_DESC rasterDesc;
        memset(&rasterDesc, 0, sizeof(rasterDesc));
        rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK; // 裏面を切る
        rasterDesc.FrontCounterClockwise = TRUE; // CCWを表面にする
        rasterDesc.DepthClipEnable = FALSE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.DepthBiasClamp = 0;
        rasterDesc.SlopeScaledDepthBias = 0;
        result = device->CreateRasterizerState(&rasterDesc, &_rasterizer);
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Create Rasterizer 0x%x", result);
            break;
        }

        // テクスチャサンプラーステート
        D3D11_SAMPLER_DESC samplerDesc;
        memset(&samplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        result = device->CreateSamplerState(&samplerDesc, &_samplerState);
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Create Sampler 0x%x", result);
            break;
        }

        // ブレンドステートオブジェクト
        D3D11_BLEND_DESC blendDesc;
        memset(&blendDesc, 0, sizeof(blendDesc));
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;   // falseの場合はRenderTarget[0]しか使用しなくなる
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        result = device->CreateBlendState(&blendDesc, &_blendState);
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Create BlendState 0x%x", result);
            break;
        }

        UINT compileFlag = 0;

        result = D3DCompile(
            shaderCode,                         // メモリー内のシェーダーへのポインターです
            shaderSize,                         // メモリー内のシェーダーのサイズです
            NULL,                               // シェーダー コードが格納されているファイルの名前
            NULL,                               // マクロ定義の配列へのポインター
            NULL,                               // インクルード ファイルを扱うインターフェイスへのポインター
            "VertNormal",                       // シェーダーの実行が開始されるシェーダー エントリポイント関数の名前
            "vs_4_0",                           // シェーダー モデルを指定する文字列。
            compileFlag,                        // シェーダーコンパイルフラグ
            0,                                  // シェーダーコンパイルフラグ
            &vertexBlob,
            &vertexError);                      // エラーが出る場合はここで
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Compile Vertex Shader");
            break;
        }
        result = device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &_vertexShader);
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Create Vertex Shader");
            break;
        }

        result = D3DCompile(
            shaderCode,                         // メモリー内のシェーダーへのポインターです
            shaderSize,                         // メモリー内のシェーダーのサイズです
            NULL,                               // シェーダー コードが格納されているファイルの名前
            NULL,                               // マクロ定義の配列へのポインター
            NULL,                               // インクルード ファイルを扱うインターフェイスへのポインター
            "PixelNormal",                      // シェーダーの実行が開始されるシェーダー エントリポイント関数の名前
            "ps_4_0",                           // シェーダー モデルを指定する文字列
            compileFlag,                        // シェーダーコンパイルフラグ
            0,                                  // シェーダーコンパイルフラグ
            &pixelBlob,
            &pixelError);                       // エラーが出る場合はここで
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Compile Pixel Shader");
            break;
        }

        result = device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &_pixelShader);
        if (FAILED(result))
        {
            LAppPal::PrintLog("Fail Create Pixel Shader");
            break;
        }

        // この描画で使用する頂点フォーマット
        D3D11_INPUT_ELEMENT_DESC elems[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        result = device->CreateInputLayout(elems, ARRAYSIZE(elems), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &_vertexFormat);

        if (FAILED(result))
        {
            LAppPal::PrintLog("CreateVertexDeclaration failed");
            break;
        }

    } while (0);

    if(pixelError)
    {
        pixelError->Release();
        pixelError = NULL;
    }
    if (vertexError)
    {
        vertexError->Release();
        vertexError = NULL;
    }

    // blobはもうここで不要
    if (pixelBlob)
    {
        pixelBlob->Release();
        pixelBlob = NULL;
    }
    if (vertexBlob)
    {
        vertexBlob->Release();
        vertexBlob = NULL;
    }

    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void CubismSpriteShader::SetupShader()
{
    ID3D11DeviceContext* deviceContext = CubismDirectXRenderer::GetInstance()->GetD3dContext();

    if (_rasterizer == NULL || _samplerState == NULL || _blendState == NULL ||
        _vertexShader == NULL || _pixelShader == NULL || _vertexFormat == NULL)
    {
        return;
    }

    // 現在のウィンドウサイズの取得
    int windowWidth, windowHeight;
    CubismDirectXRenderer::GetInstance()->GetClientSize(windowWidth, windowHeight);

    // スプライト描画用の設定をし、シェーダーセット
    deviceContext->IASetInputLayout(_vertexFormat);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetInputLayout(_vertexFormat);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<FLOAT>(windowWidth);
    viewport.Height = static_cast<FLOAT>(windowHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->RSSetState(_rasterizer);

    deviceContext->VSSetShader(_vertexShader, NULL, 0);

    deviceContext->PSSetShader(_pixelShader, NULL, 0);
    deviceContext->PSSetSamplers(0, 1, &_samplerState);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    deviceContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);
}

void CubismSpriteShader::ReleaseShader()
{
    if (_rasterizer)
    {
        _rasterizer->Release();
        _rasterizer = NULL;
    }
    if(_samplerState)
    {
        _samplerState->Release();
        _samplerState = NULL;
    }
    if(_blendState)
    {
        _blendState->Release();
        _blendState = NULL;
    }
    if (_vertexShader)
    {
        _vertexShader->Release();
        _vertexShader = NULL;
    }
    if (_pixelShader)
    {
        _pixelShader->Release();
        _pixelShader = NULL;
    }
    if(_vertexFormat)
    {
        _vertexFormat->Release();
        _vertexFormat = NULL;
    }
}
