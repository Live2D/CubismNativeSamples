/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Math/CubismViewMatrix.hpp>

/**
* @brief CubismViewMatrixを継承したクラス
*
* ビュー行列を Cubism で扱いやすいように機能を加えてラップしたもの。
*
*/
class CubismSampleViewMatrix :
    public Csm::CubismViewMatrix
{
public:
    CubismSampleViewMatrix(Csm::CubismMatrix44*& deviceToScreen, int windowWidth, int windowHeight); ///< コンストラクタ

    virtual ~CubismSampleViewMatrix(); ///< デストラクタ
};
