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
#import "MetalUIView.h"

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

    [self initializeSprite];
}


// シーンが切断された時の処理
- (void)sceneDidDisconnect:(UIScene *)scene
{
    // テクスチャマネージャーを解放
    if (_textureManager)
    {
        [_textureManager release];
        _textureManager = nil;
    }

    // ViewControllerを解放
    if (_viewController)
    {
        [_viewController release];
        _viewController = nil;
    }

    // ウィンドウを解放
    if (_window)
    {
        [_window release];
        _window = nil;
    }
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
    // シーンを復元
    [[LAppLive2DManager getInstance] changeScene:_sceneIndex];

    // 描画を再開
    MetalUIView *view = (MetalUIView*)self.viewController.view;
    [view setPaused:NO];
}


// バックグラウンドに入る時の処理
- (void)sceneDidEnterBackground:(UIScene *)scene
{
    // シーンインデックスを保存
    _sceneIndex = [[LAppLive2DManager getInstance] sceneIndex];

    // 描画を一時停止
    MetalUIView *view = (MetalUIView*)self.viewController.view;
    [view setPaused:YES];
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
