/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GLFW/glfw3.h>

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <Math/CubismMatrix44.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>

#include "LAppDefine.hpp"
#include "TouchManager_Common.hpp"
#include "MouseActionManager_Common.hpp"

/**
* @brief マウスの動作を通知するクラス
*
* マウス操作等を Cubism へ橋渡しする。
*
*/
class MouseActionManager : public MouseActionManager_Common
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
