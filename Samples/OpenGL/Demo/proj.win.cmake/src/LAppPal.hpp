/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <string>

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
    static void PrintLog(const Csm::csmChar* format, ...);

    /**
    * @brief ログを出力し最後に改行する
    *
    * ログを出力し最後に改行する
    *
    * @param[in]   format  書式付文字列
    * @param[in]   ...     (可変長引数)文字列
    *
    */
    static void PrintLogLn(const Csm::csmChar* format, ...);

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
    * @brief メッセージを出力し最後に改行する
    *
    * メッセージを出力し最後に改行する
    *
    * @param[in]   message  文字列
    *
    */
    static void PrintMessageLn(const Csm::csmChar* message);

   /**
    * @brief マルチバイト文字からワイド文字に変換する
    *
    * @param[in]   multiByte 変換元
    * @param[in]   wide      格納先
    * @param[in]   wideSize  格納先の大きさ
    */
    static bool ConvertMultiByteToWide(const Csm::csmChar* multiByte, wchar_t* wide, int wideSize);

   /**
    * @brief ワイド文字からマルチバイト文字に変換する
    *
    * @param[in]   wide          変換元
    * @param[in]   multiByte     格納先
    * @param[in]   multiByteSize 格納先の大きさ
    */
    static bool ConvertWideToMultiByte(const wchar_t* wide, Csm::csmChar* multiByte, int multiByteSize);

private:
    static double s_currentFrame;
    static double s_lastFrame;
    static double s_deltaTime;
};

