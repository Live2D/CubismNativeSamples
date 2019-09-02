LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libLive2DCubismCore
LOCAL_SRC_FILES	:=  $(LOCAL_PATH)/../../../../../../Core/lib/android/$(TARGET_ARCH_ABI)/libLive2DCubismCore.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../../cocos2d/cocos/audio/include)

LOCAL_CPPFLAGS := -DCSM_TARGET_ANDROID_ES2
LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppAllocator.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppDefine.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppLive2DManager.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppModel.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppPal.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppView.cpp \
                   $(LOCAL_PATH)/../../../Classes/LAppSprite.cpp \
                   $(LOCAL_PATH)/../../../Classes/SampleScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/TouchManager.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/ACubismMotion.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Type/csmRectF.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Type/csmString.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Effect/CubismBreath.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Utils/CubismDebug.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/CubismDefaultParameterId.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismExpressionMotion.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Effect/CubismEyeBlink.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/CubismFramework.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Id/CubismId.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Id/CubismIdManager.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Utils/CubismJson.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismMath.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismMatrix44.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Model/CubismMoc.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Model/CubismModel.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismModelMatrix.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/CubismModelSettingJson.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Model/CubismModelUserData.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Model/CubismModelUserDataJson.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismMotion.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismMotionJson.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismMotionManager.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismMotionQueueEntry.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Motion/CubismMotionQueueManager.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Physics/CubismPhysics.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Physics/CubismPhysicsJson.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Effect/CubismPose.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Rendering/CubismRenderer.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Rendering/OpenGL/CubismRenderer_OpenGLES2.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Utils/CubismString.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismTargetPoint.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Model/CubismUserModel.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismVector2.cpp \
                   $(LOCAL_PATH)/../../../../../../Framework/src/Math/CubismViewMatrix.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
                    $(LOCAL_PATH)/../../../../../../Core/include \
                    $(LOCAL_PATH)/../../../../../../Framework/src

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cc_static libLive2DCubismCore


# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
