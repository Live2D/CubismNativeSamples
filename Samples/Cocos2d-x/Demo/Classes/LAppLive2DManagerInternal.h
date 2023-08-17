/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once
#if defined(CSM_TARGET_IOS) || defined(CSM_TARGET_MACOSX) || defined(__ANDROID__)
#include <Type/csmString.hpp>
#include <Type/csmVector.hpp>
namespace LAppLive2DManagerInternal
{
    void SetUpModel(Csm::csmVector<Csm::csmString>& dst);
}
#endif
