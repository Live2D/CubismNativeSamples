/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "MetalUIView.h"
#import "MetalConfig.h"

@implementation MetalUIView
{
    CADisplayLink *_displayLink;

#if !RENDER_ON_MAIN_THREAD
    // Secondary thread containing the render loop
    NSThread *_renderThread;

    // Flag to indcate rendering should cease on the main thread
    BOOL _continueRunLoop;
#endif
}

///////////////////////////////////////
#pragma mark - Initialization and Setup
///////////////////////////////////////

+ (Class) layerClass
{
    return [CAMetalLayer class];
}

- (void)didMoveToWindow
{
    [super didMoveToWindow];

#if ANIMATION_RENDERING
    if (self.window == nil)
    {
        // If moving off of a window destroy the display link.
        [_displayLink invalidate];
        _displayLink = nil;
        return;
    }

    [self setupCADisplayLinkForScreen:self.window.screen];

#if RENDER_ON_MAIN_THREAD

    // CADisplayLink callbacks are associated with an 'NSRunLoop'. The currentRunLoop is the
    // the main run loop (since 'didMoveToWindow' is always executed from the main thread.
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

#else // IF !RENDER_ON_MAIN_THREAD

    // Protect _continueRunLoop with a `@synchronized` block since it is accessed by the seperate
    // animation thread
    @synchronized(self)
    {
        // Stop animation loop allowing the loop to complete if it's in progress.
        _continueRunLoop = NO;
    }

    // Create and start a secondary NSThread which will have another run runloop.  The NSThread
    // class will call the 'runThread' method at the start of the secondary thread's execution.
    _renderThread =  [[NSThread alloc] initWithTarget:self selector:@selector(runThread) object:nil];
    _continueRunLoop = YES;
    [_renderThread start];

#endif // END !RENDER_ON_MAIN_THREAD
#endif // ANIMATION_RENDERING

    // Perform any actions which need to know the size and scale of the drawable.  When UIKit calls
    // didMoveToWindow after the view initialization, this is the first opportunity to notify
    // components of the drawable's size
#if AUTOMATICALLY_RESIZE
    [self resizeDrawable:self.window.screen.nativeScale];
#else
    // Notify delegate of default drawable size when it can be calculated
    CGSize defaultDrawableSize = self.bounds.size;
    defaultDrawableSize.width *= self.layer.contentsScale;
    defaultDrawableSize.height *= self.layer.contentsScale;
    [self.delegate drawableResize:defaultDrawableSize];
#endif
}

//////////////////////////////////
#pragma mark - Render Loop Control
//////////////////////////////////

#if ANIMATION_RENDERING

- (void)setPaused:(BOOL)paused
{
    super.paused = paused;

    _displayLink.paused = paused;
}

- (void)setupCADisplayLinkForScreen:(UIScreen*)screen
{
    [self stopRenderLoop];

    _displayLink = [screen displayLinkWithTarget:self selector:@selector(render)];

    _displayLink.paused = self.paused;

    _displayLink.preferredFramesPerSecond = 60;
}

- (void)stopRenderLoop
{
    [_displayLink invalidate];
}

#if !RENDER_ON_MAIN_THREAD
- (void)runThread
{
    // Set the display link to the run loop of this thread so its call back occurs on this thread
    NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
    [_displayLink addToRunLoop:runLoop forMode:@"MetalDisplayLinkMode"];

    // The '_continueRunLoop' ivar is set outside this thread, so it must be synchronized.
    // Create a 'continueRunLoop' local var that can be set from the _continueRunLoop ivar in a @synchronized block
    BOOL continueRunLoop = YES;

    // Begin the run loop
    while (continueRunLoop)
    {
        // Create autorelease pool for the current iteration of loop.
        @autoreleasepool
        {
            // Run the loop once accepting input only from the display link.
            [runLoop runMode:@"MetalDisplayLinkMode" beforeDate:[NSDate distantFuture]];
        }

        // Synchronize this with the _continueRunLoop ivar which is set on another thread
        @synchronized(self)
        {
            // Anything accessed outside the thread such as the '_continueRunLoop' ivar
            // is read inside the synchronized block to ensure it is fully/atomically written
            continueRunLoop = _continueRunLoop;
        }
    }
}
#endif // END !RENDER_ON_MAIN_THREAD

#endif // END ANIMATION_RENDERING

///////////////////////
#pragma mark - Resizing
///////////////////////

#if AUTOMATICALLY_RESIZE

// Override all methods which indicate the view's size has changed

- (void)setContentScaleFactor:(CGFloat)contentScaleFactor
{
    [super setContentScaleFactor:contentScaleFactor];
    [self resizeDrawable:self.window.screen.nativeScale];
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    [self resizeDrawable:self.window.screen.nativeScale];
}

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    [self resizeDrawable:self.window.screen.nativeScale];
}

- (void)setBounds:(CGRect)bounds
{
    [super setBounds:bounds];
    [self resizeDrawable:self.window.screen.nativeScale];
}

#endif // END AUTOMATICALLY_RESIZE

@end
