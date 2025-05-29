/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <wrl/client.h>
#include <tchar.h>

#include "CubismTextureManager.hpp"
#include "CubismUserModelExtend.hpp"
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
    * @brief   デバイス再作成 ウィンドウサイズ変更の時など
    *           ロスト対応の+α処理を行う
    */
    void RecreateDevice();

    /**
    * @brief   フレーム最初の行動
    */
    void StartFrame() const;

    /**
    * @brief   フレーム最後の行動
    */
    void EndFrame(CubismUserModel* userModel);

    /**
    * @brief   デバイスロスト対処中かどうか
    */
    bool IsLostStep() const { return _deviceLostStep != LostStep_None; }

    /**
    * @brief   フルスクリーン解像度調査
    */
    bool CheckFullScreen(D3DFORMAT format, UINT width, UINT height);

    const LPCTSTR ClassName = _T("Cubism DirectX9 Sample");
    const csmInt32 BackBufferNum = 1; // バックバッファ枚数

    enum LostStep
    {
        LostStep_None,      ///< 平常
        LostStep_Lost,      ///< ロスト中
        LostStep_ReCreate,  ///< Deviceごと再作成
    };

    bool _isEnd;                                 ///< APP終了を通達する

    WNDCLASSEX _windowClass; ///< ウィンドウクラス

    HWND                    _windowHandle;  ///< ウィンドウハンドル
    LPDIRECT3D9             _direct3D;      ///< D3D
    LPDIRECT3DDEVICE9           _device;        ///< D3Dデバイス
    LostStep                _deviceLostStep;///< デバイス解放中
    int                     _lostCounter;   ///< デバイスロスト処理で再試行した回数
    bool                    _isFullScreen;  ///< フルスクリーン中か
    D3DPRESENT_PARAMETERS    _presentParameters; ///< プレゼントパラメータ
    D3DPRESENT_PARAMETERS   _presentParametersFull;     ///< プレゼントパラメータ フルスクリーン

    CubismTextureManager* _textureManager;         ///< テクスチャマネージャー

    CubismUserModelExtend* _model; ///< モデルデータ

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
    * @brief   デバイスロスト復帰時の対応
    */
    void RestoreDeviceLost(LPDIRECT3DDEVICE9 device);

    /**
    * @brief   デバイスロスト時の対応
    */
    void OnDeviceLost();
};
