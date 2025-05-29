/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Type/CubismBasicType.hpp>

 /**
 * @brief スプライトを実装するクラス。
 *
 * テクスチャID、Rectの管理。
 *
 */
class LAppSprite_Common
{
public:
    /**
    * @brief Rect 構造体。
    */
    struct Rect
    {
    public:
        float left;     ///< 左辺
        float right;    ///< 右辺
        float up;       ///< 上辺
        float down;     ///< 下辺
    };

    /**
    * @brief デフォルトコンストラクタ
    */
    LAppSprite_Common();

    /**
    * @brief コンストラクタ
    *
    * @param[in]       textureId    テクスチャID
    */
    LAppSprite_Common(Csm::csmUint64 textureId);

    /**
    * @brief デストラクタ
    */
    virtual ~LAppSprite_Common();

    /**
    * @brief Getter テクスチャID
    * @return テクスチャIDを返す
    */
    virtual Csm::csmUint64 GetTextureId() { return _textureId; }

protected:
    Csm::csmUint64 _textureId;  ///< テクスチャID
};
