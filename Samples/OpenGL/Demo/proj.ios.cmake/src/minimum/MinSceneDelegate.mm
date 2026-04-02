/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "MinSceneDelegate.h"
#import "MinViewController.h"
#import "MinLAppTextureManager.h"
#import "MinLAppLive2DManager.h"

@interface MinSceneDelegate ()

@property (nonatomic, readwrite) MinLAppTextureManager *textureManager;

@end

@implementation MinSceneDelegate

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions
{
    UIWindowScene *windowScene = (UIWindowScene *)scene;

    _textureManager = [[MinLAppTextureManager alloc] init];

    self.window = [[UIWindow alloc] initWithWindowScene:windowScene];

    self.viewController = [[MinViewController alloc] initWithNibName:nil bundle:nil];

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

    // テクスチャマネージャーを再作成
    _textureManager = [[MinLAppTextureManager alloc] init];

    // スプライトを再初期化
    [self initializeSprite];

    // モデルを読み込み
    [[MinLAppLive2DManager getInstance] init];
}


// バックグラウンドに入る時の処理
- (void)sceneDidEnterBackground:(UIScene *)scene
{
    self.viewController.mOpenGLRun = false;

    // テクスチャマネージャーを解放
    _textureManager = nil;

    // モデルを解放
    [[MinLAppLive2DManager getInstance] releaseModel];
}


- (void)initializeSprite
{
    [self.viewController initializeSprite];
}


- (MinLAppTextureManager *)getTextureManager
{
    return _textureManager;
}

@end
