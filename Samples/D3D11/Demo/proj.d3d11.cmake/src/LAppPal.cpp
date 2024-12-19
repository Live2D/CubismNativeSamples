/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"

using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
    LARGE_INTEGER s_frequency;
    LARGE_INTEGER s_lastFrame;
    double s_deltaTime = 0.0;
    const csmUint32 LogMessageLength = 256;
}

void LAppPal::StartTimer()
{
    QueryPerformanceFrequency(&s_frequency);
}

csmByte* LAppPal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    wchar_t wideStr[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0U, filePath.c_str(), -1, wideStr, MAX_PATH);

    int size = 0;
    struct _stat statBuf;
    if (_wstat(wideStr, &statBuf) == 0)
    {
        size = statBuf.st_size;

        if (size == 0)
        {
            if (DebugLogEnable)
            {
                PrintLogLn("Stat succeeded but file size is zero. path:%s", filePath.c_str());
            }
            return NULL;
        }
    }
    else
    {
        if (DebugLogEnable)
        {
            PrintLogLn("Stat failed. errno:%d path:%s", errno, filePath.c_str());
        }
        return NULL;
    }

    std::wfstream file;
    file.open(wideStr, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        if (DebugLogEnable)
        {
            PrintLogLn("File open failed. path:%s", filePath.c_str());
        }
        return NULL;
    }

    // ファイル名はワイド文字で探しているがファイルの中身はutf-8なので、1バイトずつ取得する。

    *outSize = size;
    csmChar* buf = new char[*outSize];
    std::wfilebuf* fileBuf = file.rdbuf();
    for (csmUint32 i = 0; i < *outSize; i++)
    {
        buf[i] = fileBuf->sbumpc();
    }
    file.close();

    return reinterpret_cast<csmByte*>(buf);
}

void LAppPal::ReleaseBytes(csmByte* byteData)
{
    delete[] byteData;
}

csmFloat32  LAppPal::GetDeltaTime()
{
    return static_cast<csmFloat32>(s_deltaTime);
}

void LAppPal::UpdateTime()
{
    if (s_frequency.QuadPart == 0)
    {
        StartTimer();
        QueryPerformanceCounter(&s_lastFrame);
        s_deltaTime = 0.0f;
        return;
    }

    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);

    const LONGLONG BASIS = 1000000;
    LONGLONG dwTime = ((current.QuadPart - s_lastFrame.QuadPart) * (LONGLONG)BASIS / s_frequency.QuadPart);
    s_deltaTime = (double)dwTime / (double)(BASIS);
    s_lastFrame = current;
}

void LAppPal::PrintLog(const char* format, ...)
{
    va_list args;
    char multiByteBuf[LogMessageLength];
    wchar_t wideBuf[LogMessageLength];
    va_start(args, format);
    _vsnprintf_s(multiByteBuf, sizeof(multiByteBuf), format, args);
    ConvertMultiByteToWide(multiByteBuf, wideBuf, sizeof(wideBuf));
    OutputDebugStringW(wideBuf);
    va_end(args);
}

void LAppPal::PrintLogLn(const char* format, ...)
{
    va_list args;
    char multiByteBuf[LogMessageLength];
    wchar_t wideBuf[LogMessageLength];
    va_start(args, format);
    _vsnprintf_s(multiByteBuf, sizeof(multiByteBuf), format, args);
    ConvertMultiByteToWide(multiByteBuf, wideBuf, sizeof(wideBuf));
    OutputDebugStringW(wideBuf);
    OutputDebugStringW(L"\n");   // 改行を別途付与します
    va_end(args);
}

void LAppPal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}

void LAppPal::PrintMessageLn(const csmChar* message)
{
    PrintLogLn("%s", message);
}

void LAppPal::CoordinateFullScreenToWindow(float clientWidth, float clientHeight, float fullScreenX, float fullScreenY, float& retWindowX, float& retWindowY)
{
    retWindowX = retWindowY = 0.0f;

    const float width = static_cast<float>(clientWidth);
    const float height = static_cast<float>(clientHeight);

    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    retWindowX = (fullScreenX + width) * 0.5f;
    retWindowY = (-fullScreenY + height) * 0.5f;
}

void LAppPal::CoordinateWindowToFullScreen(float clientWidth, float clientHeight, float windowX, float windowY, float& retFullScreenX, float& retFullScreenY)
{
    retFullScreenX = retFullScreenY = 0.0f;

    const float width = static_cast<float>(clientWidth);
    const float height = static_cast<float>(clientHeight);

    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    retFullScreenX = 2.0f * windowX - width;
    retFullScreenY = (2.0f * windowY - height) * -1.0f;
}

bool LAppPal::ConvertMultiByteToWide(const csmChar* multiByte, wchar_t* wide, int wideSize)
{
    return MultiByteToWideChar(CP_UTF8, 0U, multiByte, -1, wide, wideSize) != 0;
}

bool LAppPal::ConvertWideToMultiByte(const wchar_t* wide, csmChar* multiByte, int multiByteSize)
{
    return WideCharToMultiByte(CP_UTF8, 0U, wide, -1, multiByte, multiByteSize, NULL, NULL) != 0;
}
