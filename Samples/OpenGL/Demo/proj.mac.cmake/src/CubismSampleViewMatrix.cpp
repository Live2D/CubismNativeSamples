/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "CubismSampleViewMatrix.hpp"
#include "LAppDefine.hpp"

#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>

CubismSampleViewMatrix::CubismSampleViewMatrix(Csm::CubismMatrix44*& deviceToScreen, int windowWidth, int windowHeight)
    : CubismViewMatrix()
{
    if (windowWidth == 0 || windowHeight == 0)
    {
        return;
    }

    // 縦サイズを基準とする
    float ratio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float left = -ratio;
    float right = ratio;
    float bottom = LAppDefine::ViewLogicalLeft;
    float top = LAppDefine::ViewLogicalRight;

    // デバイスに対応する画面の範囲を設定
    SetScreenRect(left, right, bottom, top);

    if (windowWidth > windowHeight)
    {
        float screenW = fabsf(right - left);
        // 行列の拡大率を相対的に設定
        deviceToScreen->ScaleRelative(screenW / windowWidth, -screenW / windowWidth);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        // 行列の拡大率を相対的に設定
        deviceToScreen->ScaleRelative(screenH / windowHeight, -screenH / windowHeight);
    }

    // 行列の位置を起点に移動を行う
    deviceToScreen->TranslateRelative(-windowWidth * 0.5f, -windowHeight * 0.5f);

    // 拡大率を設定
    Scale(LAppDefine::ViewScale, LAppDefine::ViewScale);

    // 最大拡大率を設定
    SetMaxScale(LAppDefine::ViewMaxScale);

    // 最小拡大率を設定
    SetMinScale(LAppDefine::ViewMinScale);

    // デバイスに対応する論理座標上の移動可能範囲を設定
    SetMaxScreenRect(
        LAppDefine::ViewLogicalMaxLeft,
        LAppDefine::ViewLogicalMaxRight,
        LAppDefine::ViewLogicalMaxBottom,
        LAppDefine::ViewLogicalMaxTop
    );
}

CubismSampleViewMatrix::~CubismSampleViewMatrix()
{
}
