﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <wrl/client.h>

#include "CubismTextureManager.hpp"
#include "Model/CubismUserModel.hpp"

using namespace Csm;

class CubismDirectXView;

    /**
    * @brief   DirectXやWINAPIのレンダリング関連をまとめている
    */
class CubismDirectXRenderer
{
public:
    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static CubismDirectXRenderer* GetInstance();

    /**
    * @brief   D3Dデバイスの取得
    */
    ID3D11Device* GetD3dDevice();

    /**
    * @brief   D3Dデバイスコンテキストの取得
    */
    ID3D11DeviceContext* GetD3dContext();

    /**
    * @brief   ウィンドウクライアント領域取得
    */
    void GetWindowRect(RECT& rect);

    /**
    * @brief   ウィンドウクライアント領域の幅、高さ取得
    */
    void GetClientSize(int& rWidth, int& rHeight);

    /**
    * @brief   コンストラクタ
    */
    CubismDirectXRenderer();

    /**
    * @brief   クラスのインスタンス（シングルトン）を解放する
    *
    */
    void ReleaseInstance();

    /**
    * @brief   スプライト描画用シェーダの作成と頂点宣言の作成を行う
    */
    bool CreateShader();

    /**
    * @brief   レンダーターゲット作成内部関数
    */
    bool CreateRenderTarget(UINT width, UINT height);

    /**
    * @brief   シェーダをコンテキストにセット
    */
    void SetupShader();

    /**
    * @brief   CreateShaderで確保したオブジェクトの開放
    */
    void ReleaseShader();

    /**
    * @brief   アプリケーションを終了するかどうか。
    */
    bool GetIsEnd() const { return _isEnd; }

    /**
    * @brief   フレーム最初の行動
    */
    void StartFrame() const;

    /**
    * @brief   フレーム最後の行動
    */
    void EndFrame(CubismUserModel* userModel);

    const LPCSTR ClassName = "Cubism DirectX11 Sample";
    const csmInt32 BackBufferNum = 1; // バックバッファ枚数

    enum DeviceStep
    {
        DeviceStep_None,      ///< 平常
        DeviceStep_Lost,      ///< ロスト
        DeviceStep_Size,      ///< サイズ変更
    };

    bool _isEnd;                                 ///< APP終了を通達する

    WNDCLASSEX _windowClass; ///< ウィンドウクラス

    HWND                    _windowHandle;  ///< ウィンドウハンドル
    ID3D11Device*           _device;        ///< D3Dデバイス
    IDXGISwapChain*         _swapChain;     ///< スワップチェーン
    ID3D11DeviceContext*    _deviceContext; ///< D3D描画コンテキスト
    DeviceStep              _deviceStep;    ///< デバイスサイズ変更などのステップ
    DXGI_SWAP_CHAIN_DESC    _presentParameters; ///< プレゼントパラメータ

    ID3D11RenderTargetView* _renderTargetView;  ///< 描画ターゲットビュー
    ID3D11Texture2D*        _depthTexture;      ///< Zバッファ
    ID3D11DepthStencilView* _depthStencilView;  ///< Zバッファビュー
    ID3D11DepthStencilState* _depthState;       ///< スプライト描画用深度オブジェクト

    ID3D11RasterizerState*  _rasterizer;    ///< スプライト描画用ラスタライザ
    ID3D11SamplerState*     _samplerState;  ///< スプライト描画用サンプラーステート

    ID3D11VertexShader*     _vertexShader;  ///< スプライト描画シェーダ
    ID3D11PixelShader*      _pixelShader;   ///< スプライト描画シェーダ
    ID3D11BlendState*       _blendState;    ///< スプライト描画用ブレンドステート
    ID3D11InputLayout*      _vertexFormat;  ///< スプライト描画用型宣言

    CubismTextureManager* _textureManager;         ///< テクスチャマネージャー

private:
    /**
    * @brief   デストラクタ
    */
    ~CubismDirectXRenderer();

    /**
     * @brief   解放処理
     */
    void Release();

    /**
     * @brief   デバイスのサイズ変更
     */
    void ResizeDevice(CubismUserModel* userModel);
};
