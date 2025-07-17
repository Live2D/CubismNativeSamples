/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"
#include <math.h>
#include <string>
#include <d3dcompiler.h>
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "TouchManager_Common.hpp"
#include "LAppSprite.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace LAppDefine;

LAppView::LAppView():
    LAppView_Common(),
    _back(NULL),
    _gear(NULL),
    _power(NULL),
    _renderSprite(NULL),
    _renderTarget(SelectTarget_None),
    _shader(NULL)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager_Common();
}

LAppView::~LAppView()
{
    _renderBuffer.DestroyOffscreenSurface();

    ReleaseSprite();
    _shader->ReleaseShader();

   if (_shader)
    {
        delete _shader;
    }
    if (_touchManager)
    {
        delete _touchManager;
    }
}

void LAppView::Render()
{
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    if (!live2DManager)
    {
        return;
    }

    LPDIRECT3DDEVICE9 device = LAppDelegate::GetInstance()->GetD3dDevice();
    if (!device)
    {
        return;
    }

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    if (!textureManager)
    {
        return;
    }

    // スプライト描画
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    {
        IDirect3DTexture9* texture = NULL;
        // 透過設定
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);

        if (_back)
        {
            textureManager->GetTexture(_back->GetTextureId(), texture);
            _back->RenderImmidiate(device, width, height, texture);
        }
        if (_gear)
        {
            textureManager->GetTexture(_gear->GetTextureId(), texture);
            _gear->RenderImmidiate(device, width, height, texture);
        }
        if (_power)
        {
            textureManager->GetTexture(_power->GetTextureId(), texture);
            _power->RenderImmidiate(device, width, height, texture);
        }
    }

    live2DManager->SetViewMatrix(_viewMatrix);

    // Cubism更新・描画
    live2DManager->OnUpdate();

    // 各モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    {
        for (csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
        {
            LAppModel* model = live2DManager->GetModel(i);
            float alpha = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
            _renderSprite->SetColor(1.0f * alpha, 1.0f * alpha, 1.0f * alpha, alpha);

            if (model)
            {
                _renderSprite->RenderImmidiate(device, width, height, model->GetRenderBuffer().GetTexture());
            }
        }
    }
}

void LAppView::InitializeSprite()
{
    // シェーダ作成
    _shader = new LAppSpriteShader();
    if (!_shader->CreateShader())
    {
      return;
    }

    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    const string resourcesPath = ResourcesPath;

    string imageName = BackImageName;
    LAppTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName, false,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_FILTER_LINEAR);

    float x = width * 0.5f;
    float y = height * 0.5f;
    float fWidth = static_cast<float>(backgroundTexture->width * 2.0f);
    float fHeight = static_cast<float>(height * 0.95f);
    _back = new LAppSprite(x, y, fWidth, fHeight, backgroundTexture->id, _shader);

    imageName = GearImageName;
    LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName, false,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_FILTER_LINEAR);

    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(height - gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    _gear = new LAppSprite(x, y, fWidth, fHeight, gearTexture->id, _shader);

    imageName = PowerImageName;
    LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName, false,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_FILTER_LINEAR);

    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    _power = new LAppSprite(x, y, fWidth, fHeight, powerTexture->id, _shader);

    // 画面全体を覆うサイズ
    x = width * 0.5f;
    y = height * 0.5f;
    _renderSprite = new LAppSprite(x, y, static_cast<float>(width), static_cast<float>(height), 0, _shader);
}

void LAppView::ReleaseSprite()
{
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();

    if (_renderSprite)
    {
        textureManager->ReleaseTexture(_renderSprite->GetTextureId());
    }
    delete _renderSprite;
    _renderSprite = NULL;

    if (_gear)
    {
        textureManager->ReleaseTexture(_gear->GetTextureId());
    }
    delete _gear;
    _gear = NULL;

    if (_power)
    {
        textureManager->ReleaseTexture(_power->GetTextureId());
    }
    delete _power;
    _power = NULL;

    if (_back)
    {
        textureManager->ReleaseTexture(_back->GetTextureId());
    }
    delete _back;
    _back = NULL;
}

void LAppView::OnDeviceLost()
{
    // スプライト開放
    ReleaseSprite();

    // スプライト用のシェーダ・頂点宣言も開放
    if (_shader)
    {
      _shader->ReleaseShader();
    }
    delete _shader;
    _shader = NULL;

    // レンダリングターゲット開放
    _renderBuffer.DestroyOffscreenSurface();
}

void LAppView::OnTouchesBegan(float px, float py) const
{
    _touchManager->TouchesBegan(px, py);
}

void LAppView::OnTouchesMoved(float px, float py) const
{
    float viewX = this->TransformViewX(_touchManager->GetX());
    float viewY = this->TransformViewY(_touchManager->GetY());

    _touchManager->TouchesMoved(px, py);

    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    live2DManager->OnDrag(viewX, viewY);
}

void LAppView::OnTouchesEnded(float px, float py) const
{
    // タッチ終了
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    live2DManager->OnDrag(0.0f, 0.0f);
    {
        int width, height;
        LAppDelegate::GetInstance()->GetClientSize(width, height);

        // シングルタップ
        float x = _deviceToScreen->TransformX(px); // 論理座標変換した座標を取得。
        float y = _deviceToScreen->TransformY(py); // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable)
        {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
        live2DManager->OnTap(x, y);

        // 歯車にタップしたか
        if (_gear && _gear->IsHit(px, py, width, height))
        {
            live2DManager->NextScene();
        }

        // 電源ボタンにタップしたか
        if (_power && _power->IsHit(px, py, width, height))
        {
            LAppDelegate::GetInstance()->AppEnd();
        }
    }
}

void LAppView::PreModelDraw(LAppModel &refModel)
{
    if (_renderTarget != SelectTarget_None && !LAppDelegate::GetInstance()->IsLostStep())
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenSurface_D3D9* useTarget = NULL;

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {// 描画ターゲット内部未作成の場合はここで作成
            int width, height;
            LAppDelegate::GetClientSize(width, height);

            if (width != 0 && height != 0)
            {
                // モデル描画キャンバス
                useTarget->CreateOffscreenSurface(LAppDelegate::GetInstance()->GetD3dDevice(),
                    static_cast<csmUint32>(width), static_cast<csmUint32>(height));
            }
        }

        // レンダリング開始
        useTarget->BeginDraw(LAppDelegate::GetInstance()->GetD3dDevice());
        useTarget->Clear(LAppDelegate::GetInstance()->GetD3dDevice(), _clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    }
}

void LAppView::PostModelDraw(LAppModel &refModel)
{
    if (_renderTarget != SelectTarget_None && !LAppDelegate::GetInstance()->IsLostStep())
    {// 別のレンダリングターゲットへ向けて描画する場合
        LPDIRECT3DDEVICE9 device = LAppDelegate::GetInstance()->GetD3dDevice();
        // 透過設定
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);

        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenSurface_D3D9* useTarget = NULL;

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw(LAppDelegate::GetInstance()->GetD3dDevice());

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            // スプライト描画
            int width, height;
            LAppDelegate::GetInstance()->GetClientSize(width, height);

            _renderSprite->SetColor(1.0f * GetSpriteAlpha(0), 1.0f * GetSpriteAlpha(0), 1.0f * GetSpriteAlpha(0), GetSpriteAlpha(0));
            _renderSprite->RenderImmidiate(LAppDelegate::GetInstance()->GetD3dDevice(),
                                           width, height, useTarget->GetTexture());
        }
    }
}

void LAppView::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void LAppView::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}


float LAppView::GetSpriteAlpha(int assign) const
{
    // assignの数値に応じて適当に決定
    float alpha = 0.4f + static_cast<float>(assign) * 0.5f; // サンプルとしてαに適当な差をつける
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
