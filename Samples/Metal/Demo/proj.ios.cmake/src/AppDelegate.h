/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <UIKit/UIKit.h>

@class ViewController;
@class LAppView;
@class LAppTextureManager;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) ViewController *viewController;
@property (nonatomic, readonly, getter=getTextureManager) LAppTextureManager *textureManager; // テクスチャマネージャー

/**
 * @brief   Cubism SDK の初期化
 */
- (void)initializeCubism;

/**
 * @brief   アプリケーションを終了するかどうか。
 */
- (bool)getIsEnd;

/**
 * @brief   アプリケーションを終了する。
 */
- (void)finishApplication;

@end

