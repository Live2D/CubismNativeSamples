/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"
#include <math.h>
#include <string>
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "TouchManager.hpp"
#include "LAppSprite.hpp"
#include "LAppModel.hpp"

using namespace std;
using namespace LAppDefine;

LAppView::LAppView():
    _back(NULL),
    _gear(NULL),
    _power(NULL),
    _renderSprite(NULL),
    _renderTarget(SelectTarget_None)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager();

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();
}

LAppView::~LAppView()
{
    _renderBuffer.DestroyOffscreenFrame();
    delete _renderSprite;

    delete _viewMatrix;
    delete _deviceToScreen;
    delete _touchManager;
    delete _back;
    delete _gear;
    delete _power;
}

void LAppView::Initialize()
{
    int width, height;
    LAppDelegate::GetClientSize(width, height);

    if (width == 0 || height == 0)
    {
        return;
    }

    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    _viewMatrix->SetScreenRect(left, right, bottom, top); // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->Scale(ViewScale, ViewScale);

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
    _viewMatrix->SetMaxScale(ViewMaxScale); // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale); // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(
        ViewLogicalMaxLeft,
        ViewLogicalMaxRight,
        ViewLogicalMaxBottom,
        ViewLogicalMaxTop
    );
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

    // スプライト描画
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    {
        if (_back)
        {
            _back->Render(device, width, height);
        }
        if (_gear)
        {
            _gear->Render(device, width, height);
        }
        if (_power)
        {
            _power->Render(device, width, height);
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
            float alpha = GetSpriteAlpha(i); // サンプルとしてαに適当な差をつける
            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

            LAppModel *model = live2DManager->GetModel(i);
            if (model)
            {
                _renderSprite->RenderImmidiate(LAppDelegate::GetInstance()->GetD3dDevice(), width, height, model->GetRenderBuffer().GetTexture());
            }
        }
    }
}

void LAppView::InitializeSprite()
{
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
    _back = new LAppSprite(x, y, fWidth, fHeight, backgroundTexture->id);

    imageName = GearImageName;
    LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName, false,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_FILTER_LINEAR);

    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(height - gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    _gear = new LAppSprite(x, y, fWidth, fHeight, gearTexture->id);

    imageName = PowerImageName;
    LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName, false,
        D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DX_FILTER_LINEAR);

    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    _power = new LAppSprite(x, y, fWidth, fHeight, powerTexture->id);

    // 画面全体を覆うサイズ
    x = width * 0.5f;
    y = height * 0.5f;
    _renderSprite = new LAppSprite(x, y, static_cast<float>(width), static_cast<float>(height), 0);

    // シェーダ作成
    LAppDelegate::GetInstance()->CreateShader();
}

void LAppView::ReleaseSprite()
{
    delete _renderSprite; _renderSprite = NULL;
    delete _power; _power = NULL;
    delete _gear; _gear = NULL;
    delete _back; _back = NULL;

    // スプライト用のシェーダ・頂点宣言も開放
    LAppDelegate::GetInstance()->ReleaseShader();
}

void LAppView::OnDeviceLost()
{
    // スプライト開放
    ReleaseSprite();

    // レンダリングターゲット開放
    _renderBuffer.DestroyOffscreenFrame();
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
            LAppPal::PrintLog("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
        live2DManager->OnTap(x, y);

        // 歯車にタップしたか
        if (_gear && _gear->IsHit(px, py))
        {
            live2DManager->NextScene();
        }

        // 電源ボタンにタップしたか
        if (_power && _power->IsHit(px, py))
        {
            LAppDelegate::GetInstance()->AppEnd();
        }
    }
}

float LAppView::TransformViewX(float deviceX) const
{
    float screenX = _deviceToScreen->TransformX(deviceX); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX); // 拡大、縮小、移動後の値。
}

float LAppView::TransformViewY(float deviceY) const
{
    float screenY = _deviceToScreen->TransformY(deviceY); // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY); // 拡大、縮小、移動後の値。
}

float LAppView::TransformScreenX(float deviceX) const
{
    return _deviceToScreen->TransformX(deviceX);
}

float LAppView::TransformScreenY(float deviceY) const
{
    return _deviceToScreen->TransformY(deviceY);
}

void LAppView::PreModelDraw(LAppModel &refModel)
{
    if (_renderTarget != SelectTarget_None && !LAppDelegate::GetInstance()->IsLostStep())
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenFrame_D3D9* useTarget = NULL;

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {// 描画ターゲット内部未作成の場合はここで作成
            int width, height;
            LAppDelegate::GetClientSize(width, height);

            if (width != 0 && height != 0)
            {
                // モデル描画キャンバス
                useTarget->CreateOffscreenFrame(LAppDelegate::GetInstance()->GetD3dDevice(),
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

        // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
        Csm::Rendering::CubismOffscreenFrame_D3D9* useTarget = NULL;

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

            _renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));
            _renderSprite->RenderImmidiate(LAppDelegate::GetInstance()->GetD3dDevice(), width, height, useTarget->GetTexture());
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
