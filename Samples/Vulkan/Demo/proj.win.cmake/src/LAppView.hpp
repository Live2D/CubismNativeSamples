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

class TouchManager;
class LAppSprite;
class LAppModel;

/**
* @brief 描画クラス
*/
class LAppView
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
    void Initialize();

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
    * @brief   シェーダーモジュ―ルを作成する
    *
    * @param[in]  device    -> デバイス
    * @param[in]  filename  -> シェーダーのファイル名
    */
    VkShaderModule CreateShaderModule(VkDevice device, std::string filename);

    /**
     * @brief   スプライト用のディスクリプタセットのレイアウトを作成する。
     *
     * @param[in]       device            論理デバイス
     */
    void CreateDescriptorSetLayout(VkDevice device);

    /**
     * @brief スプライト用のパイプラインを作成する。
     *
     * @param[in]       device                  デバイス
     * @param[in]       extent                  フレームバッファのサイズ
     * @param[in]       vertShaderModule        頂点シェーダのモジュール
     * @param[in]       fragShaderModule        フラグメントシェーダのモジュール
     */
    void CreateSpriteGraphicsPipeline(VkDevice device, VkExtent2D extent, VkShaderModule vertShaderModule,
        VkShaderModule fragShaderModule, VkFormat swapchainFormat);

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
    * @brief X座標をView座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformViewX(float deviceX) const;

    /**
    * @brief Y座標をView座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformViewY(float deviceY) const;

    /**
    * @brief X座標をScreen座標に変換する。
    *
    * @param[in]       deviceX            デバイスX座標
    */
    float TransformScreenX(float deviceX) const;

    /**
    * @brief Y座標をScreen座標に変換する。
    *
    * @param[in]       deviceY            デバイスY座標
    */
    float TransformScreenY(float deviceY) const;

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
     * @brief ウィンドウサイズ変更の際にリソースを破棄する。
     *
     * @param[in]       device            デバイス
     */
    void Cleanup(VkDevice device);

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
    TouchManager* _touchManager;                               ///< タッチマネージャー
    Csm::CubismMatrix44* _deviceToScreen;                      ///< デバイスからスクリーンへの行列
    Csm::CubismViewMatrix* _viewMatrix;                        ///< viewMatrix
    LAppSprite* _back;                                         ///< 背景画像
    LAppSprite* _gear;                                         ///< ギア画像
    LAppSprite* _power;                                        ///< 電源画像
    LAppSprite* _renderSprite;                                 ///< レンダリング先を別ターゲットにする方式の場合に使用
    Csm::Rendering::CubismOffscreenSurface_Vulkan _renderBuffer; ///< モードによってはCubismモデル結果をこっちにレンダリング
    SelectTarget _renderTarget;                                ///< レンダリング先の選択肢
    float _clearColor[4];                                      ///< レンダリングターゲットのクリアカラー
    VkShaderModule _vertShaderModule;                          ///< 頂点シェーダーモジュール
    VkShaderModule _fragShaderModule;                          ///< フラグメントシェーダーモジュール
    VkDescriptorSetLayout _descriptorSetLayout;                ///< ディスクリプタセットのレイアウト, UBOが1つとテクスチャが1つずつ
    VkPipelineLayout _pipelineLayout;                          ///< スプライト描画に使うパイプラインのレイアウト
    VkPipeline _pipeline;                                      ///< スプライト描画に使うパイプライン
};
