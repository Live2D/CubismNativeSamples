/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "MinAppDelegate.h"
#import "MinViewController.h"
#import "MinLAppAllocator.h"
#import <iostream>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "MinLAppPal.h"
#import "MinLAppDefine.h"
#import "MinLAppLive2DManager.h"


@interface MinAppDelegate ()

@property (nonatomic) MinLAppAllocator cubismAllocator; // Cubism SDK Allocator
@property (nonatomic) Csm::CubismFramework::Option cubismOption; // Cubism SDK Option
@property (nonatomic) bool isEnd; // APPを終了しているか

@end

@implementation MinAppDelegate


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
    config.delegateClass = [MinSceneDelegate class];
    return config;
}

- (void)initializeCubism
{
    _cubismOption.LogFunction = MinLAppPal::PrintMessage;
    _cubismOption.LoggingLevel = MinLAppDefine::CubismLoggingLevel;
    _cubismOption.LoadFileFunction = MinLAppPal::LoadFileAsBytes;
    _cubismOption.ReleaseBytesFunction = MinLAppPal::ReleaseBytes;

    Csm::CubismFramework::StartUp(&_cubismAllocator,&_cubismOption);

    Csm::CubismFramework::Initialize();

    Csm::CubismMatrix44 projection;

    MinLAppPal::UpdateTime();

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
        MinSceneDelegate *sceneDelegate = (MinSceneDelegate *)activeScene.delegate;
        [sceneDelegate.viewController releaseView];
        sceneDelegate.viewController = nil;
        sceneDelegate.window = nil;
    }

    [MinLAppLive2DManager releaseInstance];

    Csm::CubismFramework::Dispose();

    _isEnd = true;

    exit(0);
}

- (MinSceneDelegate*) getActiveMinSceneDelegate
{
    // UIApplicationSupportsMultipleScenesがfalseなので、シーンは1つのみ
    for (UIScene *scene in [UIApplication sharedApplication].connectedScenes)
    {
        if ([scene.delegate isKindOfClass:[MinSceneDelegate class]])
        {
            return (MinSceneDelegate *)scene.delegate;
        }
    }
    return nil;
}

@end
