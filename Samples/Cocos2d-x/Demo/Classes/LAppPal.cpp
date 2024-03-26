/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <Model/CubismMoc.hpp>
#include "cocos2d.h"
#include "LAppDefine.hpp"

using namespace Csm;
USING_NS_CC;

csmByte* LAppPal::LoadFileAsBytes(const csmChar* filePath, csmSizeInt* outSize)
{
    ssize_t size = 0;
    csmByte* buf = FileUtils::getInstance()->getDataFromFile(filePath).takeBuffer(&size);

    if (buf == NULL)
    {
        if (LAppDefine::DebugLogEnable)
        {
            PrintLogLn("File load failed : %s", filePath);
        }
        return NULL;
    }

    *outSize = static_cast<csmSizeInt>(size);
    return buf;
}

void LAppPal::ReleaseBytes(csmByte* byteData)
{
    free(byteData);
}

csmFloat32  LAppPal::GetDeltaTime()
{
    if(Director::getInstance() != NULL)
    {
        return Director::getInstance()->getDeltaTime();
    }
    return 0.0f;
}

void LAppPal::PrintLogLn(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args); // 標準出力でレンダリング
    cocos2d::log("%s", buf); // cocos2dのログ関数で出力(関数で改行される)
    va_end(args);
}

void LAppPal::PrintMessageLn(const csmChar* message)
{
    PrintLogLn("%s", message);
}
