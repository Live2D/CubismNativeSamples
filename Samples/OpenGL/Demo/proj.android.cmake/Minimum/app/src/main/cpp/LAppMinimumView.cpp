/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppMinimumView.hpp"
#include <cmath>
#include <string>
#include "LAppPal.hpp"
#include "LAppMinimumDelegate.hpp"
#include "LAppDefine.hpp"
#include "LAppTextureManager.hpp"
#include "LAppMinimumLive2DManager.hpp"
#include "TouchManager_Common.hpp"
#include "LAppSprite.hpp"
#include "LAppSpriteShader.hpp"
#include "LAppMinimumModel.hpp"

#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>

#include "JniBridgeC.hpp"

using namespace std;
using namespace LAppDefine;
using namespace Csm;

LAppMinimumView::LAppMinimumView()
    : LAppView_Common()
    , _renderSprite(nullptr)
    , _renderTarget(SelectTarget_None)
{
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager_Common();
}

LAppMinimumView::~LAppMinimumView()
{
    _renderBuffer.DestroyOffscreenSurface();
    delete _spriteShader;
    delete _renderSprite;
    delete _touchManager;
}

void LAppMinimumView::Initialize(int width, int height)
{
    LAppView_Common::Initialize(width, height);

    // シェーダー作成委譲クラス
    _spriteShader = new LAppSpriteShader();
}

void LAppMinimumView::InitializeSprite()
{
    GLuint programId = _spriteShader->GetShaderId();
    int width = LAppMinimumDelegate::GetInstance()->GetWindowWidth();
    int height = LAppMinimumDelegate::GetInstance()->GetWindowHeight();

    // 画面全体を覆うサイズ
    float x = width * 0.5f;
    float y = height * 0.5f;

    if (!_renderSprite)
    {
        _renderSprite = new LAppSprite(x, y, width, height, 0, programId);
    }
    else
    {
        _renderSprite->ReSize(x, y, width, height);
    }
}

void LAppMinimumView::Render()
{
    LAppMinimumLive2DManager* Live2DManager = LAppMinimumLive2DManager::GetInstance();

    //Live2DManager->SetViewMatrix(_viewMatrix);

    // Cubism更新・描画
    Live2DManager->OnUpdate();

    // 各モデルが持つ描画ターゲットをテクスチャとする場合
    if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    {
        const GLfloat uvVertex[] =
                {
                        1.0f, 1.0f,
                        0.0f, 1.0f,
                        0.0f, 0.0f,
                        1.0f, 0.0f,
                };

        float alpha = GetSpriteAlpha(2); // サンプルとしてαに適当な差をつける
        _renderSprite->SetColor(1.0f * alpha, 1.0f * alpha, 1.0f * alpha, alpha);

        LAppMinimumModel *model = Live2DManager->GetModel();
        if (model)
        {
            // 画面サイズを取得する
            int maxWidth = LAppMinimumDelegate::GetInstance()->GetWindowWidth();
            int maxHeight = LAppMinimumDelegate::GetInstance()->GetWindowHeight();
            _renderSprite->SetWindowSize(maxWidth, maxHeight);

            _renderSprite->RenderImmidiate(model->GetRenderBuffer().GetColorBuffer(), uvVertex);
        }
    }
}

void LAppMinimumView::OnTouchesBegan(float pointX, float pointY) const
{
    _touchManager->TouchesBegan(pointX, pointY);
}

void LAppMinimumView::OnTouchesMoved(float pointX, float pointY) const
{
    float viewX = this->TransformViewX(_touchManager->GetX());
    float viewY = this->TransformViewY(_touchManager->GetY());

    _touchManager->TouchesMoved(pointX, pointY);

    LAppMinimumLive2DManager::GetInstance()->OnDrag(viewX, viewY);
}

void LAppMinimumView::OnTouchesEnded(float pointX, float pointY)
{
    // タッチ終了
    LAppMinimumLive2DManager* live2DManager = LAppMinimumLive2DManager::GetInstance();
    live2DManager->OnDrag(0.0f, 0.0f);
    {

        // シングルタップ
        float x = _deviceToScreen->TransformX(_touchManager->GetX()); // 論理座標変換した座標を取得。
        float y = _deviceToScreen->TransformY(_touchManager->GetY()); // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable)
        {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }
    }
}

void LAppMinimumView::PreModelDraw(LAppMinimumModel &refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_OpenGLES2* useTarget = nullptr;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        if (!useTarget->IsValid())
        {// 描画ターゲット内部未作成の場合はここで作成
            int width = LAppMinimumDelegate::GetInstance()->GetWindowWidth();
            int height = LAppMinimumDelegate::GetInstance()->GetWindowHeight();

            // モデル描画キャンバス
            useTarget->CreateOffscreenSurface(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
        }

        // レンダリング開始
        useTarget->BeginDraw();
        useTarget->Clear(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    }
}

void LAppMinimumView::PostModelDraw(LAppMinimumModel &refModel)
{
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_OpenGLES2* useTarget = nullptr;

    if (_renderTarget != SelectTarget_None)
    {// 別のレンダリングターゲットへ向けて描画する場合

        //透過設定
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        // 使用するターゲット
        useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

        // レンダリング終了
        useTarget->EndDraw();

        // LAppMinimumViewの持つフレームバッファを使うなら、スプライトへの描画はここ
        if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
        {
            const GLfloat uvVertex[] =
                    {
                            1.0f, 1.0f,
                            0.0f, 1.0f,
                            0.0f, 0.0f,
                            1.0f, 0.0f,
                    };

            _renderSprite->SetColor(1.0f * GetSpriteAlpha(0), 1.0f * GetSpriteAlpha(0), 1.0f *
                    GetSpriteAlpha(0) , GetSpriteAlpha(0));

            // 画面サイズを取得する
            int maxWidth = LAppMinimumDelegate::GetInstance()->GetWindowWidth();
            int maxHeight = LAppMinimumDelegate::GetInstance()->GetWindowHeight();
            _renderSprite->SetWindowSize(maxWidth, maxHeight);

            _renderSprite->RenderImmidiate(useTarget->GetColorBuffer(), uvVertex);
        }
    }
}

void LAppMinimumView::SwitchRenderingTarget(SelectTarget targetType)
{
    _renderTarget = targetType;
}

void LAppMinimumView::SetRenderTargetClearColor(float r, float g, float b)
{
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

float LAppMinimumView::GetSpriteAlpha(int assign) const
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
