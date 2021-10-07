﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/D3D9/CubismOffscreenSurface_D3D9.hpp>
#include <Math/CubismMatrix44.hpp>
#include "CubismFramework.hpp"

#include "MouseActionManager.hpp"
#include "CubismUserModelExtend.hpp"
#include "CubismSprite.hpp"

class CubismDirectXView
{
public:
    /**
    * @brief LAppModelのレンダリング先
    */
    enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< LAppViewの持つフレームバッファにレンダリング
    };

    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
    static CubismDirectXView* GetInstance();

    /**
    * @brief コンストラクタ
    */
    CubismDirectXView();

    /**
    * @brief デストラクタ
    */
    ~CubismDirectXView();

    /**
    * @brief 初期化する。
    */
    void Initialize();

    /**
    * @brief 描画する。
    */
    void Render(Csm::CubismUserModel* userModel);

    /**
    * @brief 画像の初期化を行う。
    */
    void InitializeSprite();

    /**
    * @brief スプライト系の開放
    */
    void ReleaseSprite();

    /**
    * @brief スプライト系のサイズ再設定
    */
    void ResizeSprite();

    /**
    * @brief X座標をView座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformViewX(float deviceX) const;

    /**
    * @brief Y座標をView座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformViewY(float deviceY) const;

    /**
    * @brief X座標をScreen座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformScreenX(float deviceX) const;

    /**
    * @brief Y座標をScreen座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformScreenY(float deviceY) const;

    /**
    * @brief   モデル1体を描画する直前にコールされる
    */
    void PreModelDraw(CubismUserModelExtend& refModel);

    /**
    * @brief   モデル1体を描画した直後にコールされる
    */
    void PostModelDraw(CubismUserModelExtend& refModel);

    /**
    * @brief   別レンダリングターゲットにモデルを描画するサンプルで
    *           描画時のαを決定する
    */
    float GetSpriteAlpha(int assign) const;

    /**
    * @brief レンダリング先を切り替える
    */
    void SwitchRenderingTarget(SelectTarget targetType);

    /**
    * @brief レンダリング先をデフォルト以外に切り替えた際の背景クリア色設定
    * @param[in]   r   赤(0.0~1.0)
    * @param[in]   g   緑(0.0~1.0)
    * @param[in]   b   青(0.0~1.0)
    */
    void SetRenderTargetClearColor(float r, float g, float b);

    /**
    * @brief   内部レンダリングバッファの破棄
    */
    void DestroyOffscreenFrame();

    /**
    * @brief デバイスロストの際の処理
    */
    void OnDeviceLost();

private:
    MouseActionManager* _mouseActionManager;                 ///< マウスアクションマネージャー
    Csm::CubismMatrix44* _deviceToScreen;      ///< デバイスからスクリーンへの行列
    int _windowWidth;    ///< 画面の幅
    int _windowHeight;    ///< 画面の高さ

    // レンダリング先を別ターゲットにする方式の場合に使用
    CubismSprite* _renderSprite;                                  ///< モードによっては_renderBufferのテクスチャを描画
    Csm::Rendering::CubismOffscreenFrame_D3D9 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    SelectTarget _renderTarget;     ///< レンダリング先の選択肢
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー
};

