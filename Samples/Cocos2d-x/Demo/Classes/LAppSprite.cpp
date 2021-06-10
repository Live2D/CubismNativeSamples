/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"
#include "base/CCDirector.h"
#include "LAppPal.hpp"

LAppSprite::LAppSprite(backend::ProgramState* programState)
{
    _drawCommand = new Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();

    // 何番目のattribute変数か
    _positionLocation = programState->getAttributeLocation("position");
    _uvLocation      = programState->getAttributeLocation( "uv");
    _textureLocation = programState->getUniformLocation("texture");
    _colorLocation = programState->getUniformLocation("baseColor");

    _spriteColor[0] = 1.0f;
    _spriteColor[1] = 1.0f;
    _spriteColor[2] = 1.0f;
    _spriteColor[3] = 1.0f;

    _drawCommand->GetCommandDraw()->GetPipelineDescriptor()->programState = programState;

    _drawCommand->CreateVertexBuffer(sizeof(float) * 8, 8 * 2);
    _drawCommand->GetCommandDraw()->GetCommand()->setDrawType(cocos2d::CustomCommand::DrawType::ARRAY);
    _drawCommand->GetCommandDraw()->GetCommand()->setPrimitiveType(cocos2d::backend::PrimitiveType::TRIANGLE_STRIP);
}

LAppSprite::~LAppSprite()
{
}

void LAppSprite::RenderImmidiate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer, Texture2D* texture, float uvVertex[8]) const
{
    auto programState = _drawCommand->GetCommandDraw()->GetPipelineDescriptor()->programState;
    auto vertexLayout = programState->getVertexLayout();

    // uniform属性の登録
    programState->setTexture(_textureLocation, 0, texture->getBackendTexture());

    // 画面サイズを取得する
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();

    // 頂点データ
    float positionVertex[] =
    {
        visibleSize.width / winSize.width, visibleSize.height / winSize.height,
       -visibleSize.width / winSize.width, visibleSize.height / winSize.height,
       -visibleSize.width / winSize.width,-visibleSize.height / winSize.height,
        visibleSize.width / winSize.width,-visibleSize.height / winSize.height,
    };

    _drawCommand->UpdateVertexBuffer(positionVertex, uvVertex, 8);

    // attribute属性を登録
    vertexLayout->setAttribute("position", _positionLocation, backend::VertexFormat::FLOAT2, sizeof(float), false);
    vertexLayout->setAttribute("uv", _uvLocation, backend::VertexFormat::FLOAT2, sizeof(float), false);
    programState->setUniform(_colorLocation, _spriteColor, sizeof(float) * 4);

    vertexLayout->setLayout(sizeof(float) * 8);

    // モデルの描画
    texture->setRenderTarget(true);

    commandBuffer->AddDrawCommand(_drawCommand->GetCommandDraw());

}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _spriteColor[0] = r;
    _spriteColor[1] = g;
    _spriteColor[2] = b;
    _spriteColor[3] = a;
}
