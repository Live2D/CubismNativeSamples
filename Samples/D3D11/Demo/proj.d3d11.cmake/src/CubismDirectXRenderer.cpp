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
{
}

CubismDirectXRenderer::~CubismDirectXRenderer()
{
    Release();
}

void CubismDirectXRenderer::Release()
{
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
        LAppPal::PrintLogLn("Fail SwapChain GetBuffer 0x%x", hr);
        return false;
    }
    hr = _instance->_device->CreateRenderTargetView(backBuffer, NULL, &_instance->_renderTargetView);
    // Getした分はRelease
    backBuffer->Release();
    if (FAILED(hr))
    {
        LAppPal::PrintLogLn("Fail CreateRenderTargetView 0x%x", hr);
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
        LAppPal::PrintLogLn("Fail Create DepthTarget 0x%x", hr);
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
        LAppPal::PrintLogLn("Fail Create DepthTargetView 0x%x", hr);
        return false;
    }

    return true;
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
                        CubismDirectXView::GetInstance()->Initialize(nowWidth, nowHeight);
                        CubismDirectXView::GetInstance()->ResizeSprite();
                        CubismDirectXView::GetInstance()->DestroyOffscreenSurface();
                    }

                    static_cast<CubismUserModelExtend*>(userModel)->GetRenderBuffer().DestroyOffscreenSurface();

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
        LAppPal::PrintLogLn("Device Lost Abort");
        // アプリケーション終了フラグを立てる
        _isEnd = true;
    }
}
