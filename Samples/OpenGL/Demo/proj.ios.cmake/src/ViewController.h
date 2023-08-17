/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>
#import "LAppModel.h"

@interface ViewController : GLKViewController <GLKViewDelegate>

typedef NS_ENUM(NSUInteger, SelectTarget)
{
    SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
    SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
    SelectTarget_ViewFrameBuffer,     ///< LAppViewの持つフレームバッファにレンダリング
};

@property (nonatomic, assign) bool mOpenGLRun;
@property (nonatomic) GLuint vertexBufferId;
@property (nonatomic) GLuint fragmentBufferId;
@property (nonatomic) GLuint programId;

@property (nonatomic) bool anotherTarget;
@property (nonatomic) Csm::Rendering::CubismOffscreenSurface_OpenGLES2 renderBuffer;

@property (nonatomic) float spriteColorR;
@property (nonatomic) float spriteColorG;
@property (nonatomic) float spriteColorB;
@property (nonatomic) float spriteColorA;
@property (nonatomic) float clearColorR;
@property (nonatomic) float clearColorG;
@property (nonatomic) float clearColorB;
@property (nonatomic) float clearColorA;
@property (nonatomic) SelectTarget renderTarget;

/**
 * @brief 解放処理
 */
- (void)dealloc;

/**
 * @brief 解放する。
 */
- (void)releaseView;

/**
 * @brief 画像の初期化を行う。
 */
- (void)initializeSprite;

/**
 * @brief X座標をView座標に変換する。
 *
 * @param[in]       deviceX            デバイスX座標
 */
- (float)transformViewX:(float)deviceX;

/**
 * @brief Y座標をView座標に変換する。
 *
 * @param[in]       deviceY            デバイスY座標
 */
- (float)transformViewY:(float)deviceY;

/**
 * @brief X座標をScreen座標に変換する。
 *
 * @param[in]       deviceX            デバイスX座標
 */
- (float)transformScreenX:(float)deviceX;

/**
 * @brief Y座標をScreen座標に変換する。
 *
 * @param[in]       deviceY            デバイスY座標
 */
- (float)transformScreenY:(float)deviceY;

/**
 * @brief   モデル1体を描画する直前にコールされる
 */
- (void)PreModelDraw:(LAppModel&) refModel;

/**
 * @brief   モデル1体を描画した直後にコールされる
 */
- (void)PostModelDraw:(LAppModel&) refModel;

/**
 * @brief   別レンダリングターゲットにモデルを描画するサンプルで
 *           描画時のαを決定する
 */
- (float)GetSpriteAlpha:(int)assign;

/**
 * @brief レンダリング先を切り替える
 */
- (void)SwitchRenderingTarget:(SelectTarget) targetType;

/**
 * @brief レンダリング先をデフォルト以外に切り替えた際の背景クリア色設定
 * @param[in]   r   赤(0.0~1.0)
 * @param[in]   g   緑(0.0~1.0)
 * @param[in]   b   青(0.0~1.0)
 */
- (void)SetRenderTargetClearColor:(float)r g:(float)g b:(float)b;

@end
