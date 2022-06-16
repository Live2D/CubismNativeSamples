﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"
#include "base/CCDirector.h"
#include "LAppPal.hpp"

LAppSprite::LAppSprite(backend::Program* program)
{
    _program = program;
    _drawCommandBuffer = NULL;
}

LAppSprite::~LAppSprite()
{
    if (_drawCommandBuffer != NULL)
    {
        CSM_FREE(_drawCommandBuffer);
    }
}

void LAppSprite::RenderImmidiate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer, Texture2D* texture, float uvVertex[8])
{
    if (_drawCommandBuffer == NULL)
    {
        _drawCommandBuffer = CSM_NEW Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();
    }

    PipelineDescriptor* pipelineDescriptor = _drawCommandBuffer->GetCommandDraw()->GetPipelineDescriptor();
    backend::BlendDescriptor* blendDescriptor = _drawCommandBuffer->GetCommandDraw()->GetBlendDescriptor();
    backend::ProgramState* programState = pipelineDescriptor->programState;

    _drawCommandBuffer->GetCommandDraw()->GetCommand()->setDrawType(cocos2d::CustomCommand::DrawType::ELEMENT);
    _drawCommandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(cocos2d::backend::PrimitiveType::TRIANGLE);
    _drawCommandBuffer->CreateVertexBuffer(sizeof(float) * 2, 4 * 2);
    _drawCommandBuffer->CreateIndexBuffer(6);

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

    short positionIndex[] =
    {
        0,1,2,
        0,2,3
    };

    _drawCommandBuffer->UpdateVertexBuffer(positionVertex, uvVertex, 4);
    _drawCommandBuffer->UpdateIndexBuffer(positionIndex, 6);
    _drawCommandBuffer->CommitVertexBuffer();

    if (!programState)
    {
        programState = new cocos2d::backend::ProgramState(_program);
    }

    // attribute属性を登録
    programState->getVertexLayout()->setAttribute("position", _program->getAttributeLocation("position"), backend::VertexFormat::FLOAT2, 0, false);
    programState->getVertexLayout()->setAttribute("uv", _program->getAttributeLocation("uv"), backend::VertexFormat::FLOAT2, sizeof(float) * 2, false);

    // uniform属性の登録
    programState->setTexture(_program->getUniformLocation("texture"), 0, texture->getBackendTexture());

    programState->setUniform(_program->getUniformLocation("baseColor"), _spriteColor, sizeof(float) * 4);

    programState->getVertexLayout()->setLayout(sizeof(float) * 4);

    blendDescriptor->sourceRGBBlendFactor = cocos2d::backend::BlendFactor::ONE;
    blendDescriptor->destinationRGBBlendFactor = cocos2d::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->sourceAlphaBlendFactor = cocos2d::backend::BlendFactor::ONE;
    blendDescriptor->destinationAlphaBlendFactor = cocos2d::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->blendEnabled = true;

    pipelineDescriptor->programState = programState;

    // モデルの描画
    commandBuffer->AddDrawCommand(_drawCommandBuffer->GetCommandDraw());

}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _spriteColor[0] = r;
    _spriteColor[1] = g;
    _spriteColor[2] = b;
    _spriteColor[3] = a;
}
