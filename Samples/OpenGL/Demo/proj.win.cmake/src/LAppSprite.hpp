/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LAppSprite_Common.hpp"

/**
* @brief スプライトを実装するクラス。
*
* テクスチャID、Rectの管理。
*
*/
class LAppSprite : public LAppSprite_Common
{
public:
    /**
    * @brief コンストラクタ
    *
    * @param[in]       x            x座標
    * @param[in]       y            y座標
    * @param[in]       width        横幅
    * @param[in]       height       高さ
    * @param[in]       textureId    テクスチャID
    * @param[in]       programId    シェーダID
    */
    LAppSprite(float x, float y, float width, float height, GLuint textureId, GLuint programId);

    /**
    * @brief デストラクタ
    */
    ~LAppSprite();

    /**
    * @brief 描画する
    *
    */
    void Render() const;

    /**
    * @brief テクスチャIDを指定して描画する
    *
    */
    void RenderImmidiate(GLuint textureId, const GLfloat uvVertex[8]) const;

    /**
    * @brief コンストラクタ
    *
    * @param[in]       pointX    x座標
    * @param[in]       pointY    y座標
    */
    bool IsHit(float pointX, float pointY) const;

    /**
     * @brief 色設定
     *
     * @param[in]       r (0.0~1.0)
     * @param[in]       g (0.0~1.0)
     * @param[in]       b (0.0~1.0)
     * @param[in]       a (0.0~1.0)
     */
    void SetColor(float r, float g, float b, float a);

    /**
     * @brief サイズ再設定
     *
     * @param[in]       x            x座標
     * @param[in]       y            y座標
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     */
    void ResetRect(float x, float y, float width, float height);

    /**
     * @brief ウインドウサイズ設定
     *
     * @param[in]       width        横幅
     * @param[in]       height       高さ
     */
    void SetWindowSize(int width, int height);

private:
    Rect _rect;             ///< 矩形
    int _positionLocation;  ///< 位置アトリビュート
    int _uvLocation;        ///< UVアトリビュート
    int _textureLocation;   ///< テクスチャアトリビュート
    int _colorLocation;     ///< カラーアトリビュート

    float _spriteColor[4];  ///< 表示カラー
    int _maxWidth;  ///< ウインドウ幅
    int _maxHeight;  ///< ウインドウ高さ
};

