/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView_Common.hpp"

#include <math.h>

#include "LAppDefine.hpp"

LAppView_Common::LAppView_Common()
{
    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new Csm::CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new Csm::CubismViewMatrix();
}

LAppView_Common::~LAppView_Common()
{
    delete _viewMatrix;
    delete _deviceToScreen;
}

void LAppView_Common::Initialize(int width, int height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = LAppDefine::ViewLogicalLeft;
    float top = LAppDefine::ViewLogicalRight;

    _viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->Scale(LAppDefine::ViewScale, LAppDefine::ViewScale);

    _deviceToScreen->LoadIdentity(); // サイズが変わった際などリセット必須
    if (width > height)
    {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    }
    else
    {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(LAppDefine::ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(LAppDefine::ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
        LAppDefine::ViewLogicalMaxLeft,
        LAppDefine::ViewLogicalMaxRight,
        LAppDefine::ViewLogicalMaxBottom,
        LAppDefine::ViewLogicalMaxTop
    );
}

float LAppView_Common::TransformViewX(float deviceX) const
{
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float LAppView_Common::TransformViewY(float deviceY) const
{
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float LAppView_Common::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float LAppView_Common::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}
