﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismDirectXView.hpp"

#include "CubismDirectXRenderer.hpp"
#include "CubismSampleViewMatrix.hpp"

namespace {
    CubismDirectXView* _instance = nullptr;
}

CubismDirectXView* CubismDirectXView::GetInstance()
{
    if (!_instance)
    {
        _instance = new CubismDirectXView();
    }

    return  _instance;
}

CubismDirectXView::CubismDirectXView()
    :_windowWidth(0), _windowHeight(0), _renderSprite(nullptr), _renderTarget(SelectTarget_None)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    _mouseActionManager = MouseActionManager::GetInstance();

    // デバイス座標からスクリーン座標に変換するための行列
    _deviceToScreen = new CubismMatrix44();

    Initialize();
}

CubismDirectXView::~CubismDirectXView()
{
    _renderBuffer.DestroyOffscreenFrame();

    delete _renderSprite;
    delete _deviceToScreen;

    ReleaseSprite();
}

void CubismDirectXView::Initialize()
{
    CubismDirectXRenderer::GetInstance()->GetClientSize(_windowWidth, _windowHeight);

    InitializeSprite();
}

void CubismDirectXView::Render(CubismUserModel* userModel)
{
    // スプライト描画
    CubismDirectXRenderer::GetInstance()->GetClientSize(_windowWidth, _windowHeight);

    //AppViewの初期化
    MouseActionManager::GetInstance()->ViewInitialize(_windowWidth, _windowHeight);

    CubismUserModelExtend* model = static_cast<CubismUserModelExtend*>(userModel);

    // モデル描画の事前準備
    PreModelDraw(*model);

    // モデルの更新及び描画
    model->ModelOnUpdate();

    // モデル描画の後処理
    PostModelDraw(*model);

    // モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    {
        float alpha = GetSpriteAlpha(2); // サンプルとしてαに適当な差をつける
        _renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

        if (userModel)
        {
            _renderSprite->RenderImmidiate(CubismDirectXRenderer::GetInstance()->_device, _windowWidth, _windowHeight, model->GetRenderBuffer().GetTexture());
        }
    }
}

void CubismDirectXView::InitializeSprite()
{
    CubismDirectXRenderer::GetInstance()->GetClientSize(_windowWidth, _windowHeight);
    float x = _windowWidth * 0.5f;
    float y = _windowHeight * 0.5f;
    _renderSprite = new CubismSprite(x, y, static_cast<float>(_windowWidth), static_cast<float>(_windowHeight), 0);

    // シェーダ作成
    CubismDirectXRenderer::GetInstance()->CreateShader();
}

void CubismDirectXView::ReleaseSprite()
{
    delete _renderSprite;
    _renderSprite = nullptr;

    // スプライト用のシェーダ・頂点宣言も開放
    CubismDirectXRenderer::GetInstance()->ReleaseShader();
}

void CubismDirectXView::ResizeSprite()
{
    // 描画領域サイズ
    CubismDirectXRenderer::GetInstance()->GetClientSize(_windowWidth, _windowHeight);

    if (_renderSprite)
    {
        float x = _windowWidth * 0.5f;
        float y = _windowHeight * 0.5f;
        _renderSprite->ResetRect(x, y, static_cast<float>(_windowWidth), static_cast<float>(_windowHeight));
    }
}

float CubismDirectXView::TransformViewX(float deviceX) const
{
    CubismSampleViewMatrix* viewMatrix = MouseActionManager::GetInstance()->GetViewMatrix();
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float CubismDirectXView::TransformViewY(float deviceY) const
{
    CubismSampleViewMatrix* viewMatrix = MouseActionManager::GetInstance()->GetViewMatrix();
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float CubismDirectXView::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float CubismDirectXView::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}

void CubismDirectXView::PreModelDraw(CubismUserModelExtend& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenFrame_D3D9* useTarget = nullptr;

    if (_renderTarget != SelectTarget_None)
    {
        // 別のレンダリングターゲットへ向けて描画する場合
        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {
            // 描画ターゲット内部未作成の場合はここで作成
            CubismDirectXRenderer::GetInstance()->GetClientSize(_windowWidth, _windowHeight);

            if (_windowWidth != 0 && _windowHeight != 0)
            {
                // モデル描画キャンバス
                useTarget->CreateOffscreenFrame(CubismDirectXRenderer::GetInstance()->_device,
                    static_cast<csmUint32>(_windowWidth), static_cast<csmUint32>(_windowHeight));
            }
        }

        // レンダリング開始
        useTarget->BeginDraw(CubismDirectXRenderer::GetInstance()->_device);
        useTarget->Clear(CubismDirectXRenderer::GetInstance()->_device, _clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    }
}

void CubismDirectXView::PostModelDraw(CubismUserModelExtend& refModel)
{
    if (_renderTarget != SelectTarget_None && !CubismDirectXRenderer::GetInstance()->IsLostStep())
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenFrame_D3D9* useTarget = NULL;

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw(CubismDirectXRenderer::GetInstance()->_device);

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            // スプライト描画
            int width, height;
            CubismDirectXRenderer::GetInstance()->GetClientSize(width, height);

            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            _renderSprite->RenderImmidiate(CubismDirectXRenderer::GetInstance()->_device, width, height, useTarget->GetTexture());
        }
    }
}

float CubismDirectXView::GetSpriteAlpha(int assign) const
{
    // assignの数値に応じて適当に決定
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    if (alpha < 0.1f)
    {
        alpha = 0.1f;
    }

    return alpha;
}

void CubismDirectXView::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void CubismDirectXView::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

void CubismDirectXView::DestroyOffscreenFrame()
{
    _renderBuffer.DestroyOffscreenFrame();
}

void CubismDirectXView::OnDeviceLost()
{
    // スプライト開放
    ReleaseSprite();

    // レンダリングターゲット開放
    _renderBuffer.DestroyOffscreenFrame();
}
