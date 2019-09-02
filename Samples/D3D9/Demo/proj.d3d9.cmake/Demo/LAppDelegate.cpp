/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppDelegate.hpp"

#include <Rendering/D3D9/CubismRenderer_D3D9.hpp>
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
    LAppDelegate* s_instance = NULL;

    const LPCSTR ClassName = "Cubism DirectX9 Sample";
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
        LAppPal::PrintLog("START");
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
        LAppPal::PrintLog("Fail Create Window");
        return false;
    }

    //ウインドウ表示
    ShowWindow(_windowHandle, SW_SHOWDEFAULT);
    UpdateWindow(_windowHandle);

    // Direct3D9 init
    if ((_direct3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    {
        LAppPal::PrintLog("Fail Direct3D Initialize");
        return false;
    }

    // デフォルトバックバッファ作成枚数
    const csmInt32 BackBufferNum = 1;

    // デバイス設定
    memset(&_presentParameters, 0, sizeof(_presentParameters));
    _presentParameters.Windowed = TRUE;                          // ウインドウモード
    _presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;       // バックバッファのスワップエフェクト Direct3Dにスワップエフェクトをまかせる
    _presentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;       // バックバッファのフォーマット D3DFMT_UNKNOWNだと今表示されているモニタの設定と同じ
    _presentParameters.BackBufferCount = BackBufferNum;          // バックバッファ数
    _presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;    // マルチサンプリング無し
    _presentParameters.MultiSampleQuality = 0;                   // マルチサンプリング無し
    _presentParameters.EnableAutoDepthStencil = TRUE;            // ZはD3d任せ
    _presentParameters.AutoDepthStencilFormat = D3DFMT_D16;      // Z精度はushort
    _presentParameters.hDeviceWindow = _windowHandle;
    _presentParameters.Flags = 0;
    _presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // 今のリフレッシュレートをそのまま使う
    _presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;   // モニタの垂直回帰を待つ

    HRESULT createResult =_direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _windowHandle,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &_presentParameters,
        &_device);

    if(FAILED(createResult))
    {
        LAppPal::PrintLog("Fail Direct3D Create Device %x", createResult);
        return false;
    }

    // メンバーコピー
    _presentParametersFull = _presentParameters;
    _presentParametersFull.Windowed = FALSE; // 違いはこれとサイズ
    // フルスクリーン化した場合の最適解像度をチェック
    CheckFullScreen(D3DFMT_X8R8G8B8, RenderTargetWidth, RenderTargetHeight);

    //AppViewの初期化
    _view->Initialize();

    // Cubism SDK の初期化
    InitializeCubism();

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

    if(_device)
    {
        _device->Release();
        _device = NULL;
    }
    if(_direct3D)
    {
        _direct3D->Release();
        _direct3D = NULL;
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
    , _windowHandle(NULL)
    , _direct3D(NULL)
    , _device(NULL)
    , _deviceLostStep(LostStep::LostStep_None)
    , _lostCounter(0)
    , _isFullScreen(false)
    , _shaderEffect(NULL)
    , _vertexFormat(NULL)
{
    _textureManager = new LAppTextureManager();
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
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    // モデルロード前に必ず呼び出す必要がある
    Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D9::InitializeConstantSettings(1, _device);

    //load model
    LAppLive2DManager::GetInstance();

    LAppPal::UpdateTime();

    _view->InitializeSprite();
}

bool LAppDelegate::CreateShader()
{
    ReleaseShader();

    static const csmChar* SpriteShaderEffectSrc =
        "float4x4 projectMatrix;"\
        "float4 baseColor;"\
        "texture mainTexture;"\
        \
        "sampler mainSampler = sampler_state{"\
            "texture = <mainTexture>;"\
        "};"\
        \
        "struct VS_IN {"\
            "float2 pos : POSITION;"\
            "float2 uv : TEXCOORD0;"\
        "};"\
        "struct VS_OUT {"\
            "float4 Position : POSITION0;"\
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
        "float4 PixelNormal(VS_OUT In) : COLOR0{"\
            "float4 color = tex2D(mainSampler, In.uv) * baseColor;"\
            "return color;"\
        "}"\
        \
    "/* Technique */"\
        "technique ShaderNames_Normal {"\
            "pass p0{"\
                "VertexShader = compile vs_2_0 VertNormal();"\
                "PixelShader = compile ps_2_0 PixelNormal();"\
            "}"\
        "}";

    ID3DXBuffer* error = 0;
    if (FAILED(D3DXCreateEffect(_device, SpriteShaderEffectSrc, static_cast<UINT>(strlen(SpriteShaderEffectSrc)), 0, 0, 0, 0, &_shaderEffect, &error)))
    {
        LAppPal::PrintLog("Cannot load the shaders");
        return false;
    }

    // この描画で使用する頂点フォーマット
    D3DVERTEXELEMENT9 elems[] = {
        { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, sizeof(float) * 2, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };
    if (_device->CreateVertexDeclaration(elems, &_vertexFormat))
    {
        LAppPal::PrintLog("CreateVertexDeclaration failed");
        CSM_ASSERT(0);
    }

    return true;
}

ID3DXEffect* LAppDelegate::SetupShader()
{
    if(_device==NULL || _vertexFormat==NULL || _shaderEffect==NULL)
    {
        return NULL;
    }

    _device->SetVertexDeclaration(_vertexFormat);
    return _shaderEffect;
}

void LAppDelegate::ReleaseShader()
{
    if(_vertexFormat)
    {
        _vertexFormat->Release();
        _vertexFormat = NULL;
    }
    if(_shaderEffect)
    {
        _shaderEffect->Release();
        _shaderEffect = NULL;
    }
}


void LAppDelegate::StartFrame()
{
    /*
    アプリのフレーム先頭処理 他の描画物がある体での各種設定、
    レンダーターゲットクリアなど
    */

    // デバイス未設定
    if (!_device)
    {
        return;
    }

    //バックバッファのクリア
    _device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    //シーンの開始
    if (SUCCEEDED(_device->BeginScene()))
    {
        // アルファ・ブレンディングを行う
        _device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        // α、1-α
        _device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        // 消す面を指定
        _device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    }

}

void LAppDelegate::EndFrame()
{
    // テクスチャの参照を外しておく
    if (_shaderEffect)
    {
        _shaderEffect->SetTexture("mainTexture", NULL);
        _shaderEffect->CommitChanges();
    }

    if (_device)
    {
        //シーンの終了
        _device->EndScene();
        //バックバッファを表画面に反映させる
        HRESULT result = _device->Present(NULL, NULL, NULL, NULL);

        if(result== D3DERR_DEVICELOST)
        {// ロスト中
            if(_deviceLostStep == LostStep::LostStep_None)
            {
                // Viewロスト処理
                _view->OnDeviceLost();
                // L2Dデバイスロスト処理(各モデルレンダラーの破棄)
                LAppLive2DManager::GetInstance()->OnDeviceLost(_device);
                // テクスチャ全開放
                _textureManager->ReleaseTextures();

                _lostCounter = 0;
            }
            _deviceLostStep = LostStep::LostStep_Lost;
        }

        if(_deviceLostStep == LostStep::LostStep_Lost)
        {
            result = _device->TestCooperativeLevel();
            if (result == D3DERR_DEVICENOTRESET)
            {
                if(s_instance->_isFullScreen && _lostCounter==0)
                {
                    result = _device->Reset(&_presentParametersFull);
                }
                else
                {
                    // 今の大きさに合わせます
                    int nowWidth, nowHeight;
                    GetClientSize(nowWidth, nowHeight);
                    _presentParameters.BackBufferWidth = nowWidth;
                    _presentParameters.BackBufferHeight = nowHeight;
                    if(nowWidth==0 || nowHeight==0)
                    {// サイズゼロは無理なので、初期値で
                        _presentParameters.BackBufferWidth = RenderTargetWidth;
                        _presentParameters.BackBufferHeight = RenderTargetHeight;
                    }

                    result = _device->Reset(&_presentParameters);
                }
                _lostCounter++;

                if (SUCCEEDED(result))
                {
                    // スプライト再作成
                    _view->InitializeSprite();
                    // レンダラー再生
                    LAppLive2DManager::GetInstance()->RestoreDeviceLost(_device);

                    _deviceLostStep = LostStep::LostStep_None;
                }
            }
        }
    }
    else
    {
        if (_deviceLostStep == LostStep::LostStep_ReCreate)
        {
            D3DPRESENT_PARAMETERS *presentParam = NULL;

            int nowWidth, nowHeight;
            if (s_instance->_isFullScreen && _lostCounter == 0)
            {
                nowWidth = _presentParametersFull.BackBufferWidth;
                nowHeight = _presentParametersFull.BackBufferHeight;

                presentParam = &_presentParametersFull;
            }
            else
            {
                // 今の大きさに合わせます
                GetClientSize(nowWidth, nowHeight);
                _presentParameters.BackBufferWidth = nowWidth;
                _presentParameters.BackBufferHeight = nowHeight;
                if (nowWidth == 0 || nowHeight == 0)
                {// サイズゼロは無理なので、初期値で
                    _presentParameters.BackBufferWidth = RenderTargetWidth;
                    _presentParameters.BackBufferHeight = RenderTargetHeight;
                }

                presentParam = &_presentParameters;
            }
            _lostCounter++;

            // デバイス設定
            if (nowWidth==0 || nowHeight==0)// サイズが0の際は最小化されていると思われる CreateDeviceは成功しない
            {
                // NOP サイズが戻ったら再チャレンジ
            }
            else if(FAILED(_direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _windowHandle,
                D3DCREATE_HARDWARE_VERTEXPROCESSING, presentParam,
                &_device)))
            {
                LAppPal::PrintLog("Fail Direct3D Create Device");
                _device = NULL;
            }
            else
            {
                // デバイスが変わったことを通知
                Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D9::InitializeConstantSettings(1, _device);

                // 描画のパラメータをウィンドウサイズに合わせて新設定
                _view->Initialize();

                // スプライト再作成
                _view->InitializeSprite();
                // レンダラー再生
                LAppLive2DManager::GetInstance()->RestoreDeviceLost(_device);

                _deviceLostStep = LostStep::LostStep_None;
            }
        }
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


void LAppDelegate::RecreateDevice()
{
    // Viewロスト処理
    _view->OnDeviceLost();
    // L2Dデバイスロスト処理(各モデルレンダラーの破棄)
    LAppLive2DManager::GetInstance()->OnDeviceLost(_device);
    // テクスチャ全開放
    _textureManager->ReleaseTextures();

    // ロスト時の処理に加えてさらにデバイスも再作成のため破棄
    if(_device)
    {
        _device->Release();
        _device = NULL;
    }

    // デバイス再作成の方へ行く
    // LostStep_Lostだったとしても上書き
    _deviceLostStep = LostStep::LostStep_ReCreate;
    _lostCounter = 0;
}

LRESULT WINAPI LAppDelegate::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:    // 終了
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:  // ウインドウ描画時
        ValidateRect(hWnd, NULL);
        return 0;

    case WM_SIZE:   // ウィンドウサイズ変更
        if(s_instance != NULL && s_instance->_view!=NULL && s_instance->_device)
        {// _device作成前にCreateWindowをやった時もここに来るので、_deviceのNullチェックは必須
            // デバイス再作成通知
            s_instance->RecreateDevice();
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

    case WM_SYSKEYDOWN:
        if (s_instance != NULL)
        {
            switch (wParam)
            {
            case VK_RETURN: // Alt+Enterでフルスクリーン切り替え
                {
                    // フラグを反転してデバイス再作成ステップへ
                    s_instance->_isFullScreen = !s_instance->_isFullScreen;
                    s_instance->RecreateDevice();
                }
                break;
            default:
                break;
            }
        }
        break;

    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LPDIRECT3DDEVICE9 LAppDelegate::GetD3dDevice()
{
    if (s_instance == NULL)
    {
        return NULL;
    }
    return s_instance->_device;
}

bool LAppDelegate::CheckFullScreen(D3DFORMAT format, UINT width, UINT height)
{
    UINT nearest = UINT_MAX;
    UINT minWidh=0, minHeight=0;

    // ディスプレイモード数
    D3DDISPLAYMODE d3dspMode;
    int modeNum = _direct3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, format);
    for (int i = 0; i < modeNum; i++)
    {
        if(FAILED(_direct3D->EnumAdapterModes(D3DADAPTER_DEFAULT, format, i, &d3dspMode)))
        {
            continue;
        }

        // 指定サイズに一番近いもの
        INT nearW = d3dspMode.Width - width;
        INT nearH = d3dspMode.Height - height;
        UINT sum = abs(nearW) + abs(nearH);
        if(nearest>sum) // 差分が小さい
        {
            nearest = sum;
            minWidh = d3dspMode.Width;
            minHeight = d3dspMode.Height;
        }
    }

    if(nearest != UINT_MAX)
    {// 調査成功
        _presentParametersFull.BackBufferWidth = minWidh;
        _presentParametersFull.BackBufferHeight = minHeight;
    }

    // 見つかっていたらtrueを返す
    return nearest != UINT_MAX;
}
