/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"

#include <d3dcompiler.h>
#include <Rendering/D3D11/CubismNativeInclude_D3D11.hpp>

#include "CubismDirectXRenderer.hpp"
#include "CubismDirectXView.hpp"
#include "CubismTextureManager.hpp"

namespace {
    CubismDirectXRenderer* _instance = nullptr;
}

CubismDirectXRenderer* CubismDirectXRenderer::GetInstance()
{
    if (!_instance)
    {
        _instance = new CubismDirectXRenderer();
    }

    return  _instance;
}

CubismDirectXRenderer::CubismDirectXRenderer()
    : _isEnd(false)
    , _textureManager(nullptr)
    , _windowHandle(nullptr)
    , _device(nullptr)
    , _deviceContext(nullptr)
    , _swapChain(nullptr)
    , _deviceStep(DeviceStep_None)
    , _renderTargetView(nullptr)
    , _depthTexture(nullptr)
    , _depthStencilView(nullptr)
    , _depthState(nullptr)
    , _rasterizer(nullptr)
    , _samplerState(nullptr)
    , _vertexShader(nullptr)
    , _pixelShader(nullptr)
    , _blendState(nullptr)
    , _vertexFormat(nullptr)
{
}

CubismDirectXRenderer::~CubismDirectXRenderer()
{
    Release();
}

void CubismDirectXRenderer::Release()
{
    if (_samplerState)
    {
        _samplerState->Release();
        _samplerState = nullptr;
    }
    if (_rasterizer)
    {
        _rasterizer->Release();
        _rasterizer = nullptr;
    }

    if (_renderTargetView)
    {
        _renderTargetView->Release();
        _renderTargetView = nullptr;
    }
    if (_depthState)
    {
        _depthState->Release();
        _depthState = nullptr;
    }
    if (_depthStencilView)
    {
        _depthStencilView->Release();
        _depthStencilView = nullptr;
    }
    if (_depthTexture)
    {
        _depthTexture->Release();
        _depthTexture = nullptr;
    }

    if (_swapChain)
    {
        _swapChain->Release();
        _swapChain = nullptr;
    }
    if (_deviceContext)
    {
        _deviceContext->Release();
        _deviceContext = nullptr;
    }

    if (_device)
    {
        _device->Release();
        _device = nullptr;
    }

    //ウィンドウの削除
    UnregisterClass(ClassName, _windowClass.hInstance);
}

void CubismDirectXRenderer::ReleaseInstance()
{
    if (_instance)
    {
        delete _instance;
    }

    _instance = nullptr;
}


ID3D11Device* CubismDirectXRenderer::GetD3dDevice()
{
    if (_instance == nullptr)
    {
        return nullptr;
    }
    return _instance->_device;
}

ID3D11DeviceContext* CubismDirectXRenderer::GetD3dContext()
{
    if (_instance == nullptr)
    {
        return nullptr;
    }
    return _instance->_deviceContext;
}

void CubismDirectXRenderer::GetWindowRect(RECT& rect)
{
    GetClientRect(_instance->_windowHandle, &rect);
}

void CubismDirectXRenderer::GetClientSize(int& rWidth, int& rHeight)
{
    RECT clientRect;
    GetClientRect(_instance->_windowHandle, &clientRect);

    rWidth = (clientRect.right - clientRect.left);
    rHeight = (clientRect.bottom - clientRect.top);
}

bool CubismDirectXRenderer::CreateRenderTarget(UINT width, UINT height)
{
    if (!_instance->_swapChain || !_instance->_device)
    {// SwapChain、Deviceが無い場合は無理
        return false;
    }

    HRESULT hr;
    // レンダーターゲットビュー作成
    ID3D11Texture2D* backBuffer;
    hr = _instance->_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        LAppPal::PrintLog("Fail SwapChain GetBuffer 0x%x", hr);
        return false;
    }
    hr = _instance->_device->CreateRenderTargetView(backBuffer, NULL, &_instance->_renderTargetView);
    // Getした分はRelease
    backBuffer->Release();
    if (FAILED(hr))
    {
        LAppPal::PrintLog("Fail CreateRenderTargetView 0x%x", hr);
        return false;
    }

    // Depth/Stencil
    D3D11_TEXTURE2D_DESC depthDesc;
    memset(&depthDesc, 0, sizeof(depthDesc));
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;   // format
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    hr = _instance->_device->CreateTexture2D(&depthDesc, NULL, &_instance->_depthTexture);
    if (FAILED(hr))
    {
        LAppPal::PrintLog("Fail Create DepthTarget 0x%x", hr);
        return false;
    }
    // DepthView
    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
    memset(&depthViewDesc, 0, sizeof(depthViewDesc));
    depthViewDesc.Format = depthDesc.Format;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Texture2D.MipSlice = 0;
    hr = _instance->_device->CreateDepthStencilView(_instance->_depthTexture, &depthViewDesc, &_instance->_depthStencilView);
    if (FAILED(hr))
    {
        LAppPal::PrintLog("Fail Create DepthTargetView 0x%x", hr);
        return false;
    }

    return true;
}

bool CubismDirectXRenderer::CreateShader()
{
    // 一旦削除する
    ReleaseShader();

    // スプライト描画用シェーダ
    static const csmChar* SpriteShaderEffectSrc =
        "cbuffer ConstantBuffer {"\
        "float4x4 projectMatrix;"\
        "float4x4 clipMatrix;"\
        "float4 baseColor;"\
        "float4 channelFlag;"\
        "}"\
        \
        "Texture2D mainTexture : register(t0);"\
        "SamplerState mainSampler : register(s0);"\
        "struct VS_IN {"\
        "float2 pos : POSITION0;"\
        "float2 uv : TEXCOORD0;"\
        "};"\
        "struct VS_OUT {"\
        "float4 Position : SV_POSITION;"\
        "float2 uv : TEXCOORD0;"\
        "float4 clipPosition : TEXCOORD1;"\
        "};"\
        \
        "/* Vertex Shader */"\
        "/* normal */"\
        "VS_OUT VertNormal(VS_IN In) {"\
        "VS_OUT Out = (VS_OUT)0;"\
        "Out.Position = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);"\
        "Out.uv.x = In.uv.x;"\
        "Out.uv.y = 1.0 - +In.uv.y;"\
        "return Out;"\
        "}"\
        \
        "/* Pixel Shader */"\
        "/* normal */"\
        "float4 PixelNormal(VS_OUT In) : SV_Target {"\
        "float4 color = mainTexture.Sample(mainSampler, In.uv) * baseColor;"\
        "return color;"\
        "}";

    ID3DBlob* vertexError = nullptr;
    ID3DBlob* pixelError = nullptr;

    ID3DBlob* vertexBlob = nullptr;   ///< スプライト描画用シェーダ
    ID3DBlob* pixelBlob = nullptr;     ///< スプライト描画用シェーダ

    HRESULT hr = S_OK;
    do
    {
        UINT compileFlag = 0;

        hr = D3DCompile(
            SpriteShaderEffectSrc,              // メモリー内のシェーダーへのポインターです
            strlen(SpriteShaderEffectSrc),      // メモリー内のシェーダーのサイズです
            nullptr,                               // シェーダー コードが格納されているファイルの名前
            nullptr,                               // マクロ定義の配列へのポインター
            nullptr,                               // インクルード ファイルを扱うインターフェイスへのポインター
            "VertNormal",                       // シェーダーの実行が開始されるシェーダー エントリポイント関数の名前
            "vs_4_0",                           // シェーダー モデルを指定する文字列。
            compileFlag,                        // シェーダーコンパイルフラグ
            0,                                  // シェーダーコンパイルフラグ
            &vertexBlob,
            &vertexError);                              // エラーが出る場合はここで
        if (FAILED(hr))
        {
            LAppPal::PrintLog("Fail Compile Vertex Shader");
            break;
        }
        hr = _device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &_vertexShader);
        if (FAILED(hr))
        {
            LAppPal::PrintLog("Fail Create Vertex Shader");
            break;
        }

        hr = D3DCompile(
            SpriteShaderEffectSrc,              // メモリー内のシェーダーへのポインターです
            strlen(SpriteShaderEffectSrc),      // メモリー内のシェーダーのサイズです
            nullptr,                               // シェーダー コードが格納されているファイルの名前
            nullptr,                               // マクロ定義の配列へのポインター
            nullptr,                               // インクルード ファイルを扱うインターフェイスへのポインター
            "PixelNormal",                      // シェーダーの実行が開始されるシェーダー エントリポイント関数の名前
            "ps_4_0",                           // シェーダー モデルを指定する文字列
            compileFlag,                        // シェーダーコンパイルフラグ
            0,                                  // シェーダーコンパイルフラグ
            &pixelBlob,
            &pixelError);                       // エラーが出る場合はここで
        if (FAILED(hr))
        {
            LAppPal::PrintLog("Fail Compile Pixel Shader");
            break;
        }

        hr = _device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &_pixelShader);
        if (FAILED(hr))
        {
            LAppPal::PrintLog("Fail Create Pixel Shader");
            break;
        }

        // この描画で使用する頂点フォーマット
        D3D11_INPUT_ELEMENT_DESC elems[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        hr = _device->CreateInputLayout(elems, ARRAYSIZE(elems), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &_vertexFormat);

        if (FAILED(hr))
        {
            LAppPal::PrintLog("CreateVertexDeclaration failed");
            break;
        }

    } while (0);

    if (pixelError)
    {
        pixelError->Release();
        pixelError = nullptr;
    }
    if (vertexError)
    {
        vertexError->Release();
        vertexError = nullptr;
    }

    // blobはもうここで不要
    if (pixelBlob)
    {
        pixelBlob->Release();
        pixelBlob = nullptr;
    }
    if (vertexBlob)
    {
        vertexBlob->Release();
        vertexBlob = nullptr;
    }

    if (FAILED(hr))
    {
        return false;
    }

    // レンダリングステートオブジェクト
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

    _device->CreateBlendState(&blendDesc, &_blendState);

    return true;
}

void CubismDirectXRenderer::SetupShader()
{
    if (!_device || !_vertexFormat || !_vertexShader || !_pixelShader)
    {
        return;
    }

    // 現在のウィンドウサイズ
    int windowWidth, windowHeight;
    GetClientSize(windowWidth, windowHeight);

    // スプライト描画用の設定をし、シェーダセット
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    _deviceContext->OMSetBlendState(_blendState, blendFactor, 0xffffffff);

    _deviceContext->IASetInputLayout(_vertexFormat);
    _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _deviceContext->IASetInputLayout(_vertexFormat);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<FLOAT>(windowWidth);
    viewport.Height = static_cast<FLOAT>(windowHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    _deviceContext->RSSetViewports(1, &viewport);
    _deviceContext->RSSetState(_rasterizer);

    _deviceContext->VSSetShader(_vertexShader, nullptr, 0);

    _deviceContext->PSSetShader(_pixelShader, nullptr, 0);
    _deviceContext->PSSetSamplers(0, 1, &_samplerState);
}

void CubismDirectXRenderer::ReleaseShader()
{
    if (_blendState)
    {
        _blendState->Release();
        _blendState = nullptr;
    }
    if (_vertexFormat)
    {
        _vertexFormat->Release();
        _vertexFormat = nullptr;
    }
    if (_pixelShader)
    {
        _pixelShader->Release();
        _pixelShader = nullptr;
    }
    if (_vertexShader)
    {
        _vertexShader->Release();
        _vertexShader = nullptr;
    }
}

void CubismDirectXRenderer::ResizeDevice(CubismUserModel* userModel)
{
    // 今のクライアント領域の大きさに合わせます
    int nowWidth, nowHeight;
    GetClientSize(nowWidth, nowHeight);

    // デバイス設定
    if (nowWidth == 0 || nowHeight == 0)// サイズが0の際は最小化されていると思われる
    {
        // NOP サイズが戻ったら再チャレンジ
    }
    else
    {
        // ターゲットがあるなら削除
        if (_renderTargetView)
        {
            _renderTargetView->Release();
            _renderTargetView = nullptr;
        }
        if (_depthStencilView)
        {
            _depthStencilView->Release();
            _depthStencilView = nullptr;
        }
        if (_depthTexture)
        {
            _depthTexture->Release();
            _depthTexture = nullptr;
        }

        _presentParameters.BufferDesc.Width = nowWidth;
        _presentParameters.BufferDesc.Height = nowHeight;

        if (_swapChain)
        {
            // サイズを変更
            HRESULT hr = _swapChain->ResizeBuffers(
                _presentParameters.BufferCount,
                static_cast<UINT>(nowWidth),
                static_cast<UINT>(nowHeight),
                DXGI_FORMAT_R8G8B8A8_UNORM,
                0
            );

            // ResizeBuffersでもロストチェックをした方が良いとMSDNにある
            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {// デバイスがロスト
                _deviceStep = DeviceStep_Lost;
            }
            else
            {
                // RenderTargetView/DepthStencilViewの再作成
                if (CreateRenderTarget(static_cast<UINT>(nowWidth), static_cast<UINT>(nowHeight)))
                {
                    if (CubismDirectXView::GetInstance())
                    {
                        // パラメータ、スプライトサイズなど再設定
                        CubismDirectXView::GetInstance()->Initialize();
                        CubismDirectXView::GetInstance()->ResizeSprite();
                        CubismDirectXView::GetInstance()->DestroyOffscreenFrame();
                    }

                    static_cast<CubismUserModelExtend*>(userModel)->GetRenderBuffer().DestroyOffscreenFrame();

                    // 通常に戻る
                    _deviceStep = DeviceStep_None;
                }
            }
        }
    }
}

void CubismDirectXRenderer::StartFrame() const
{
    /*
    アプリのフレーム先頭処理 他の描画物がある体での各種設定、
    レンダーターゲットクリアなど
    */

    // デバイス未設定
    if (!_device || !_deviceContext)
    {
        return;
    }

    // バックバッファのクリア
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
    _deviceContext->ClearRenderTargetView(_renderTargetView, clearColor);
    _deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Z無効
    _deviceContext->OMSetDepthStencilState(_depthState, 0);
}

void CubismDirectXRenderer::EndFrame(CubismUserModel* userModel)
{
    // 画面反映
    HRESULT hr = _swapChain->Present(1, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {// デバイスロストチェック
        _deviceStep = DeviceStep_Lost;
    }

    // ウィンドウサイズ変更対応
    if (_deviceStep == DeviceStep_Size)
    {
        ResizeDevice(userModel);
    }

    if (_deviceStep == DeviceStep_Lost)
    {// ロストした
        LAppPal::PrintLog("Device Lost Abort");
        // アプリケーション終了フラグを立てる
        _isEnd = true;
    }
}
