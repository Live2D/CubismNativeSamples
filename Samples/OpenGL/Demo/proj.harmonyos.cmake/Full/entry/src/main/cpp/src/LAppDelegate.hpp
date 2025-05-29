/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "LAppAllocator_Common.hpp"

class LAppView;
class LAppTextureManager;

/**
* @brief   アプリケーションクラス。
*   Cubism SDK の管理を行う。
*/
class LAppDelegate
{
public:
    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static LAppDelegate* GetInstance();

    /**
    * @brief   クラスのインスタンス（シングルトン）を解放する。
    *
    */
    static void ReleaseInstance();

    /**
     * @brief 実行処理。
     */
    void Run();

    /**
     * @brief EGLの設定
     *
     */
    void InitializeEgl();

    /**
     * @brief EGLの破棄
     *
     */
    void ReleaseEgl();

    /**
     * @brief OnSurfaceCreateのコールバック関数。
     *
     */
    void InitializeDisplay();

    /**
     * @brief OnSurfaceChangedのコールバック関数。
     *
     * @param width
     * @param height
     */
    void ChangeDisplay(float width, float height);

    /**
    * @brief Touch開始。
    *
    * @param[in] x x座標
    * @param[in] y x座標
    */
    void OnTouchBegan(double x, double y);

    /**
    * @brief Touch終了。
    *
    * @param[in] x x座標
    * @param[in] y x座標
    */
    void OnTouchEnded(double x, double y);

    /**
    * @brief Touch移動。
    *
    * @param[in] x x座標
    * @param[in] y x座標
    */
    void OnTouchMoved(double x, double y);

    /**
    * @brief テクスチャマネージャーの取得
    */
    LAppTextureManager* GetTextureManager() { return _textureManager; }

    /**
    * @brief ウインドウ幅の取得
    */
    int GetWindowWidth() { return _width; }

    /**
    * @brief ウインドウ高さの取得
    */
    int GetWindowHeight() { return _height; }

    /**
     * @brief windowの設定
     *
     * @param window
     */
    void SetWindow(void *window) { _eglWindow = reinterpret_cast<EGLNativeWindowType>(window); }

    /**
     * @brief ウインドウの取得
     */
    const void* GetWindow() { return reinterpret_cast<void*>(_eglWindow); };

    /**
    * @brief   アプリケーションを非アクティブにする。
    */
    void DeActivateApp() { _isActive = false; }

    /**
    * @brief   View情報を取得する。
    */
    LAppView* GetView() { return _view; }

private:
    /**
    * @brief コンストラクタ
    */
    LAppDelegate();

    /**
    * @brief デストラクタ
    */
    ~LAppDelegate();

    LAppAllocator_Common _cubismAllocator;       ///< Cubism SDK Allocator
    Csm::CubismFramework::Option _cubismOption;  ///< Cubism SDK Option
    LAppTextureManager* _textureManager;         ///< テクスチャマネージャー
    LAppView* _view;                             ///< View情報
    int _width;                                  ///< Windowの幅
    int _height;                                 ///< windowの高さ
    bool _captured;                              ///< クリックしているか
    bool _isActive;                              ///< アプリがアクティブ状態なのか
    float _mouseY;                               ///< マウスY座標
    float _mouseX;                               ///< マウスX座標
    EGLNativeWindowType _eglWindow;
    EGLDisplay _eglDisplay;
    EGLConfig _eglConfig;
    EGLSurface _eglSurface;
    EGLContext _eglContext;
    EGLContext _eglSharedContext;
};
