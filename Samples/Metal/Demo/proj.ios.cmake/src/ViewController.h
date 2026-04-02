/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <UIKit/UIKit.h>
#import "LAppModel.h"
#import "MetalView.h"

@class SceneDelegate;

@interface ViewController : UIViewController <MetalViewDelegate>

@property (nonatomic) SceneDelegate *sceneDelegate;
@property (nonatomic) bool anotherTarget;
@property (nonatomic) float spriteColorR;
@property (nonatomic) float spriteColorG;
@property (nonatomic) float spriteColorB;
@property (nonatomic) float spriteColorA;
@property (nonatomic) float clearColorR;
@property (nonatomic) float clearColorG;
@property (nonatomic) float clearColorB;
@property (nonatomic) float clearColorA;
@property (nonatomic) id<MTLCommandQueue> commandQueue;
@property (nonatomic) id<MTLTexture> depthTexture;

/**
 * @brief 解放処理
 */
- (void)dealloc;

/**
 * @brief 解放する。
 */
- (void)releaseView;

/**
 * @brief 画面リサイズ処理
 */
- (void)resizeScreen;

/**
 * @brief 画像の初期化を行う。
 */
- (void)initializeSprite;

/**
 * @brief 画像のリサイズ処理
 *
 * @param[in]       width            幅
 * @param[in]       height           高さ
 */
- (void)resizeSprite:(float)width height:(float)height;

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
 * @brief デバイスを取得する。
 *
 * @return  デバイスを返す
 */
- (id <MTLDevice>)getDevice;

/**
 * @brief   ウインドウの幅を取得する
 *
 * @return  ウィンドウの幅
 */
- (int)getWindowWidth;

/**
 * @brief   ウインドウの高さを取得する
 *
 * @return  ウィンドウの高さ
 */
- (int)getWindowHeight;

/**
 * @brief   セーフエリア内のビューポートを取得する
 *
 * @return   セーフエリア内のビューポート
 */
- (MTLViewport) getSafeAreaViewport;

@end
