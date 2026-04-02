/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "SceneDelegate.h"
#import "ViewController.h"
#import "LAppTextureManager.h"
#import "LAppLive2DManager.h"

@interface SceneDelegate ()

@property (nonatomic, readwrite) LAppTextureManager *textureManager;
@property (nonatomic) Csm::csmInt32 sceneIndex;

@end

@implementation SceneDelegate

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions
{
    UIWindowScene *windowScene = (UIWindowScene *)scene;

    _textureManager = [[LAppTextureManager alloc] init];

    self.window = [[UIWindow alloc] initWithWindowScene:windowScene];

    self.viewController = [[ViewController alloc] initWithNibName:nil bundle:nil];

    self.viewController.sceneDelegate = self;

    self.window.rootViewController = self.viewController;

    [self.window makeKeyAndVisible];
}


// シーンが切断された時の処理
- (void)sceneDidDisconnect:(UIScene *)scene
{
}


// シーンがアクティブになった時の処理
- (void)sceneDidBecomeActive:(UIScene *)scene
{
}


// シーンが非アクティブになる時の処理
- (void)sceneWillResignActive:(UIScene *)scene
{
}


// フォアグラウンドに入る時の処理
- (void)sceneWillEnterForeground:(UIScene *)scene
{
    self.viewController.mOpenGLRun = true;

    // シーンを復元
    [[LAppLive2DManager getInstance] changeScene:_sceneIndex];
}


// バックグラウンドに入る時の処理
- (void)sceneDidEnterBackground:(UIScene *)scene
{
    self.viewController.mOpenGLRun = false;

    // シーンインデックスを保存
    _sceneIndex = [[LAppLive2DManager getInstance] sceneIndex];
}


- (void)initializeSprite
{
    [self.viewController initializeSprite];
}


- (LAppTextureManager *)getTextureManager
{
    return _textureManager;
}

@end
