/*
* Copyright(c) Live2D Inc. All rights reserved.
*
* Use of this source code is governed by the Live2D Open Software license
* that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
*/

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
* @brief スプライトを実装するクラス。
*
* テクスチャID、Rectの管理。
*
*/
class LAppSprite
{
public:
    /**
    * @brief Rect 構造体。
    */
    struct Rect
    {
    public:
        float left;     ///< 左辺
        float right;    ///< 右辺
        float up;       ///< 上辺
        float down;     ///< 下辺
    };

    /**
    * @brief コンストラクタ
    *
    * @param[in]       x            x座標
    * @param[in]       y            y座標
    * @param[in]       width        横幅
    * @param[in]       height       高さ
    * @param[in]       textureId    テクスチャID
    */
    LAppSprite(float x, float y, float width, float height, GLuint textureId);

    /**
    * @brief デストラクタ
    */
    ~LAppSprite();

    /**
    * @brief Getter テクスチャID
    * @return テクスチャIDを返す
    */
    GLuint GetTextureId() { return _textureId; }

    /**
    * @brief 描画する
    *
    * @param[in]       programId    シェーダID
    * @param[in]       window       描画するWindow情報
    */
    void Render(GLuint programId, GLFWwindow* window) const;
    
    /**
    * @brief コンストラクタ
    *
    * @param[in]       pointX    x座標
    * @param[in]       pointY    y座標
    */
    bool IsHit(float pointX, float pointY) const;

private:
    GLuint _textureId;   ///< テクスチャID
    Rect _rect;          ///< 矩形
};

