/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppTextureManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"
#include "LAppDelegate.hpp"


using namespace LAppDefine;

LAppTextureManager::LAppTextureManager() : LAppTextureManager_Common()
{
    _sequenceId = 0;
}

LAppTextureManager::~LAppTextureManager()
{
    ReleaseTextures();
}

LAppTextureManager::TextureInfo* LAppTextureManager::CreateTextureFromPngFile(std::string fileName, bool isPreMult, UINT maxSize)
{
    ID3D11Device* device = LAppDelegate::GetD3dDevice();
    ID3D11DeviceContext* context = LAppDelegate::GetD3dContext();

    // wcharに変換
    const int WCHAR_LENGTH = 512;
    wchar_t wchrStr[WCHAR_LENGTH] = L"";
    LAppPal::ConvertMultiByteToWide(fileName.c_str(), wchrStr, sizeof(wchrStr));

    ID3D11Resource* texture = NULL;
    ID3D11ShaderResourceView* textureView = NULL;
    LAppTextureManager::TextureInfo* textureInfo = NULL;

    HRESULT hr = S_OK;

    if(isPreMult)
    {
        hr = DirectX::CreateWICTextureFromFileEx(device,
            NULL,   // NULLにするとMIP=1となる
            wchrStr, maxSize,
            D3D11_USAGE_DYNAMIC,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_CPU_ACCESS_WRITE,
            0,
            DirectX::WIC_LOADER_DEFAULT,
            &texture, &textureView);
    }
    else
    {
        hr = DirectX::CreateWICTextureFromFileEx(device,
            context,
            wchrStr, maxSize,
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0,
            0,
            DirectX::WIC_LOADER_DEFAULT,
            &texture, &textureView);
    }

    if(SUCCEEDED(hr))
    {
        do
        {
            Microsoft::WRL::ComPtr<IWICImagingFactory> factoryWic;  ///<
            hr = CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&factoryWic)
            );
            if (FAILED(hr))
            {
                break;
            }

            // decoder作ってファイルを渡す
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            hr = factoryWic->CreateDecoderFromFilename(wchrStr, NULL,
                GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
            if (FAILED(hr))
            {
                break;
            }

            // decoderからframeを取得
            Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
            hr = decoder->GetFrame(0, &frame);
            if (FAILED(hr))
            {
                break;
            }
            UINT texWidth, texHeight;
            hr = frame->GetSize(&texWidth, &texHeight);
            if (FAILED(hr))
            {
                break;
            }

            // テクスチャ情報
            textureInfo = new LAppTextureManager::TextureInfo();

            if (!textureInfo)
            {
                break;
            }

            // 作成成功
            {
                // 次のID
                const Csm::csmUint32 addId = _sequenceId + 1;

                // 情報格納
                textureInfo->fileName = fileName;
                textureInfo->width = static_cast<int>(texWidth);
                textureInfo->height = static_cast<int>(texHeight);
                textureInfo->id = addId;

                _sequenceId = addId;

                _texturesInfo.PushBack(textureInfo);
                _textures.PushBack(texture);
                _textureView.PushBack(textureView);


                if (isPreMult)
                {
                    D3D11_MAPPED_SUBRESOURCE subRes;
                    if (SUCCEEDED(context->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
                    {
                        // テンポラリ領域確保
                        byte* pdd = static_cast<byte*>(malloc(subRes.RowPitch * texHeight));
                        // 全エリアコピー
                        frame->CopyPixels(NULL, subRes.RowPitch, subRes.RowPitch * texHeight, pdd);

                        ULONG* pixel32 = static_cast<ULONG*>(subRes.pData);
                        for (unsigned int htLoop = 0; htLoop < texHeight; htLoop++)
                        {
                            unsigned char* pixel4 = reinterpret_cast<unsigned char*>(pdd) + subRes.RowPitch*htLoop;
                            unsigned int* pixel32 =
                                reinterpret_cast<unsigned int*>(reinterpret_cast<unsigned char*>(subRes.pData) + subRes.RowPitch*htLoop);

                            for (UINT i = 0; i < subRes.RowPitch; i += 4)
                            {
                                unsigned int val = Premultiply(pixel4[i + 0], pixel4[i + 1], pixel4[i + 2], pixel4[i + 3]);
                                pixel32[(i >> 2)] = val;
                            }
                        }

                        // テンポラリ開放
                        free(pdd);
                        // 解除
                        context->Unmap(texture, 0);
                    }
                }

                return textureInfo;
            }
        } while (0);
    }

    // 失敗
    if (DebugLogEnable)
    {
        LAppPal::PrintLogLn("Texture Load Error : %s", fileName.c_str());
    }

    return NULL;
}

void LAppTextureManager::ReleaseTextures()
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        // info除去
        delete _texturesInfo[i];

        // 実体除去
        if (_textureView[i])
        {
            _textureView[i]->Release();
            _textureView[i] = NULL;
        }
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = NULL;
        }
    }

    _texturesInfo.Clear();
    _textures.Clear();
    _textureView.Clear();
}

void LAppTextureManager::ReleaseTexture(Csm::csmUint32 textureId)
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

        // 実体除去
        if (_textureView[i])
        {
            _textureView[i]->Release();
            _textureView[i] = NULL;
        }
        if (_textures[i])
        {
            _textures[i]->Release();
            _textures[i] = NULL;
        }

        // 器除去
        _texturesInfo.Remove(i);
        _textures.Remove(i);
        _textureView.Remove(i);

        break;
    }

    if (_textureView.GetSize() == 0)
    {
        _textureView.Clear();
    }
    if (_textures.GetSize() == 0)
    {
        _textures.Clear();
    }
    if(_texturesInfo.GetSize()==0)
    {
        _texturesInfo.Clear();
    }
}

void LAppTextureManager::ReleaseTexture(std::string fileName)
{
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->fileName == fileName)
        {
            // info除去
            delete _texturesInfo[i];

            // 実体除去
            if (_textureView[i])
            {
                _textureView[i]->Release();
                _textureView[i] = NULL;
            }
            if (_textures[i])
            {
                _textures[i]->Release();
                _textures[i] = NULL;
            }

            // 器除去
            _texturesInfo.Remove(i);
            _textures.Remove(i);
            _textureView.Remove(i);

            break;
        }
    }

    if (_textureView.GetSize() == 0)
    {
        _textureView.Clear();
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

bool LAppTextureManager::GetTexture(Csm::csmUint32 textureId, ID3D11ShaderResourceView*& retTexture) const
{
    retTexture = NULL;
    for (Csm::csmUint32 i = 0; i < _texturesInfo.GetSize(); i++)
    {
        if (_texturesInfo[i]->id == textureId)
        {
            retTexture = _textureView[i];
            return true;
        }
    }

    return false;
}
