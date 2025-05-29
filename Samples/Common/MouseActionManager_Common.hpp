/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once


#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <Math/CubismMatrix44.hpp>

#include "TouchManager_Common.hpp"
#include "CubismSampleViewMatrix_Common.hpp"

/**
* @brief マウスの動作を通知するクラス
*
* マウス操作等を Cubism へ橋渡しする。
*
*/
class MouseActionManager_Common
{
public:
    /**
     * @brief   クラスのインスタンス（シングルトン）を返す
     *
     * インスタンスが生成されていない場合は内部でインスタンスを生成する
     *
     * @return  クラスのインスタンス
     */
    static MouseActionManager_Common* GetInstance();

    /**
     * @brief   クラスのインスタンス（シングルトン）を解放する
     */
    static void ReleaseInstance();

    MouseActionManager_Common(); ///< コンストラクタ
    virtual ~MouseActionManager_Common(); ///< デストラクタ

    /**
     * @brief   必要なものを初期化する
     */
    virtual void Initialize(int windowWidth, int windowHeight);

    /**
     * @brief 行列の初期化をする
     */
    virtual void ViewInitialize(int windowWidth, int windowHeight);

    /**
     * @brief _userModelをセットする
     */
    virtual void SetUserModel(Csm::CubismUserModel* userModel);

    /**
     * @brief _viewMatrixを取得する
     */
    virtual CubismSampleViewMatrix_Common* GetViewMatrix();

    /**
    * @brief ドラッグ
    *
    * ドラッグ時にどれだけ移動したかを通知する
    */
    virtual void OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
    * @brief クリック入力始め
    *
    * クリックの入力が始まった時に呼ばれる
    */
    virtual void OnTouchesBegan(float px, float py);

    /**
    * @brief クリック入力中の移動
    *
    * クリック入力中の移動時に呼ばれる
    */
    virtual void OnTouchesMoved(float px, float py);

    /**
    * @brief クリック入力終了時
    *
    * クリックの入力が終了した時に呼ばれる
    */
    virtual void OnTouchesEnded(float px, float py);

protected:
    Csm::CubismUserModel* _userModel;

    TouchManager_Common* _TouchManager;                 ///< タッチマネージャー

    bool _captured;                              ///< クリックしているか
    float _mouseX;                               ///< マウスX座標
    float _mouseY;                               ///< マウスY座標

    CubismSampleViewMatrix_Common* _viewMatrix; ///< 画面の表示の拡大縮小や移動の変換を行う行列
    Csm::CubismMatrix44* _deviceToScreen; ///< デバイスからスクリーンへの行列
};
