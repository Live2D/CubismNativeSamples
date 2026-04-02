/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <UIKit/UIKit.h>

@class MinViewController;
@class MinLAppTextureManager;

@interface MinSceneDelegate : UIResponder <UIWindowSceneDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) MinViewController *viewController;
@property (nonatomic, readonly, getter=getTextureManager) MinLAppTextureManager *textureManager;

/**
 * @brief   スプライトを初期化する。
 */
- (void)initializeSprite;

@end
