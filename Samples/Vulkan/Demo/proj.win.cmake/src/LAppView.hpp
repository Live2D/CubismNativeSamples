/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Rendering/Vulkan/CubismOffscreenSurface_Vulkan.hpp>
#include <Rendering/Vulkan/CubismRenderer_Vulkan.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"

#include "LAppView_Common.hpp"

class TouchManager_Common;
class LAppSprite;
class LAppModel;
class LAppSpritePipeline;
class LAppModelSpritePipeline;

/**
* @brief 描画クラス
*/
class LAppView : public LAppView_Common
{
public:
    /**
     * @brief LAppModelのレンダリング先
     */
    enum SelectTarget
    {
        SelectTarget_None,
        ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,
        ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,
        ///< LAppViewの持つフレームバッファにレンダリング
    };

    /**
    * @brief コンストラクタ
    */
    LAppView();

    /**
    * @brief デストラクタ
    */
    ~LAppView();

    /**
    * @brief 初期化する。
    */
    virtual void Initialize(int width, int height) override;

    /**
    * @brief レンダリングを開始する。
    * @param[in]  commandBuffer    -> コマンドバッファ
    * @param[in]  r                -> r
    * @param[in]  g                -> g
    * @param[in]  b                -> b
    * @param[in]  a                -> a
    * @param[in]  isClear                -> クリアするかのフラグ
    */
    void BeginRendering(VkCommandBuffer commandBuffer, float r, float g, float b, float a, bool isClear);

    /**
    * @brief レンダリングを終了する際のレイアウトを変更する。
    *
    * @param[in]  commandBuffer    -> コマンドバッファ
    */
    void ChangeEndLayout(VkCommandBuffer commandBuffer);

    /**
    * @brief レンダリングを終了する。
    *
    * @param[in]  commandBuffer    -> コマンドバッファ
    */
    void EndRendering(VkCommandBuffer commandBuffer);

    /**
    * @brief 描画する。
    */
    void Render();

    /**
    * @brief 画像の初期化を行う。
    */
    void InitializeSprite();

    /**
     * @brief タッチされたときに呼ばれる。
     *
     * @param[in]       pointX            スクリーンX座標
     * @param[in]       pointY            スクリーンY座標
     */
    void OnTouchesBegan(float pointX, float pointY) const;

    /**
     * @brief タッチしているときにポインタが動いたら呼ばれる。
     *
     * @param[in]       pointX            スクリーンX座標
     * @param[in]       pointY            スクリーンY座標
     */
    void OnTouchesMoved(float pointX, float pointY) const;

    /**
     * @brief タッチが終了したら呼ばれる。
     *
     * @param[in]       pointX            スクリーンX座標
     * @param[in]       pointY            スクリーンY座標
     */
    void OnTouchesEnded(float pointX, float pointY) const;

    /**
     * @brief   モデル1体を描画した直後にコールされる
     *
     * @param[in]       refModel            モデルのインスタンス
     */
    void PreModelDraw(LAppModel& refModel);

    /**
     * @brief   モデル1体を描画した直後にコールされる
     *
     * @param[in]       refModel            モデルのインスタンス
     * @param[in]       modelIndex          モデルのインデックス
     */
    void PostModelDraw(LAppModel& refModel, Csm::csmInt32 modelIndex);

    /**
     * @brief レンダリング先を切り替える
     *
     * @param[in]       targetType          ターゲットタイプ
     */
    void SwitchRenderingTarget(SelectTarget targetType);

    /**
     * @brief レンダリング先をデフォルト以外に切り替えた際の背景クリア色設定
     *
     * @param[in]   r   赤(0.0~1.0)
     * @param[in]   g   緑(0.0~1.0)
     * @param[in]   b   青(0.0~1.0)
     */
    void SetRenderTargetClearColor(float r, float g, float b);

    /**
     * @brief   別レンダリングターゲットにモデルを描画するサンプルで
     *           描画時のαを決定する
     */
    float GetSpriteAlpha(int assign) const;

    /**
     * @brief ウィンドウサイズ変更の際にスプライトを再作成する
     *
     * @param[in]       width            横幅
     * @param[in]       height           縦幅
     */
    void ResizeSprite(int width, int height);

    /**
     * @brief オフスクリーンの破棄
     */
    void DestroyOffscreenSurface();

private:
    TouchManager_Common* _touchManager;                          ///< タッチマネージャー
    LAppSprite* _back;                                           ///< 背景画像
    LAppSprite* _gear;                                           ///< ギア画像
    LAppSprite* _power;                                          ///< 電源画像
    LAppSprite* _renderSprite;                                   ///< レンダリング先を別ターゲットにする方式の場合に使用
    Csm::Rendering::CubismOffscreenSurface_Vulkan _renderBuffer; ///< モードによってはCubismモデル結果をこっちにレンダリング
    SelectTarget _renderTarget;                                  ///< レンダリング先の選択肢
    float _clearColor[4];                                        ///< レンダリングターゲットのクリアカラー
    LAppSpritePipeline* _spritePipeline;                         ///< スプライト用パイプライン
    LAppModelSpritePipeline* _modelSpritePipeline;               ///< モデルスプライト用パイプライン
};
