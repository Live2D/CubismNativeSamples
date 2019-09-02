/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <Windows.h>
#include <string>
#include <CubismFramework.hpp>
#include <Type/CubismBasicType.hpp>

//========================================================
//  名前空間のエイリアス
//========================================================
namespace Csm = Live2D::Cubism::Framework;

/**
* @brief プラットフォーム依存機能を抽象化する Cubism Platform Abstraction Layer.
*
* ファイル読み込みや時刻取得等のプラットフォームに依存する関数をまとめる
*
*/
class LAppPal
{
public:

    /**
     * @brief タイマーをスタート
     */
    static void StartTimer();

    /**
    * @brief ファイルをバイトデータとして読み込む
    *
    * ファイルをバイトデータとして読み込む
    *
    * @param[in]   filePath    読み込み対象ファイルのパス
    * @param[out]  outSize     ファイルサイズ
    * @return                  バイトデータ
    */
    static Csm::csmByte* LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize);


    /**
    * @brief バイトデータを解放する
    *
    * バイトデータを解放する
    *
    * @param[in]   byteData    解放したいバイトデータ
    */
    static void ReleaseBytes(Csm::csmByte* byteData);

    /**
    * @biref   デルタ時間（前回フレームとの差分）を取得する
    *
    * @return  デルタ時間[ms]
    *
    */
    static Csm::csmFloat32 GetDeltaTime();

    static void UpdateTime();

    /**
    * @brief ログを出力する
    *
    * ログを出力する
    *
    * @param[in]   format  書式付文字列
    * @param[in]   ...     (可変長引数)文字列
    *
    */
    static void PrintLog(const char*, ...);

    /**
    * @brief ログを出力する wchar版
    *
    * ログを出力する
    *
    * @param[in]   format  書式付文字列
    * @param[in]   ...     (可変長引数)文字列
    *
    */
    static void PrintLogW(const wchar_t*, ...);

    /**
    * @brief メッセージを出力する
    *
    * メッセージを出力する
    *
    * @param[in]   message  文字列
    *
    */
    static void PrintMessage(const Csm::csmChar* message);

    /**
     * @brief   座標変換
     *          フルスクリーン LT(-bufferX, bufferY) RB(bufferX, -bufferY) → ウィンドウ・マウス LT(0, 0) RB(bufferX, bufferY)
     */
    static void CoordinateFullScreenToWindow(float clientWidth, float clientHeight, float fullScreenX, float fullScreenY, float& retWindowX, float& retWindowY);

    /**
     * @brief   座標変換
     *          ウィンドウ・マウス LT(0, 0) RB(bufferX, bufferY) → フルスクリーン LT(-bufferX, bufferY) RB(bufferX, -bufferY)
     */
    static void CoordinateWindowToFullScreen(float clientWidth, float clientHeight, float windowX, float windowY, float& retFullScreenX, float& retFullScreenY);
};

