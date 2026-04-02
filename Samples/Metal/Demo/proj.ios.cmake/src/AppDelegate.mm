/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "AppDelegate.h"
#import "ViewController.h"
#import "LAppAllocator.h"
#import <iostream>
#import "LAppPal.h"
#import "LAppDefine.h"
#import "LAppLive2DManager.h"

@interface AppDelegate ()

@property (nonatomic) LAppAllocator cubismAllocator; // Cubism SDK Allocator
@property (nonatomic) Csm::CubismFramework::Option cubismOption; // Cubism SDK Option
@property (nonatomic) bool isEnd; // APPを終了しているか

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initializeCubism];
    return YES;
}

- (UISceneConfiguration *)application:(UIApplication *)application
        configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession
                                      options:(UISceneConnectionOptions *)options {
    UISceneConfiguration *config = [[UISceneConfiguration alloc]
                                    initWithName:@"Default Configuration"
                                    sessionRole:connectingSceneSession.role];
    config.delegateClass = [SceneDelegate class];
    return config;
}

- (void)initializeCubism
{
    _cubismOption.LogFunction = LAppPal::PrintMessageLn;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = LAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = LAppPal::ReleaseBytes;

    Csm::CubismFramework::StartUp(&_cubismAllocator,&_cubismOption);

    Csm::CubismFramework::Initialize();

    Csm::CubismMatrix44 projection;

    LAppPal::UpdateTime();

}

- (bool)getIsEnd
{
    return _isEnd;
}

- (void)finishApplication
{
    // アクティブなSceneを取得
    UIWindowScene *activeScene = nil;
    for (UIScene *scene in [UIApplication sharedApplication].connectedScenes)
    {
        if (scene.activationState == UISceneActivationStateForegroundActive)
        {
            activeScene = (UIWindowScene *)scene;
            break;
        }
    }

    if (activeScene)
    {
        SceneDelegate *sceneDelegate = (SceneDelegate *)activeScene.delegate;
        [sceneDelegate.viewController release];
        sceneDelegate.viewController = nil;
        [sceneDelegate.window release];
        sceneDelegate.window = nil;
    }

    [LAppLive2DManager releaseInstance];

    Csm::CubismFramework::Dispose();

    _isEnd = true;

    exit(0);
}

- (SceneDelegate*)getActiveSceneDelegate
{
    // UIApplicationSupportsMultipleScenesがfalseなので、シーンは1つのみ
    for (UIScene *scene in [UIApplication sharedApplication].connectedScenes)
    {
        if ([scene.delegate isKindOfClass:[SceneDelegate class]])
        {
            return (SceneDelegate *)scene.delegate;
        }
    }
    return nil;
}

@end
