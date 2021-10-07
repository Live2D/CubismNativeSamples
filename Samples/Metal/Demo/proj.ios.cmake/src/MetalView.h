/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <UIKit/UIKit.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import "MetalConfig.h"

// Protocol to provide resize and redraw callbacks to a delegate
@protocol MetalViewDelegate <NSObject>

- (void)drawableResize:(CGSize)size;

- (void)renderToMetalLayer:(nonnull CAMetalLayer *)metalLayer;

@end

// Metal view base class
@interface MetalView : UIView <CALayerDelegate>

@property (nonatomic, nonnull, readonly) CAMetalLayer *metalLayer;

@property (nonatomic, getter=isPaused) BOOL paused;

@property (nonatomic, nullable) id<MetalViewDelegate> delegate;

- (void)initCommon;

#if AUTOMATICALLY_RESIZE
- (void)resizeDrawable:(CGFloat)scaleFactor;
#endif

#if ANIMATION_RENDERING
- (void)stopRenderLoop;
#endif

@end
