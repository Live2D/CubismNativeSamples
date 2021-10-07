/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#ifndef MinLAppSprite_h
#define MinLAppSprite_h

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "MinViewController.h"

@interface MinLAppSprite : NSObject
@property (strong, nonatomic) GLKBaseEffect *baseEffect;
@property (nonatomic, readonly, getter=GetTextureId) GLuint textureId; // テクスチャID
@property (nonatomic) float spriteColorR;
@property (nonatomic) float spriteColorG;
@property (nonatomic) float spriteColorB;
@property (nonatomic) float spriteColorA;

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
 * @param[in]       textureId    テクスチャID
 */
- (id)initWithMyVar:(float)x Y:(float)y Width:(float)width Height:(float)height TextureId:(GLuint) textureId;

/**
 * @brief 描画する
 *
 * @param[in]     vertexBufferID    フラグメントシェーダID
 * @param[in]     fragmentBufferID  バーテックスシェーダID
 */
- (void)render:(GLuint)vertexBufferID fragmentBufferID:(GLuint)fragmentBufferID;


/**
 * @brief 描画する
 *
 * @param[in]     vertexBufferID    フラグメントシェーダID
 * @param[in]     fragmentBufferID  バーテックスシェーダID
 */
- (void)renderImmidiate:(GLuint)vertexBufferID fragmentBufferID:(GLuint)fragmentBufferID TextureId:(GLuint) textureId uvArray:(float *)uvArray;


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

@end

#endif /* MinLAppSprite_h */
