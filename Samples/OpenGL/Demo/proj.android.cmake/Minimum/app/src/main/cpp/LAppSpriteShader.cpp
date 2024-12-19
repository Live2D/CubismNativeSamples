/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSpriteShader.hpp"

#include "LAppDefine.hpp"
#include "LAppPal.hpp"

LAppSpriteShader::LAppSpriteShader()
{
    _programId = CreateShader();
}

LAppSpriteShader::~LAppSpriteShader()
{
    glDeleteShader(_programId);
}

GLuint LAppSpriteShader::GetShaderId() const
{
    return _programId;
}

GLuint LAppSpriteShader::CreateShader()
{
    // シェーダーのパスの作成
    Csm::csmString vertShaderFile(LAppDefine::ShaderPath);
    vertShaderFile += LAppDefine::VertShaderName;
    Csm::csmString fragShaderFile(LAppDefine::ShaderPath);
    fragShaderFile += LAppDefine::FragShaderName;

    // シェーダーのコンパイル
    GLuint vertexShaderId = CompileShader(vertShaderFile, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = CompileShader(fragShaderFile, GL_FRAGMENT_SHADER);

    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    // 不要になったシェーダーオブジェクトの削除
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

GLuint LAppSpriteShader::CompileShader(Csm::csmString filename, GLenum shaderType)
{
    // ファイル読み込み
    Csm::csmSizeInt bufferSize = 0;
    const char* shaderString = reinterpret_cast<const char*>(LAppPal::LoadFileAsBytes(filename.GetRawString(), &bufferSize));
    const GLint glSize = (GLint)bufferSize;

    // コンパイル
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderString, &glSize);
    glCompileShader(shaderId);

    // 読み込んだシェーダー文字列の開放
    LAppPal::ReleaseBytes(reinterpret_cast<Csm::csmByte*>(const_cast<char*>(shaderString)));

    return shaderId;
}
