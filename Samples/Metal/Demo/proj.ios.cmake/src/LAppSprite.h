/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#ifndef LAppSprite_h
#define LAppSprite_h

#import "ViewController.h"

@interface LAppSprite : NSObject
@property (nonatomic, readonly, getter=GetTextureId) id <MTLTexture> texture; // テクスチャ
@property (nonatomic) float spriteColorR;
@property (nonatomic) float spriteColorG;
@property (nonatomic) float spriteColorB;
@property (nonatomic) float spriteColorA;
@property (strong, nonatomic) id <MTLRenderPipelineState> pipelineState;
/**
 * @brief Rect 構造体。
 */
typedef struct
{
    float left;     ///< 左辺
    float right;    ///< 右辺
    float up;       ///< 上辺
    float down;     ///< 下辺
}SpriteRect;

/**
 * @brief 初期化
 *
 * @param[in]       x            x座標
 * @param[in]       y            y座標
 * @param[in]       width        横幅
 * @param[in]       height       高さ
 * @param[in]       maxWidth     最大幅
 * @param[in]       maxHeight    最大高さ
 * @param[in]       texture      テクスチャ
 */
- (id)initWithMyVar:(float)x Y:(float)y Width:(float)width Height:(float)height
                    MaxWidth:(float)maxWidth MaxHeight:(float)maxHeight Texture:(id <MTLTexture>) texture;

/**
 * @brief 解放処理
 */
- (void)dealloc;

/**
 * @brief 描画する
 */
- (void)renderImmidiate:(id<MTLRenderCommandEncoder>)renderEncoder;

/**
 * @brief 画面サイズ変更処理
 *
 * @param[in]       x            x座標
 * @param[in]       y            y座標
 * @param[in]       width        横幅
 * @param[in]       height       高さ
 * @param[in]       maxWidth     最大幅
 * @param[in]       maxHeight    最大高さ
 */
- (void)resizeImmidiate:(float)x Y:(float)y Width:(float)width Height:(float)height MaxWidth:(float)maxWidth MaxHeight:(float)maxHeight;

/**
 * @brief コンストラクタ
 *
 * @param[in]       pointX    x座標
 * @param[in]       pointY    y座標
 */
- (bool)isHit:(float)pointX PointY:(float)pointY;

/**
 * @brief 色設定
 *
 * @param[in]       r       赤
 * @param[in]       g       緑
 * @param[in]       b       青
 * @param[in]       a       α
 */
- (void)SetColor:(float)r g:(float)g b:(float)b a:(float)a;

/**
 * @brief MTLRenderPipelineDescriptor設定
 */
- (void)SetMTLRenderPipelineDescriptor:(id <MTLDevice>)device vertexProgram:(id <MTLFunction>)vertexProgram fragmentProgram:(id <MTLFunction>)fragmentProgram;

/**
 * @brief MTLRenderPipelineState設定
 */
- (void)SetMTLRenderPipelineState:(id <MTLDevice>)device pipelineDescriptor:(MTLRenderPipelineDescriptor*)pipelineDescriptor;

@end

#endif /* LAppSprite_h */
