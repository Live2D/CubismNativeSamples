/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>

#include "CubismFramework.hpp"

/**
* @brief 描画クラス
*/
class LAppView_Common
{
public:
    /**
     * @brief コンストラクタ
     */
    LAppView_Common();

    /**
    * @brief デストラクタ
    */
    virtual ~LAppView_Common();

    /**
    * @brief 初期化する。
    */
    virtual void Initialize(int width, int height);

    /**
    * @brief X座標をView座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    virtual float TransformViewX(float deviceX) const;

    /**
    * @brief Y座標をView座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    virtual float TransformViewY(float deviceY) const;

    /**
    * @brief X座標をScreen座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    virtual float TransformScreenX(float deviceX) const;

    /**
    * @brief Y座標をScreen座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    virtual float TransformScreenY(float deviceY) const;

protected:
    Csm::CubismMatrix44* _deviceToScreen;    ///< デバイスからスクリーンへの行列
    Csm::CubismViewMatrix* _viewMatrix;      ///< viewMatrix
};
