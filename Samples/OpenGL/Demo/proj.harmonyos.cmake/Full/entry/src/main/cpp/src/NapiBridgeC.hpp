/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once
#include <Type/csmVector.hpp>
#include <Type/csmString.hpp>

/**
* @brief Napi Bridge Class
*/
class NapiBridgeC
{
public:
    /**
    * @brief Assets 取得
    */
    static Csm::csmVector<Csm::csmString> GetAssetList(const Csm::csmString& path);

    /**
    * @brief ファイル読み込み
    */
    static char* LoadFile(const char* filePath, unsigned int* outSize);

    /**
    * @brief アプリをバックグラウンドに移動
    */
    static void MoveTaskToBack();

};
