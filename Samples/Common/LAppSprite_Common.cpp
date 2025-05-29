/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "LAppSprite_Common.hpp"

LAppSprite_Common::LAppSprite_Common()
    : _textureId(0)
{}

LAppSprite_Common::LAppSprite_Common(Csm::csmUint64 textureId)
    : _textureId(textureId)
{}

LAppSprite_Common::~LAppSprite_Common()
{}
