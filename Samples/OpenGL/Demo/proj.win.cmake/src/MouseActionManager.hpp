/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <Math/CubismMatrix44.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>

#include "LAppDefine.hpp"
#include "TouchManager.hpp"
#include "CubismSampleViewMatrix.hpp"

/**
* @brief マウスの動作を通知するクラス
*
* マウス操作等を Cubism へ橋渡しする。
*
*/
class MouseActionManager
{
public:
  /**
  * @brief   クラスのインスタンス（シングルトン）を返す
  *
  * インスタンスが生成されていない場合は内部でインスタンスを生成する
  *
  * @return  クラスのインスタンス
  */
  static MouseActionManager* GetInstance();

  /**
  * @brief   クラスのインスタンス（シングルトン）を解放する
  *
  */
  static void ReleaseInstance();

  MouseActionManager(); ///< コンストラクタ
  ~MouseActionManager(); ///< デストラクタ

  /**
  * @brief   必要なものを初期化する
  */
  void Initialize(int windowWidth, int windowHeight);

  /**
  * @brief 行列の初期化をする
  */
  void ViewInitialize(int windowWidth, int windowHeight);

  /**
  * @brief   OpenGL用 glfwSetMouseButtonCallback用関数。
  *
  * @param[in]       window            コールバックを呼んだWindow情報
  * @param[in]       button            ボタン種類
  * @param[in]       action            実行結果
  * @param[in]       modify
  */
  void OnMouseCallBack(GLFWwindow* window, int button, int action, int modify);

  /**
  * @brief   OpenGL用 glfwSetCursorPosCallback用関数。
  *
  * @param[in]       window            コールバックを呼んだWindow情報
  * @param[in]       x                 x座標
  * @param[in]       y                 x座標
  */
  void OnMouseCallBack(GLFWwindow* window, double x, double y);

  /**
  * @brief _userModelをセットする
  *
  */
  void SetUserModel(Csm::CubismUserModel* userModel);

  /**
  * @brief _viewMatrixを取得する
  *
  */
  CubismSampleViewMatrix * GetViewMatrix();

private:
    /**
    * @brief ドラッグ
    *
    * ドラッグ時にどれだけ移動したかを通知する
    */
    void OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
    * @brief クリック入力始め
    *
    * クリックの入力が始まった時に呼ばれる
    */
    void OnTouchesBegan(float px, float py);

    /**
    * @brief クリック入力中の移動
    *
    * クリック入力中の移動時に呼ばれる
    */
    void OnTouchesMoved(float px, float py);

    /**
    * @brief クリック入力終了時
    *
    * クリックの入力が終了した時に呼ばれる
    */
    void OnTouchesEnded(float px, float py);

    Csm::CubismUserModel* _userModel;

    TouchManager* _touchManager;                 ///< タッチマネージャー

    bool _captured;                              ///< クリックしているか
    float _mouseX;                               ///< マウスX座標
    float _mouseY;                               ///< マウスY座標

    CubismSampleViewMatrix* _viewMatrix; ///< 画面の表示の拡大縮小や移動の変換を行う行列
    Csm::CubismMatrix44* _deviceToScreen; ///< デバイスからスクリーンへの行列
};

class EventHandler
{
public:
    /**
    * @brief   glfwSetMouseButtonCallback用コールバック関数。
    */
    static void OnMouseCallBack(GLFWwindow* window, int button, int action, int modify)
    {
        MouseActionManager::GetInstance()->OnMouseCallBack(window, button, action, modify);
    }

    /**
    * @brief   glfwSetCursorPosCallback用コールバック関数。
    */
    static void OnMouseCallBack(GLFWwindow* window, double x, double y)
    {
        MouseActionManager::GetInstance()->OnMouseCallBack(window, x, y);
    }
};
