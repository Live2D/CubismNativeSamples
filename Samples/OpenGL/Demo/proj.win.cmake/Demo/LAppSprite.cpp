/*
* Copyright(c) Live2D Inc. All rights reserved.
*
* Use of this source code is governed by the Live2D Open Software license
* that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
*/

#include "LAppSprite.hpp"
#include "LAppDelegate.hpp"

LAppSprite::LAppSprite(float x, float y, float width, float height, GLuint textureId) 
    : _rect()
{
    _rect.left = (x - width * 0.5f);
    _rect.right = (x + width * 0.5f);
    _rect.up = (y + height * 0.5f);
    _rect.down = (y - height * 0.5f);
    _textureId = textureId;
}

LAppSprite::~LAppSprite()
{
}

void LAppSprite::Render(GLuint programId, GLFWwindow* window) const
{
    glEnable(GL_TEXTURE_2D);
    const GLfloat uvVertex[] = 
    {
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    // 何番目のattribute変数か
    int positionLocation = glGetAttribLocation(programId, "position");
    int uvLocation = glGetAttribLocation(programId, "uv");
    int textureLocation = glGetUniformLocation(programId, "texture");

    // attribute属性を有効にする
    glEnableVertexAttribArray(positionLocation);
    glEnableVertexAttribArray(uvLocation);

    // uniform属性の登録
    glUniform1i(textureLocation, 0);

    // 画面サイズを取得する
    int maxWidth, maxHeight;
    glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &maxWidth, &maxHeight);

    // 頂点データ
    float positionVertex[] = 
    {
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.up   - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.left  - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f),
        (_rect.right - maxWidth * 0.5f) / (maxWidth * 0.5f), (_rect.down - maxHeight * 0.5f) / (maxHeight * 0.5f)
    };

    // attribute属性を登録
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, positionVertex);
    glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, 0, uvVertex);

    // モデルの描画
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

bool LAppSprite::IsHit(float pointX, float pointY) const
{
    // 画面サイズを取得する
    int maxWidth, maxHeight;
    glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &maxWidth, &maxHeight);
    
    //Y座標は変換する必要あり
    float y = maxHeight - pointY;

    return (pointX >= _rect.left && pointX <= _rect.right && y <= _rect.up && y >= _rect.down);
}