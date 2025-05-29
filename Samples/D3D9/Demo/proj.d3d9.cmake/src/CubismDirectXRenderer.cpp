/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"

#include "CubismDirectXRenderer.hpp"
#include "CubismDirectXView.hpp"
#include "CubismTextureManager.hpp"
#include "Rendering/D3D9/CubismRenderer_D3D9.hpp"

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
    , _direct3D(nullptr)
    , _device(nullptr)
    , _deviceLostStep(LostStep::LostStep_None)
    , _lostCounter(0)
    , _isFullScreen(false)
    , _model(nullptr)
{
    _textureManager = new CubismTextureManager();
}

CubismDirectXRenderer::~CubismDirectXRenderer()
{
    Release();
}

void CubismDirectXRenderer::Release()
{
    delete _textureManager;
    _textureManager = nullptr;

    if (_device)
    {
        _device->Release();
        _device = nullptr;
    }

    //ウィンドウの削除
    UnregisterClass(ClassName, _windowClass.hInstance);
}

void CubismDirectXRenderer::RestoreDeviceLost(LPDIRECT3DDEVICE9 device)
{
    _model->ReloadRenderer();
}

void CubismDirectXRenderer::OnDeviceLost()
{
    _model->OnDeviceLost();

    // シェーダ・頂点宣言解放等
    Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D9::OnDeviceLost();
}

void CubismDirectXRenderer::ReleaseInstance()
{
    if (_instance)
    {
        delete _instance;
    }

    _instance = nullptr;
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

void CubismDirectXRenderer::RecreateDevice()
{
    // Viewロスト処理
    CubismDirectXView::GetInstance()->OnDeviceLost();
    // デバイスロスト処理(各モデルレンダラーの破棄)
    OnDeviceLost();
    // テクスチャ全開放
    _textureManager->ReleaseTextures();

    // ロスト時の処理に加えてさらにデバイスも再作成のため破棄
    if (_device)
    {
        _device->Release();
        _device = NULL;
    }

    // デバイス再作成の方へ行く
    // LostStep_Lostだったとしても上書き
    _deviceLostStep = LostStep::LostStep_ReCreate;
    _lostCounter = 0;
}

void CubismDirectXRenderer::StartFrame() const
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

void CubismDirectXRenderer::EndFrame(CubismUserModel* userModel)
{
    if (_device)
    {
        //シーンの終了
        _device->EndScene();
        //バックバッファを表画面に反映させる
        HRESULT result = _device->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST)
        {// ロスト中
            if (_deviceLostStep == LostStep::LostStep_None)
            {
                // Viewロスト処理
                CubismDirectXView::GetInstance()->OnDeviceLost();
                // デバイスロスト処理(各モデルレンダラーの破棄)
                OnDeviceLost();
                // テクスチャ全開放
                _textureManager->ReleaseTextures();

                _lostCounter = 0;
            }
            _deviceLostStep = LostStep::LostStep_Lost;
        }

        if (_deviceLostStep == LostStep::LostStep_Lost)
        {
            result = _device->TestCooperativeLevel();
            if (result == D3DERR_DEVICENOTRESET)
            {
                if (_instance->_isFullScreen && _lostCounter == 0)
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
                    if (nowWidth == 0 || nowHeight == 0)
                    {// サイズゼロは無理なので、初期値で
                        _presentParameters.BackBufferWidth = LAppDefine::RenderTargetWidth;
                        _presentParameters.BackBufferHeight = LAppDefine::RenderTargetHeight;
                    }

                    result = _device->Reset(&_presentParameters);
                }
                _lostCounter++;

                if (SUCCEEDED(result))
                {
                    // スプライト再作成
                    CubismDirectXView::GetInstance()->InitializeSprite();
                    // レンダラー再生
                    RestoreDeviceLost(_device);

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
            if (_instance->_isFullScreen && _lostCounter == 0)
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
                    _presentParameters.BackBufferWidth = LAppDefine::RenderTargetWidth;
                    _presentParameters.BackBufferHeight = LAppDefine::RenderTargetHeight;
                }

                presentParam = &_presentParameters;
            }
            _lostCounter++;

            // デバイス設定
            if (nowWidth == 0 || nowHeight == 0)// サイズが0の際は最小化されていると思われる CreateDeviceは成功しない
            {
                // NOP サイズが戻ったら再チャレンジ
            }
            else if (FAILED(_direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _windowHandle,
                D3DCREATE_HARDWARE_VERTEXPROCESSING, presentParam,
                &_device)))
            {
                LAppPal::PrintLogLn("Fail Direct3D Create Device");
                _device = NULL;
            }
            else
            {
                // デバイスが変わったことを通知
                Live2D::Cubism::Framework::Rendering::CubismRenderer_D3D9::InitializeConstantSettings(1, _device);

                // 描画のパラメータをウィンドウサイズに合わせて新設定
                CubismDirectXView::GetInstance()->Initialize(nowWidth, nowHeight);

                // スプライト再作成
                CubismDirectXView::GetInstance()->InitializeSprite();
                // レンダラー再生
                RestoreDeviceLost(_device);

                _deviceLostStep = LostStep::LostStep_None;
            }
        }
    }
}

bool CubismDirectXRenderer::CheckFullScreen(D3DFORMAT format, UINT width, UINT height)
{
    UINT nearest = UINT_MAX;
    UINT minWidh = 0, minHeight = 0;

    // ディスプレイモード数
    D3DDISPLAYMODE d3dspMode;
    int modeNum = _direct3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, format);
    for (int i = 0; i < modeNum; i++)
    {
        if (FAILED(_direct3D->EnumAdapterModes(D3DADAPTER_DEFAULT, format, i, &d3dspMode)))
        {
            continue;
        }

        // 指定サイズに一番近いもの
        INT nearW = d3dspMode.Width - width;
        INT nearH = d3dspMode.Height - height;
        UINT sum = abs(nearW) + abs(nearH);
        if (nearest > sum) // 差分が小さい
        {
            nearest = sum;
            minWidh = d3dspMode.Width;
            minHeight = d3dspMode.Height;
        }
    }

    if (nearest != UINT_MAX)
    {// 調査成功
        _presentParametersFull.BackBufferWidth = minWidh;
        _presentParametersFull.BackBufferHeight = minHeight;
    }

    // 見つかっていたらtrueを返す
    return nearest != UINT_MAX;
}
