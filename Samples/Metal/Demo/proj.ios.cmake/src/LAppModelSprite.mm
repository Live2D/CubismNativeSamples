/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import "LAppModelSprite.h"
#import <Foundation/Foundation.h>
#import "LAppDefine.h"
#import "LAppPal.h"
#import <CubismFramework.hpp>
#import <Rendering/Metal/CubismRenderer_Metal.hpp>
#import "Rendering/Metal/CubismRenderingInstanceSingleton_Metal.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))


@implementation LAppModelSprite

- (void)SetMTLRenderPipelineDescriptor:(id <MTLDevice>)device vertexProgram:(id <MTLFunction>)vertexProgram fragmentProgram:(id <MTLFunction>)fragmentProgram
{
    MTLRenderPipelineDescriptor* pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    //パイプライン・ステート・オブジェクトを作成するパイプライン・ステート・ディスクリプターの作成

    //デバッグ時に便利
    pipelineDescriptor.label                           = @"SpritePipeline";
    // Vertexステージで実行する関数を指定する
    pipelineDescriptor.vertexFunction                  = vertexProgram;
    // Fragmentステージで実行する関数を指定する
    pipelineDescriptor.fragmentFunction                = fragmentProgram;
    pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipelineDescriptor.colorAttachments[0].blendingEnabled = true;
    pipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    pipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
    pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
    pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

    [super SetMTLRenderPipelineState:device pipelineDescriptor:pipelineDescriptor];
    [pipelineDescriptor release];
}

@end

