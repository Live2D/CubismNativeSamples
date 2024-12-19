/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "NapiBridgeC.hpp"
#include <atomic>
#include <bits/alltypes.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <uv.h>
#include "rawfile/raw_file_manager.h"
#include "rawfile/raw_file.h"
#include "rawfile/raw_dir.h"
#include "LAppDelegate.hpp"
#include "LAppPal.hpp"

using namespace Csm;

static csmString g_Id;
static NativeResourceManager *g_NativeResourceManager = nullptr;
static OH_NativeXComponent *g_NativeXComponent;
static OH_NativeXComponent_Callback g_Callback;
static napi_threadsafe_function g_MoveTaskToBack;
static std::atomic<csmBool> g_IsSurfaceChanged;
static std::atomic<OH_NativeXComponent_TouchEvent> g_TouchEvent;
static std::atomic<csmBool> g_IsTouchEvent;
static std::atomic<csmBool> g_IsLoop;

static csmBool CheckComponent(OH_NativeXComponent *component)
{
    int32_t ret;
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    ret = OH_NativeXComponent_GetXComponentId(component, idStr, &idSize);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
    {
        return false;
    }

    return g_Id == idStr;
}

static napi_value ConvertBooleanToNapiValue(napi_env env, bool result)
{
    napi_value state;
    napi_status status = napi_get_boolean(env, result, &state);
    if (status != napi_ok)
    {
        LAppPal::PrintLogLn("Convert boolean error.");
        return nullptr;
    }

    return state;
}

csmVector<csmString> NapiBridgeC::GetAssetList(const csmString &path)
{
    csmVector<csmString> assetList;
    RawDir *rawDir;
    int count;

    if (g_NativeResourceManager)
    {
        rawDir = OH_ResourceManager_OpenRawDir(g_NativeResourceManager, path.GetRawString());
        count = OH_ResourceManager_GetRawFileCount(rawDir);
        for (int i = 0; i < count; i++)
        {
            assetList.PushBack(OH_ResourceManager_GetRawFileName(rawDir, i));
        }
        OH_ResourceManager_CloseRawDir(rawDir);
    }

    return assetList;
}

char *NapiBridgeC::LoadFile(const char *filePath, unsigned int *outSize)
{
    RawFile *rawFile;
    long length;
    char *buffer = nullptr;

    if (g_NativeResourceManager)
    {
        rawFile = OH_ResourceManager_OpenRawFile(g_NativeResourceManager, filePath);
        if (!rawFile)
        {
            LAppPal::PrintLogLn("LoadFile failed.");
            return nullptr;
        }
        length = OH_ResourceManager_GetRawFileSize(rawFile);
        buffer = new char[length];
        *outSize = static_cast<unsigned int>(OH_ResourceManager_ReadRawFile(rawFile, buffer, length));

        OH_ResourceManager_CloseRawFile(rawFile);
    }

    return buffer;
}

void NapiBridgeC::MoveTaskToBack()
{
    napi_call_threadsafe_function(g_MoveTaskToBack, nullptr, napi_tsfn_blocking);
}

static void CallMoveTaskToBack(napi_env env, napi_value jsFunc, void* context, void* data)
{
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_call_function(env, undefined, jsFunc, 0, nullptr, nullptr);
}

static void AsyncLoop(napi_env env, void* data)
{
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_TouchEvent touchEvent;
    int32_t ret;

    LAppPal::PrintLogLn("Start SubThread.");

    LAppDelegate::GetInstance()->InitializeEgl();
    LAppDelegate::GetInstance()->InitializeDisplay();

    while (g_IsLoop)
    {
        if (g_IsSurfaceChanged)
        {
            ret = OH_NativeXComponent_GetXComponentSize(g_NativeXComponent, LAppDelegate::GetInstance()->GetWindow(), &width, &height);
            if (ret == OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
            {
                LAppDelegate::GetInstance()->ChangeDisplay(width, height);
            }
            g_IsSurfaceChanged = false;
        }
        if (g_IsTouchEvent)
        {
            touchEvent = g_TouchEvent;
            switch (touchEvent.type)
            {
                case OH_NATIVEXCOMPONENT_DOWN:
                    LAppDelegate::GetInstance()->OnTouchBegan(touchEvent.x, touchEvent.y);
                    break;
                case OH_NATIVEXCOMPONENT_UP:
                    LAppDelegate::GetInstance()->OnTouchEnded(touchEvent.x, touchEvent.y);
                    break;
                case OH_NATIVEXCOMPONENT_MOVE:
                    LAppDelegate::GetInstance()->OnTouchMoved(touchEvent.x, touchEvent.y);
                    break;
            }
            g_IsTouchEvent = false;
        }

        LAppDelegate::GetInstance()->Run();
    }
    LAppDelegate::GetInstance()->ReleaseEgl();
    LAppDelegate::GetInstance()->ReleaseInstance();
}

static void AsyncCallback(napi_env env, napi_status status, void* data)
{
    LAppPal::PrintLogLn("Call AsyncCallback.");

    napi_delete_async_work(env, *reinterpret_cast<napi_async_work*>(data));
    CSM_FREE(data);
}

static napi_value InitializeMainLoop(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2];
    napi_value workName;
    napi_async_work* asyncWork;
    napi_status status;

    LAppPal::PrintLogLn("Start InitSurface.");

    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    g_NativeResourceManager = OH_ResourceManager_InitNativeResourceManager(env, argv[0]);

    // create moveTaskToBack function
    status =  napi_create_threadsafe_function(env, argv[1], nullptr, workName, 0, 1, nullptr, nullptr, nullptr, CallMoveTaskToBack, &g_MoveTaskToBack);
    if (status != napi_ok)
    {
        LAppPal::PrintLogLn("Failed to create moveTaskToBack function.");
        return ConvertBooleanToNapiValue(env, false);
    }

    // create async
    g_IsSurfaceChanged = true;
    g_IsTouchEvent = false;
    g_IsLoop = true;
    status = napi_create_string_utf8(env, "live2dDemoLoop", NAPI_AUTO_LENGTH, &workName);
    if (status != napi_ok)
    {
        LAppPal::PrintLogLn("Failed to obtain the name of the async process.");
        return ConvertBooleanToNapiValue(env, false);
    }
    asyncWork = reinterpret_cast<napi_async_work*>(CSM_MALLOC(sizeof(napi_async_work)));
    if (!asyncWork)
    {
        LAppPal::PrintLogLn("Memory allocation failed.");
        return ConvertBooleanToNapiValue(env, false);
    }
    status = napi_create_async_work(env, NULL, workName, AsyncLoop, AsyncCallback, asyncWork, asyncWork);
    if (status != napi_ok)
    {
        LAppPal::PrintLogLn("Failed to create async process.");
        return ConvertBooleanToNapiValue(env, false);
    }
    status = napi_queue_async_work(env, *asyncWork);
    if (status != napi_ok)
    {
        LAppPal::PrintLogLn("Failed to queue async process.");
        return ConvertBooleanToNapiValue(env, false);
    }

    return ConvertBooleanToNapiValue(env, true);
}

static void OnSurfaceCreated(OH_NativeXComponent *component, void *window)
{
    LAppPal::PrintLogLn("Start OnSurfaceCreated.");

    if (!CheckComponent(component))
    {
        LAppPal::PrintLogLn("OnSurfaceCreated failed.");
        return;
    }

    LAppDelegate::GetInstance()->SetWindow(window);
}

static void OnSurfaceChanged(OH_NativeXComponent *component, void *window)
{
    LAppPal::PrintLogLn("Start OnSurfaceChanged.");

    if (!CheckComponent(component))
    {
        LAppPal::PrintLogLn("OnSurfaceChanged failed.");
        return;
    }

    g_IsSurfaceChanged = true;
}

static void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    LAppPal::PrintLogLn("Start OnSurfaceDestroyed.");

    g_IsLoop = false;

    if (g_NativeResourceManager)
    {
        OH_ResourceManager_ReleaseNativeResourceManager(g_NativeResourceManager);
        g_NativeResourceManager = nullptr;
    }

    napi_release_threadsafe_function(g_MoveTaskToBack, napi_tsfn_release);
}

static void DispatchTouchEvent(OH_NativeXComponent *component, void *window)
{
    LAppPal::PrintLogLn("Start DispatchTouchEvent.");

    OH_NativeXComponent_TouchEvent touchEvent;
    int32_t ret;

    ret = OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
    {
        LAppPal::PrintLogLn("DispatchTouchEvent failed.");
        return;
    }
    g_TouchEvent = touchEvent;
    g_IsTouchEvent = true;
}

static bool Export(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value exportInstance = nullptr;
    OH_NativeXComponent *nativeXComponent = nullptr;
    int32_t ret;
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;

    status = napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    if (status != napi_ok)
    {
        return false;
    }

    status = napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent));
    if (status != napi_ok)
    {
        return false;
    }

    ret = OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize);
    if (ret != OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
    {
        return false;
    }

    g_Id = idStr;
    g_NativeXComponent = nativeXComponent;

    g_Callback.OnSurfaceCreated = OnSurfaceCreated;
    g_Callback.OnSurfaceChanged = OnSurfaceChanged;
    g_Callback.OnSurfaceDestroyed = OnSurfaceDestroyed;
    g_Callback.DispatchTouchEvent = DispatchTouchEvent;
    OH_NativeXComponent_RegisterCallback(g_NativeXComponent, &g_Callback);

    return true;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    LAppPal::PrintLogLn("Start Init.");

    napi_property_descriptor desc[] =
    {
        { "initializeMainLoop", nullptr, InitializeMainLoop, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    if (!Export(env, exports))
    {
        LAppPal::PrintLogLn("Init failed.");
    }

    return exports;
}
EXTERN_C_END

static napi_module demoModule =
{
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "Demo",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
