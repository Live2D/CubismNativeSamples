/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppModel_Common.hpp"

#include "LAppDefine.hpp"
#include "LAppPal.hpp"

Csm::csmByte* LAppModel_Common::CreateBuffer(const Csm::csmChar* path, Csm::csmSizeInt* size)
{
    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]create buffer: %s ", path);
    }
    return LAppPal::LoadFileAsBytes(path, size);
}

void LAppModel_Common::DeleteBuffer(Csm::csmByte* buffer, const Csm::csmChar* path)
{
    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLogLn("[APP]delete buffer: %s", path);
    }
    LAppPal::ReleaseBytes(buffer);
}
