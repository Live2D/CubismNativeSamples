/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppTextureManager_Common.hpp"

LAppTextureManager_Common::LAppTextureManager_Common()
{
}

LAppTextureManager_Common::~LAppTextureManager_Common()
{
    ReleaseTexturesInfo();
}

void LAppTextureManager_Common::ReleaseTexturesInfo()
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        delete _texturesInfo[i];
    }

    _texturesInfo.Clear();
}

LAppTextureManager_Common::TextureInfo* LAppTextureManager_Common::GetTextureInfoByName(std::string& fileName) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            return _texturesInfo[i];
        }
    }

    return NULL;
}

LAppTextureManager_Common::TextureInfo* LAppTextureManager_Common::GetTextureInfoById(Csm::csmUint32 textureId) const
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            return _texturesInfo[i];
        }
    }

    return NULL;
}
