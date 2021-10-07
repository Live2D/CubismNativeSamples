/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#ifndef MinLAppLive2DManager_h
#define MinLAppLive2DManager_h

#import <string>
#import <CubismFramework.hpp>
#import <Math/CubismMatrix44.hpp>
#import <Type/csmVector.hpp>
#import "MinLAppModel.h"

@interface MinLAppLive2DManager : NSObject

@property (nonatomic) Csm::CubismMatrix44 *viewMatrix; //モデル描画に用いるView行列
@property (nonatomic) MinLAppModel* model; //モデルインスタンス
@property (nonatomic) std::string currentModelDictionary; //現在のモデルディレクトリ

/**
 * @brief クラスのインスタンスを返す。
 *        インスタンスが生成されていない場合は内部でインスタンスを生成する。
 */
+ (MinLAppLive2DManager*)getInstance;

/**
 * @brief クラスのインスタンスを解放する。
 */
+ (void)releaseInstance;

/**
 * @brief モデルを返す。
 * @return モデルのインスタンスを返す。。
 */
- (MinLAppModel*)getModel;

/**
 * @breif 現在のシーンで保持している全てのモデルを解放する
 */
- (void)releaseModel;

/**
 * @brief   画面をドラッグしたときの処理
 *
 * @param[in]   x   画面のX座標
 * @param[in]   y   画面のY座標
 */
- (void)onDrag:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y;

/**
 * @brief   画面をタップしたときの処理
 *
 * @param[in]   x   画面のX座標
 * @param[in]   y   画面のY座標
 */
- (void)onTap:(Csm::csmFloat32)x floatY:(Csm::csmFloat32)y;

/**
 * @brief   画面を更新するときの処理
 *          モデルの更新処理および描画処理を行う
 */
- (void)onUpdate;

/**
 * @brief   viewMatrixをセットする
 */
- (void)SetViewMatrix:(Csm::CubismMatrix44*)m;

/**
 * @brief   パスの設定
 *
 * @param[in] path モデルのディレクトリパス
 */
- (void)SetAssetDirectry:(const std::string)path;

/**
 * @brief   モデルのロード処理
 */
- (void)LoadModel;

@end


#endif /* MinLAppLive2DManager_h */
