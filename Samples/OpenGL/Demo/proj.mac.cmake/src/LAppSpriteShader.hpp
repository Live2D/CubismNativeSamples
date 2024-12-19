/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Utils/CubismString.hpp>
#include "CubismFramework.hpp"

/**
* @brief スプライト用のシェーダー設定を保持するクラス
*/
class LAppSpriteShader
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param[in]       executeAbsolutePath 絶対パス
     */
    LAppSpriteShader(Csm::csmString executeAbsolutePath);

    /**
     * @brief デストラクタ
     */
    ~LAppSpriteShader();

    /**
     * @brief   シェーダーIDを取得する
     */
    GLuint GetShaderId() const;

private:
    /**
    * @brief シェーダーを作成する。
    *
    * @param[in]       executeAbsolutePath 絶対パス
    */
    GLuint CreateShader(Csm::csmString executeAbsolutePath);

    /**
     * @brief   CreateShader内部関数 エラーチェック
     */
    bool CheckShader(GLuint shaderId);

    /**
     * @brief シェーダーをコンパイルする
     *        コンパイルに失敗した場合は 0 が戻る。
     *
     * @param[in]       filename     シェーダーファイル名
     * @param[in]       shaderType   作成するシェーダーの種類
     */
    GLuint CompileShader(Csm::csmString filename, GLenum shaderType);

    GLuint _programId;                       ///< シェーダID
};
