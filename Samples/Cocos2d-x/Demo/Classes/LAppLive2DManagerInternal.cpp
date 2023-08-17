/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppLive2DManagerInternal.h"
#if defined(CSM_TARGET_IOS) || defined(CSM_TARGET_MACOSX)
#include <CoreFoundation/CFBundle.h>
#elif defined(__ANDROID__)
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include "platform/android/jni/JniHelper.h"
#endif


namespace
{
#if defined(CSM_TARGET_IOS) || defined(CSM_TARGET_MACOSX)
inline Csm::csmString GetPath(CFURLRef url)
{
    CFStringRef cfstr = CFURLCopyFileSystemPath(url, CFURLPathStyle::kCFURLPOSIXPathStyle);
    CFIndex size = CFStringGetLength(cfstr) * 4 + 1; // Length * UTF-16 Max Character size + null-terminated-byte
    char* buf = new char[size];
    CFStringGetCString(cfstr, buf, size, CFStringBuiltInEncodings::kCFStringEncodingUTF8);
    Csm::csmString result(buf);
    delete[] buf;
    return result;
}
#elif defined(__ANDROID__)
// jobject, jstring
template<class T>
class ScopedJObject {
public:
    ScopedJObject(JNIEnv* env, T ptr);
    ScopedJObject(const ScopedJObject&) = delete;
    ScopedJObject(ScopedJObject&& move) = default;
    ScopedJObject& operator= (const ScopedJObject&) = delete;
    ScopedJObject& operator= (ScopedJObject&&) = default;
    ~ScopedJObject();
    inline JNIEnv* env()
    {
        return _env;
    }
    inline const T& get()
    {
        return _ptr;
    }
    inline void reset(T ptr = nullptr)
    {
        if (_ptr != nullptr)
        {
            _env->DeleteLocalRef(_ptr);
        }
        _ptr = ptr;
    }
private:
    JNIEnv* _env;
    T _ptr = nullptr;
};

class JniContext;
class JniAssetManager;

template<class T>
ScopedJObject<T>::ScopedJObject(JNIEnv* env, T ptr)
    : _env(env)
    , _ptr(ptr)
{
}
template<class T>
ScopedJObject<T>::~ScopedJObject()
{
    if (_ptr != nullptr)
    {
        _env->DeleteLocalRef(_ptr);
    }
}

class JniContext
{
public:
    JniContext() = delete;
    JniContext(const JniContext&) = delete;
    JniContext(JniContext&&) = default;
    JniContext& operator= (const JniContext&) = delete;
    JniContext& operator= (JniContext&&) = default;
    JniAssetManager getAssets();
    static JniContext GetContext(JNIEnv* env);

private:
    JniContext(JNIEnv* env, jobject obj);

    ScopedJObject<jobject> obj;
    jmethodID method_getAssets;
};

class JniAssetManager
{
public:
    JniAssetManager() = delete;
    JniAssetManager(JNIEnv* env, jobject assetManager);
    JniAssetManager(const JniAssetManager&) = delete;
    JniAssetManager(JniAssetManager&&) = default;
    JniAssetManager& operator= (const JniAssetManager&) = delete;
    JniAssetManager& operator= (JniAssetManager&&) = default;

    jobjectArray list(jstring path);
    Csm::csmVector<Csm::csmString> list(const char* path);

private:
    ScopedJObject<jobject> obj;
    jmethodID method_list;
};

JniContext::JniContext(JNIEnv* env, jobject context)
    : obj(env, context)
    , method_getAssets(env->GetMethodID(env->GetObjectClass(context), "getAssets", "()Landroid/content/res/AssetManager;"))
{
}

JniAssetManager JniContext::getAssets()
{
    return JniAssetManager(obj.env(), obj.env()->CallObjectMethod(obj.get(), method_getAssets));
}

JniContext JniContext::GetContext(JNIEnv* env)
{
    cocos2d::JniMethodInfo getContext;
    cocos2d::JniHelper::getStaticMethodInfo(getContext, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;");
    return JniContext(env, env->CallStaticObjectMethod(getContext.classID, getContext.methodID));
}

JniAssetManager::JniAssetManager(JNIEnv* env, jobject assetManager)
    : obj(env, assetManager)
    , method_list(env->GetMethodID(env->GetObjectClass(assetManager), u8"list", u8"(Ljava/lang/String;)[Ljava/lang/String;"))
{
}

Csm::csmVector<Csm::csmString> JniAssetManager::list(const char* path)
{
    JNIEnv* env = obj.env();
    ScopedJObject<jstring> jstr(env, env->NewStringUTF(path));
    jobjectArray files = (jobjectArray)env->CallObjectMethod(obj.get(), method_list, jstr.get());
    int length = env->GetArrayLength(files);
    Csm::csmVector<Csm::csmString> list(length);
    for (int i = 0; i < length; i++)
    {
        ScopedJObject<jstring> jstr(env, reinterpret_cast<jstring>(env->GetObjectArrayElement(files, i)));
        const char* jchars = env->GetStringUTFChars(jstr.get(), nullptr);
        list.PushBack(Csm::csmString(jchars));
        env->ReleaseStringUTFChars(jstr.get(), jchars);
    }
    return list;
}
#endif
}
namespace LAppLive2DManagerInternal {
#if defined(CSM_TARGET_IOS) || defined(CSM_TARGET_MACOSX)
void SetUpModel(Csm::csmVector<Csm::csmString>& dst)
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef rootUrl = CFBundleCopyResourcesDirectoryURL(mainBundle);
#if defined(CSM_TARGET_MACOSX)
    rootUrl = CFURLCopyAbsoluteURL(rootUrl);
#endif
    Csm::csmString rootPath(GetPath(rootUrl));
    rootPath.Append(1, '/');
    {
        CFArrayRef arr = CFBundleCreateBundlesFromDirectory(NULL, rootUrl, NULL);
        CFIndex count = CFArrayGetCount(arr);
        for (CFIndex i = 0; i < count; i++)
        {
            CFBundleRef item = (CFBundleRef)CFArrayGetValueAtIndex(arr, i);
            CFURLRef itemDirUrl = CFBundleCopyResourcesDirectoryURL(item);
            CFArrayRef model3json = CFBundleCopyResourceURLsOfType(item, CFSTR(".model3.json"), NULL);
            if (CFArrayGetCount(model3json) == 1)
            {
                Csm::csmString itemDirPath(GetPath(itemDirUrl));
                dst.PushBack(Csm::csmString(itemDirPath.GetRawString() + rootPath.GetLength()));
            }
        }
    }
}
#elif defined(__ANDROID__)
void SetUpModel(Csm::csmVector<Csm::csmString>& dst)
{
    JniContext cxtObj = JniContext::GetContext(cocos2d::JniHelper::getEnv());
    JniAssetManager assetManager = cxtObj.getAssets();
    Csm::csmVector<Csm::csmString> root = assetManager.list("");
    for (int i = 0; i < root.GetSize(); i++)
    {
        Csm::csmVector<Csm::csmString> files = assetManager.list(root[i].GetRawString());
        Csm::csmString target(root[i]);
        target += ".model3.json";
        for (int j = 0; j < files.GetSize(); j++)
        {
            if (target == files[j])
            {
                dst.PushBack(root[i]);
                break;
            }
        }
    }
}
#endif
}
