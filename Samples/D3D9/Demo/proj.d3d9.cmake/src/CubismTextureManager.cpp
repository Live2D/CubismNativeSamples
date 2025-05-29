/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismTextureManager.hpp"

#include <wrl/client.h>



#include "CubismDirectXRenderer.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"


CubismTextureManager::CubismTextureManager() : LAppTextureManager_Common()
{
    _sequenceId = 0;
}

CubismTextureManager::~CubismTextureManager()
{
    ReleaseTextures();
}

CubismTextureManager::TextureInfo* CubismTextureManager::CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT width, UINT height, UINT mipLevel, DWORD filter)
{
    LPDIRECT3DDEVICE9 device = CubismDirectXRenderer::GetInstance()->_device;

    IDirect3DTexture9* texture = nullptr;
    CubismTextureManager::TextureInfo* textureInfo = nullptr;

    D3DXIMAGE_INFO resultInfo;

    // 実験としてここでα合成を行う場合はMipLevelを1にする
    if (isPreMult)
    {
        mipLevel = 1;
    }

    // Lockする場合は立てる
    DWORD usage = isPreMult ? D3DUSAGE_DYNAMIC : 0;

    if (SUCCEEDED(D3DXCreateTextureFromFileExA(device, fileName.c_str(), width, height,
        mipLevel,   // 「この値が 0 または D3DX_DEFAULT の場合は、完全なミップマップ チェーンが作成される。」
        usage,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        filter,
        D3DX_DEFAULT,   // ミップフィルタ設定 「このパラメータに D3DX_DEFAULT を指定することは、D3DX_FILTER_BOX を指定することと等しい。」
        0,              // カラーキー
        &resultInfo,
        NULL,           // パレット 使用せず
        &texture)))
    {
        // テクスチャ情報
        textureInfo = new CubismTextureManager::TextureInfo();

        if (textureInfo)
        {
            // 次のID
            const Csm::csmUint32 addId = _sequenceId + 1;

            _textures.PushBack(texture);

            // 情報格納
            textureInfo->fileName = fileName;
            textureInfo->width = static_cast<int>(resultInfo.Width);
            textureInfo->height = static_cast<int>(resultInfo.Height);
            textureInfo->id = addId;
            _texturesInfo.PushBack(textureInfo);

            _sequenceId = addId;

            if (isPreMult)
            {
                D3DLOCKED_RECT locked;
                HRESULT hr = texture->LockRect(0, &locked, nullptr, 0);
                if (SUCCEEDED(hr))
                {
                    for (unsigned int htLoop = 0; htLoop < resultInfo.Height; htLoop++)
                    {
                        unsigned char* pixel4 = reinterpret_cast<unsigned char*>(locked.pBits) + locked.Pitch*htLoop;
                        unsigned int* pixel32 = reinterpret_cast<unsigned int*>(pixel4);

                        for (int i = 0; i < locked.Pitch; i += 4)
                        {
                            unsigned int val = Premultiply(pixel4[i + 0], pixel4[i + 1], pixel4[i + 2], pixel4[i + 3]);
                            pixel32[(i >> 2)] = val;
                        }
                    }

                    texture->UnlockRect(0);
                }
            }

            return textureInfo;
        }
    }

    // 失敗
    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLogLn("Texture Load Error : %s", fileName.c_str());
    }

    delete textureInfo;
    if (texture)
    {
        texture->Release();
        texture = nullptr;
    }

    return nullptr;
}

bool CubismTextureManager::GetTexture(Csm::csmUint32 textureId, IDirect3DTexture9*& retTexture) const
{
    retTexture = NULL;
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            retTexture = _textures[i];
            return true;
        }
    }

    return false;
}

void CubismTextureManager::ReleaseTextures()
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        // info除去
        delete _texturesInfo[i];

        // 実体除去
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = nullptr;
        }
    }

    _texturesInfo.Clear();
    _textures.Clear();
}

void CubismTextureManager::ReleaseTexture(Csm::csmUint32 textureId)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id != textureId)
        {
            continue;
        }
        // ID一致

        // info除去
        delete _texturesInfo[i];
        _texturesInfo.Remove(i);

        // 実体除去
        // getBaseAddressでFramework::loadTextureFromGnfが確保した個所が取れる
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = nullptr;
        }
        _textures.Remove(i);

        break;
    }

    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }
}

void CubismTextureManager::ReleaseTexture(std::string fileName)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            // info除去
            delete _texturesInfo[i];
            _texturesInfo.Remove(i);

            // 実体除去
            if (_textures[i])
            {
                _textures[i]->Release();
                _textures[i] = NULL;
            }
            _textures.Remove(i);

            break;
        }
    }

    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
    if (_texturesInfo.GetSize() == 0)
    {
        _texturesInfo.Clear();
    }
}


