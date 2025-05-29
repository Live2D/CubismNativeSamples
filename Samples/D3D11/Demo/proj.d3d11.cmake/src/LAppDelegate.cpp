/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"

#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>

#include <tchar.h>
#include "LAppLive2DManager.hpp"
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppTextureManager.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace Csm;
using namespace LAppDefine;

namespace {
    LAppDelegate* s_instance = NULL;

    const LPCTSTR ClassName = _T("Cubism DirectX11 Sample");

    const csmInt32 BackBufferNum = 1; // バックバッファ枚数
}

LAppDelegate* LAppDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppDelegate();
    }

    return s_instance;
}

void LAppDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

bool LAppDelegate::Initialize()
{
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("START");
    }

    // ウィンドウクラス
    _windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ClassName, NULL };
    RegisterClassEx(&_windowClass);

    // タイトルバー、ウィンドウ枠の分サイズを増やす
    RECT rect;
    {
        SetRect(&rect, 0, 0, RenderTargetWidth, RenderTargetHeight);
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        rect.right = rect.right - rect.left;
        rect.bottom = rect.bottom - rect.top;
        rect.top = 0;
        rect.left = 0;
    }

    //ウインドウの生成
    _windowHandle = CreateWindow(ClassName, ClassName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom, NULL, NULL, _windowClass.hInstance, NULL);
    if(_windowHandle==NULL)
    {
        LAppPal::PrintLogLn("Fail Create Window");
        return false;
    }

    //ウインドウ表示
    ShowWindow(_windowHandle, SW_SHOWDEFAULT);
    UpdateWindow(_windowHandle);

    // 現在のウィンドウサイズ
    int windowWidth, windowHeight;
    GetClientSize(windowWidth, windowHeight);

    // デバイス設定
    memset(&_presentParameters, 0, sizeof(_presentParameters));
    _presentParameters.BufferCount = BackBufferNum;
    _presentParameters.BufferDesc.Width = windowWidth;
    _presentParameters.BufferDesc.Height = windowHeight;
    _presentParameters.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    _presentParameters.BufferDesc.RefreshRate.Numerator = 60;
    _presentParameters.BufferDesc.RefreshRate.Denominator = 1;
    _presentParameters.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    _presentParameters.SampleDesc.Count = 1;
    _presentParameters.SampleDesc.Quality = 0;
    _presentParameters.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    _presentParameters.Flags = 0;
    _presentParameters.Windowed = TRUE;
    _presentParameters.OutputWindow = _windowHandle;

    D3D_FEATURE_LEVEL level;
    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL, // D3D_DRIVER_TYPE_HARDWAREの場合はNULL固定
#ifdef CSM_DEBUG
        D3D11_CREATE_DEVICE_DEBUG,
#else
        0,  // 単一スレッドでの描画コマンド発行を保証する場合はこれでちょっと速度向上する → D3D11_CREATE_DEVICE_SINGLETHREADED
#endif
        NULL,   // NULLの場合勝手に{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1, }; と認識される
        0,      // ↑の要素数
        D3D11_SDK_VERSION,
        &_presentParameters,
        &_swapChain,
        &_device,
        &level,
        &_deviceContext);
    if (FAILED(result))
    {
        LAppPal::PrintLogLn("Fail Direct3D Create Device 0x%x", result);
        return false;
    }

    // RenderTargetView/DepthStencilViewの作成
    CreateRenderTarget(static_cast<UINT>(LAppDefine::RenderTargetWidth), static_cast<UINT>(LAppDefine::RenderTargetHeight));

    // Z無効深度オブジェクト
    D3D11_DEPTH_STENCIL_DESC depthDesc;
    memset(&depthDesc, 0, sizeof(depthDesc));
    depthDesc.DepthEnable = false;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDesc.StencilEnable = false;
    result = _device->CreateDepthStencilState(&depthDesc, &_depthState);
    if (FAILED(result))
    {
        LAppPal::PrintLogLn("Fail Create Depth 0x%x", result);
        return false;
    }

    // デバイス作成の後
    _textureManager = new LAppTextureManager();

    //AppViewの初期化
    int clientWidth, clientHeight;
    GetClientSize(clientWidth, clientHeight);
    _view->Initialize(clientWidth, clientHeight);

    // Cubism SDK の初期化
    InitializeCubism();

    return true;
}

bool LAppDelegate::CreateRenderTarget(UINT width, UINT height)
{
    if(!_swapChain || !_device)
    {// SwapChain、Deviceが無い場合は無理
        return false;
    }

    HRESULT hr;
    // レンダーターゲットビュー作成
    ID3D11Texture2D* backBuffer;
    hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        LAppPal::PrintLogLn("Fail SwapChain GetBuffer 0x%x", hr);
        return false;
    }
    hr = _device->CreateRenderTargetView(backBuffer, NULL, &_renderTargetView);
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
    hr = _device->CreateTexture2D(&depthDesc, NULL, &_depthTexture);
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
    hr = _device->CreateDepthStencilView(_depthTexture, &depthViewDesc, &_depthStencilView);
    if (FAILED(hr))
    {
        LAppPal::PrintLogLn("Fail Create DepthTargetView 0x%x", hr);
        return false;
    }

    return true;
}

void LAppDelegate::Release()
{
    _view->ReleaseSprite();

    // リソースを解放
    LAppLive2DManager::ReleaseInstance();

    delete _view;
    delete _textureManager;
    _view = NULL;
    _textureManager = NULL;

    if (_renderTargetView)
    {
        _renderTargetView->Release();
        _renderTargetView = NULL;
    }
    if(_depthState)
    {
        _depthState->Release();
        _depthState = NULL;
    }
    if(_depthStencilView)
    {
        _depthStencilView->Release();
        _depthStencilView = NULL;
    }
    if (_depthTexture)
    {
        _depthTexture->Release();
        _depthTexture = NULL;
    }

    if (_swapChain)
    {
        _swapChain->Release();
        _swapChain = NULL;
    }
    if (_deviceContext)
    {
        _deviceContext->Release();
        _deviceContext = NULL;
    }

    if (_device)
    {
        _device->Release();
        _device = NULL;
    }

    UnregisterClass(ClassName, _windowClass.hInstance);
}

void LAppDelegate::Run()
{
    MSG msg;

    do
    {
        //メッセージループ
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 時間更新
            LAppPal::UpdateTime();

            // 画面クリアなど
            StartFrame();

            // 描画
            _view->Render();

            // フレーム末端処理
            EndFrame();

            // アプリケーション終了メッセージでウィンドウを破棄する
            if (GetIsEnd() && _windowHandle!=NULL)
            {// ウィンドウ破壊
                DestroyWindow(_windowHandle);
                _windowHandle = NULL;
            }
        }
    } while (msg.message != WM_QUIT);

    // 解放
    Release();
    // インスタンス削除
    ReleaseInstance();
}

LAppDelegate::LAppDelegate()
    : _captured(false)
    , _mouseX(0.0f)
    , _mouseY(0.0f)
    , _isEnd(false)
    , _textureManager(NULL)
    , _windowHandle(NULL)
    , _device(NULL)
    , _deviceContext(NULL)
    , _swapChain(NULL)
    , _deviceStep(DeviceStep_None)
    , _renderTargetView(NULL)
    , _depthTexture(NULL)
    , _depthStencilView(NULL)
    , _depthState(NULL)
{
    _view = new LAppView();
}

LAppDelegate::~LAppDelegate()
{
}

void LAppDelegate::InitializeCubism()
{
    //setup cubism
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    // モデルロード前に必ず呼び出す必要がある
    Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D11::InitializeConstantSettings(BackBufferNum, _device);

    //load model
    LAppLive2DManager::GetInstance();

    LAppPal::UpdateTime();

    _view->InitializeSprite();
}

void LAppDelegate::StartFrame()
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

void LAppDelegate::EndFrame()
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
        ResizeDevice();
    }

    if(_deviceStep == DeviceStep_Lost)
    {// ロストした
        LAppPal::PrintLogLn("Device Lost Abort");
        // アプリケーション終了
        AppEnd();
    }

    // 遅延開放監視
    LAppLive2DManager::GetInstance()->EndFrame();
}

void LAppDelegate::GetWindowRect(RECT& rect)
{
    if (!s_instance)
    {
        return;
    }

    GetClientRect(s_instance->_windowHandle, &rect);
}

void LAppDelegate::GetClientSize(int& rWidth, int& rHeight)
{
    if (!s_instance)
    {
        return;
    }

    RECT clientRect;
    GetClientRect(s_instance->_windowHandle, &clientRect);

    rWidth = (clientRect.right - clientRect.left);
    rHeight = (clientRect.bottom - clientRect.top);
}

void LAppDelegate::ResizeDevice()
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
            _renderTargetView = NULL;
        }
        if (_depthStencilView)
        {
            _depthStencilView->Release();
            _depthStencilView = NULL;
        }
        if (_depthTexture)
        {
            _depthTexture->Release();
            _depthTexture = NULL;
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
                    if (_view)
                    {
                        // パラメータ、スプライトサイズなど再設定
                        int clientWidth, clientHeight;
                        GetClientSize(clientWidth, clientHeight);
                        _view->Initialize(clientWidth, clientHeight);
                        _view->ResizeSprite();
                        _view->DestroyOffscreenSurface();
                    }

                    // マネージャにサイズ変更通知
                    LAppLive2DManager::GetInstance()->ResizedWindow();

                    // 通常に戻る
                    _deviceStep = DeviceStep_None;
                }
            }
        }
    }
}

LRESULT WINAPI LAppDelegate::MsgProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:    // 終了
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:  // ウインドウ描画時
        ValidateRect(wnd, NULL);
        return 0;

    case WM_SIZE:   // ウィンドウサイズ変更
        if(s_instance != NULL && s_instance->_view!=NULL && s_instance->_device && s_instance->_deviceStep!=DeviceStep_Lost)
        {// _device作成前にCreateWindowをやった時もここに来るので、_deviceのNullチェックは必須
            // Resize指示を出す
            s_instance->_deviceStep = DeviceStep_Size;
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (s_instance != NULL)
        {
            if (s_instance->_view == NULL)
            {
                return 0;
            }

            s_instance->_mouseX = static_cast<float>(LOWORD(lParam));
            s_instance->_mouseY = static_cast<float>(HIWORD(lParam));

            {
                s_instance->_captured = true;
                s_instance->_view->OnTouchesBegan(s_instance->_mouseX, s_instance->_mouseY);
            }
        }
        return 0;

    case WM_LBUTTONUP:
        if (s_instance != NULL)
        {
            if (s_instance->_view == NULL)
            {
                return 0;
            }

            s_instance->_mouseX = static_cast<float>(LOWORD(lParam));
            s_instance->_mouseY = static_cast<float>(HIWORD(lParam));

            {
                if (s_instance->_captured)
                {
                    s_instance->_captured = false;
                    s_instance->_view->OnTouchesEnded(s_instance->_mouseX, s_instance->_mouseY);
                }
            }
        }
        return 0;

    case WM_MOUSEMOVE:
        if(s_instance!=NULL)
        {
            s_instance->_mouseX = static_cast<float>(LOWORD(lParam));
            s_instance->_mouseY = static_cast<float>(HIWORD(lParam));

            if (!s_instance->_captured)
            {
                return 0;
            }
            if (s_instance->_view == NULL)
            {
                return 0;
            }

            s_instance->_view->OnTouchesMoved(s_instance->_mouseX, s_instance->_mouseY);
        }
        return 0;

    default:
        break;
    }
    return DefWindowProc(wnd, msg, wParam, lParam);
}

ID3D11Device* LAppDelegate::GetD3dDevice()
{
    if (s_instance == NULL)
    {
        return NULL;
    }
    return s_instance->_device;
}

ID3D11DeviceContext* LAppDelegate::GetD3dContext()
{
    if (s_instance == NULL)
    {
        return NULL;
    }
    return s_instance->_deviceContext;
}
