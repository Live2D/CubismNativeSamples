/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#import <Foundation/Foundation.h>
#import "LAppSprite.h"
#import <GLKit/GLKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))


@interface LAppSprite()

@property (nonatomic, readwrite) GLuint textureId; // テクスチャID
@property (nonatomic) SpriteRect rect; // 矩形
@property (nonatomic) GLuint vertexBufferId;
@property (nonatomic) GLuint fragmentBufferId;

@end

@implementation LAppSprite
@synthesize baseEffect;

- (id)initWithMyVar:(float)x Y:(float)y Width:(float)width Height:(float)height TextureId:(GLuint) textureId
{
    self = [super self];

    if(self != nil)
    {
        _rect.left = (x - width * 0.5f);
        _rect.right = (x + width * 0.5f);
        _rect.up = (y + height * 0.5f);
        _rect.down = (y - height * 0.5f);
        _textureId = textureId;

        _spriteColorR = _spriteColorG = _spriteColorB = _spriteColorA = 1.0f;

        self.baseEffect = [[GLKBaseEffect alloc] init];
        self.baseEffect.useConstantColor = GL_TRUE;
        self.baseEffect.constantColor = GLKVector4Make(1.0f, 1.0f, 1.0f, 1.0f);
        self.baseEffect.texture2d0.enabled = GL_TRUE;
    }


    return self;
}

- (void)render:(GLuint)vertexBufferID fragmentBufferID:(GLuint)fragmentBufferID
{
    //描画画像変更
    self.baseEffect.texture2d0.name = _textureId;

    // color
    self.baseEffect.constantColor = GLKVector4Make(_spriteColorR, _spriteColorG, _spriteColorB, _spriteColorA);

    [self.baseEffect prepareToDraw];

    CGRect screenRect = [[UIScreen mainScreen] bounds];
    float maxWidth = screenRect.size.width;
    float maxHeight = screenRect.size.height;

    float positionVertex[] =
    {
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionVertex), positionVertex, GL_STATIC_DRAW);

    // 頂点情報の位置を、頂点処理の変数に指定する（これを用いて描画を行う）
    glEnableVertexAttribArray(GLKVertexAttribPosition);

    // 頂点情報の格納場所と書式を頂点処理に教える
    glVertexAttribPointer(GLKVertexAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, fragmentBufferID);

    const GLfloat uv[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, fragmentBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);

    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // 図形を描く
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

- (void)renderImmidiate:(GLuint)vertexBufferID fragmentBufferID:(GLuint)fragmentBufferID TextureId:(GLuint) textureId uvArray:(float *)uvArray
{
    //描画画像変更
    self.baseEffect.texture2d0.name = textureId;

    // color
    self.baseEffect.constantColor = GLKVector4Make(_spriteColorR, _spriteColorG, _spriteColorB, _spriteColorA);

    [self.baseEffect prepareToDraw];

    CGRect screenRect = [[UIScreen mainScreen] bounds];
    float maxWidth = screenRect.size.width;
    float maxHeight = screenRect.size.height;

    float positionVertex[] =
    {
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positionVertex), positionVertex, GL_STATIC_DRAW);

    // 頂点情報の位置を、頂点処理の変数に指定する（これを用いて描画を行う）
    glEnableVertexAttribArray(GLKVertexAttribPosition);

    // 頂点情報の格納場所と書式を頂点処理に教える
    glVertexAttribPointer(GLKVertexAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, fragmentBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, uvArray, GL_STATIC_DRAW);

    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // 図形を描く
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

- (bool)isHit:(float)pointX PointY:(float)pointY
{
    return (pointX >= _rect.left && pointX <= _rect.right &&
            pointY >= _rect.down && pointY <= _rect.up);
}

- (void)SetColor:(float)r g:(float)g b:(float)b a:(float)a
{
    self.baseEffect.constantColor = GLKVector4Make(r, g, b, a);

    _spriteColorR = r;
    _spriteColorG = g;
    _spriteColorB = b;
    _spriteColorA = a;
}

@end

