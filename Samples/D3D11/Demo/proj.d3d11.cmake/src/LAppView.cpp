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
#include "TouchManager_Common.hpp"
#include "LAppSprite.hpp"
#include "LAppSpriteShader.hpp"
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
    _spriteShader(NULL),
    _modelSpriteShader(NULL)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager_Common();

    // スプライト用シェーダー
    _spriteShader = new LAppSpriteShader();

    // モデルスプライト用シェーダー
    _modelSpriteShader = new LAppModelSpriteShader();
}

LAppView::~LAppView()
{
    _renderBuffer.DestroyOffscreenSurface();

    ReleaseSprite();

    if (_spriteShader)
    {
        _spriteShader->ReleaseShader();
        delete _spriteShader;
    }
    if (_modelSpriteShader)
    {
        _modelSpriteShader->ReleaseShader();
        delete _modelSpriteShader;
    }
    if (_touchManager)
    {
        delete _touchManager;
    }
}

void LAppView::Initialize(int width, int height)
{
    LAppView_Common::Initialize(width, height);

    // シェーダー作成
    _spriteShader->CreateShader();
    _modelSpriteShader->CreateShader();
}

void LAppView::Render()
{
    LAppLive2DManager* live2DManager = LAppLive2DManager::GetInstance();
    if (!live2DManager)
    {
        return;
    }

    // スプライト描画
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    // デバイスコンテキスト取得
    ID3D11DeviceContext* renderContext = LAppDelegate::GetD3dContext();

    // デバイス取得
    ID3D11Device* device = LAppDelegate::GetInstance()->GetD3dDevice();

    // 透過設定
    if (_back)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_back->GetTextureId(), textureView);
        _back->RenderImmidiate(width, height, textureView, renderContext);
    }
    if (_gear)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_gear->GetTextureId(), textureView);
        _gear->RenderImmidiate(width, height, textureView, renderContext);
    }
    if (_power)
    {
        ID3D11ShaderResourceView* textureView = NULL;
        LAppDelegate::GetInstance()->GetTextureManager()->GetTexture(_power->GetTextureId(), textureView);
        _power->RenderImmidiate(width, height, textureView, renderContext);
    }

    live2DManager->SetViewMatrix(_viewMatrix);

    // Cubism更新・描画
    live2DManager->OnUpdate();

    // 透過設定
    if (_renderTarget == SelectTarget_ModelFrameBuffer)
    {
        for (csmUint32 i = 0; i < live2DManager->GetModelNum(); i++)
        {
            LAppModel* model = live2DManager->GetModel(i);
            float alpha = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
            _renderSprite->SetColor(1.0f * alpha, 1.0f * alpha, 1.0f * alpha, alpha);

            if (model)
            {
                _renderSprite->RenderImmidiate(width, height, model->GetRenderBuffer().GetTextureView(), renderContext);
            }
        }
    }
}

void LAppView::InitializeSprite()
{
    // 描画領域サイズ
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    const string resourcesPath = ResourcesPath;

    ID3D11Device* device = LAppDelegate::GetInstance()->GetD3dDevice();

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    string imageName = resourcesPath + BackImageName;
    LAppTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = width * 0.5f;
    y = height * 0.5f;
    fWidth = static_cast<float>(backgroundTexture->width * 2);
    fHeight = static_cast<float>(height) * 0.95f;
    _back = new LAppSprite(x, y, fWidth, fHeight, backgroundTexture->id, _spriteShader, device);

    imageName = resourcesPath + GearImageName;
    LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = static_cast<float>(width - gearTexture->width * 0.5f);
    y = static_cast<float>(height - gearTexture->height * 0.5f);
    fWidth = static_cast<float>(gearTexture->width);
    fHeight = static_cast<float>(gearTexture->height);
    _gear = new LAppSprite(x, y, fWidth, fHeight, gearTexture->id, _spriteShader, device);

    imageName = resourcesPath + PowerImageName;
    LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(imageName, false);
    x = static_cast<float>(width - powerTexture->width * 0.5f);
    y = static_cast<float>(powerTexture->height * 0.5f);
    fWidth = static_cast<float>(powerTexture->width);
    fHeight = static_cast<float>(powerTexture->height);
    _power = new LAppSprite(x, y, fWidth, fHeight, powerTexture->id, _spriteShader, device);

    x = width * 0.5f;
    y = height * 0.5f;
    _renderSprite = new LAppSprite(x, y, static_cast<float>(width), static_cast<float>(height), 0, _modelSpriteShader, device);
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

void LAppView::ResizeSprite()
{
    LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    if (!textureManager)
    {
        return;
    }

    // 描画領域サイズ
    int width, height;
    LAppDelegate::GetInstance()->GetClientSize(width, height);

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    if(_back)
    {
        Csm::csmUint64 id = _back->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if(texInfo)
        {
            x = width * 0.5f;
            y = height * 0.5f;
            fWidth = static_cast<float>(texInfo->width * 2);
            fHeight = static_cast<float>(height) * 0.95f;
            _back->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_power)
    {
        Csm::csmUint64 id = _power->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _power->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_gear)
    {
        Csm::csmUint64 id = _gear->GetTextureId();
        LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
        if (texInfo)
        {
            x = static_cast<float>(width - texInfo->width * 0.5f);
            y = static_cast<float>(height - texInfo->height * 0.5f);
            fWidth = static_cast<float>(texInfo->width);
            fHeight = static_cast<float>(texInfo->height);
            _gear->ResetRect(x, y, fWidth, fHeight);
        }
    }

    if (_renderSprite)
    {
        x = width * 0.5f;
        y = height * 0.5f;
        _renderSprite->ResetRect(x, y, static_cast<float>(width), static_cast<float>(height));
    }
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
        if (_gear->IsHit(px, py, width, height))
        {
            live2DManager->NextScene();
        }

        // 電源ボタンにタップしたか
        if (_power->IsHit(px, py, width, height))
        {
            LAppDelegate::GetInstance()->AppEnd();
        }
    }
}

void LAppView::PreModelDraw(LAppModel& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_D3D11* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

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
        useTarget->BeginDraw(LAppDelegate::GetInstance()->GetD3dContext());
        useTarget->Clear(LAppDelegate::GetInstance()->GetD3dContext(), _clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    }
}

void LAppView::PostModelDraw(LAppModel& refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_D3D11* useTarget = NULL;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw(LAppDelegate::GetInstance()->GetD3dContext());

        // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            // スプライト描画
            int width, height;
            LAppDelegate::GetInstance()->GetClientSize(width, height);

            float alpha = GetSpriteAlpha(0);
            _renderSprite->SetColor(alpha, alpha, alpha, alpha);
            _renderSprite->RenderImmidiate(width, height, useTarget->GetTextureView(), LAppDelegate::GetInstance()->GetD3dContext());
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

void LAppView::DestroyOffscreenSurface()
{
    _renderBuffer.DestroyOffscreenSurface();
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
