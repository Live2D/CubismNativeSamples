
#---------------------------------------
# Cmake Version
#---------------------------------------
cmake_minimum_required(VERSION 3.10.2)

#---------------------------------------
# CXX_STANDARD
#---------------------------------------
set(CMAKE_CXX_STANDARD 11)

#---------------------------------------
# Variable
#---------------------------------------
set(SRC_DIR                     ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/src)
set(THIRD_PARTY_INCLUDE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/../../../thirdParty/stb/include)
set(CUBISM_CORE_DIR             ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Core)
set(FRAMEWORK_DIR               ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Framework/src)


#---------------------------------------
# Define
#---------------------------------------
add_definitions(-D CSM_TARGET_ANDROID_ES2)
# add_definitions(-D CSM_DEBUG)
# add_definitions(-D CSM_DEBUG_MEMORY_LEAKING)

# ここを有効にすると、Cubismモデルをフレームバッファとは別のテクスチャにレンダリングする挙動をとる 
#add_definitions(-DUSE_RENDER_TARGET)

#---------------------------------------
# Include Directories
#---------------------------------------
include_directories(
    ${CUBISM_CORE_DIR}/include/
    ${THIRD_PARTY_INCLUDE_DIR}
    ${FRAMEWORK_DIR}
    ${SRC_DIR}
)


#---------------------------------------
# Add Library
#---------------------------------------
add_library(
    JniBridge

    SHARED

    src/main/cpp/JniBridgeC.cpp

    # FrameWork
    ${FRAMEWORK_DIR}/CubismDefaultParameterId.cpp
    ${FRAMEWORK_DIR}/CubismFramework.cpp
    ${FRAMEWORK_DIR}/CubismModelSettingJson.cpp
    ${FRAMEWORK_DIR}/Effect/CubismBreath.cpp
    ${FRAMEWORK_DIR}/Effect/CubismEyeBlink.cpp
    ${FRAMEWORK_DIR}/Effect/CubismPose.cpp
    ${FRAMEWORK_DIR}/Id/CubismId.cpp
    ${FRAMEWORK_DIR}/Id/CubismIdManager.cpp
    ${FRAMEWORK_DIR}/Math/CubismMath.cpp
    ${FRAMEWORK_DIR}/Math/CubismMatrix44.cpp
    ${FRAMEWORK_DIR}/Math/CubismModelMatrix.cpp
    ${FRAMEWORK_DIR}/Math/CubismTargetPoint.cpp
    ${FRAMEWORK_DIR}/Math/CubismVector2.cpp
    ${FRAMEWORK_DIR}/Math/CubismViewMatrix.cpp
    ${FRAMEWORK_DIR}/Model/CubismMoc.cpp
    ${FRAMEWORK_DIR}/Model/CubismModel.cpp
    ${FRAMEWORK_DIR}/Model/CubismModelUserData.cpp
    ${FRAMEWORK_DIR}/Model/CubismModelUserDataJson.cpp
    ${FRAMEWORK_DIR}/Model/CubismUserModel.cpp
    ${FRAMEWORK_DIR}/Motion/ACubismMotion.cpp
    ${FRAMEWORK_DIR}/Motion/CubismExpressionMotion.cpp
    ${FRAMEWORK_DIR}/Motion/CubismMotion.cpp
    ${FRAMEWORK_DIR}/Motion/CubismMotionJson.cpp
    ${FRAMEWORK_DIR}/Motion/CubismMotionManager.cpp
    ${FRAMEWORK_DIR}/Motion/CubismMotionQueueEntry.cpp
    ${FRAMEWORK_DIR}/Motion/CubismMotionQueueManager.cpp
    ${FRAMEWORK_DIR}/Physics/CubismPhysics.cpp
    ${FRAMEWORK_DIR}/Physics/CubismPhysicsJson.cpp
    ${FRAMEWORK_DIR}/Rendering/CubismRenderer.cpp
    ${FRAMEWORK_DIR}/Rendering/OpenGL/CubismRenderer_OpenGLES2.cpp
    ${FRAMEWORK_DIR}/Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.cpp
    ${FRAMEWORK_DIR}/Type/csmRectF.cpp
    ${FRAMEWORK_DIR}/Type/csmString.cpp
    ${FRAMEWORK_DIR}/Utils/CubismDebug.cpp
    ${FRAMEWORK_DIR}/Utils/CubismJson.cpp
    ${FRAMEWORK_DIR}/Utils/CubismString.cpp

    # Source
    ${SRC_DIR}/LAppAllocator.cpp
    ${SRC_DIR}/LAppDefine.cpp
    ${SRC_DIR}/LAppDelegate.cpp
    ${SRC_DIR}/LAppLive2DManager.cpp
    ${SRC_DIR}/LAppModel.cpp
    ${SRC_DIR}/LAppPal.cpp
    ${SRC_DIR}/LAppSprite.cpp
    ${SRC_DIR}/LAppTextureManager.cpp
    ${SRC_DIR}/LAppView.cpp
    ${SRC_DIR}/TouchManager.cpp

)


#---------------------------------------
# Import Library
#---------------------------------------
add_library(
    Live2DCubismCore
    STATIC
    IMPORTED
)

set_target_properties(
    Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION
    ${CUBISM_CORE_DIR}/lib/android/${ANDROID_ABI}/libLive2DCubismCore.a
)


#---------------------------------------
# Link Library
#---------------------------------------
target_link_libraries(
   JniBridge

   log
   GLESv2
   Live2DCubismCore
)

