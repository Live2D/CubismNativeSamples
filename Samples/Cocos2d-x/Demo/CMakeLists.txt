cmake_minimum_required(VERSION 3.10)

option(
  CORE_CRL_MD
  "Use Cubism Core that is multithread-specific and DLL-specific version"
  OFF
)

# Set app name.
set(APP_NAME Demo)
# Set directory paths.
set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules)
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)
set(CLASSES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Classes)
set(RES_PATH ${SDK_ROOT_PATH}/Samples/Resources)

# Set project.
project(${APP_NAME})

# Set Visual Studio startup project.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})

# Define output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${APP_NAME})
# Set configuration (Release and Debug only).
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)
# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION ON)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add cocos libs.
include(CocosBuildSet)
add_subdirectory(
  ${COCOS2DX_ROOT_PATH}/cocos
  ${ENGINE_BINARY_PATH}/cocos/core
)

# Record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

# Add resource directories.
set(GAME_RES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${RES_PATH})
if(APPLE OR WINDOWS)
  cocos_mark_multi_resources(common_res_files
    RES_TO Resources FOLDERS ${GAME_RES_FOLDER}
  )
endif()

# Add cross-platforms source files and header files.
list(APPEND GAME_SOURCE
  ${CLASSES_PATH}/AppDelegate.cpp
  ${CLASSES_PATH}/LAppAllocator.cpp
  ${CLASSES_PATH}/LAppDefine.cpp
  ${CLASSES_PATH}/LAppLive2DManager.cpp
  ${CLASSES_PATH}/LAppModel.cpp
  ${CLASSES_PATH}/LAppPal.cpp
  ${CLASSES_PATH}/LAppSprite.cpp
  ${CLASSES_PATH}/LAppView.cpp
  ${CLASSES_PATH}/SampleScene.cpp
  ${CLASSES_PATH}/TouchManager.cpp
)

list(APPEND GAME_HEADER
  ${CLASSES_PATH}/AppDelegate.h
  ${CLASSES_PATH}/AppMacros.h
  ${CLASSES_PATH}/LAppAllocator.hpp
  ${CLASSES_PATH}/LAppDefine.hpp
  ${CLASSES_PATH}/LAppLive2DManager.hpp
  ${CLASSES_PATH}/LAppModel.hpp
  ${CLASSES_PATH}/LAppPal.hpp
  ${CLASSES_PATH}/LAppSprite.hpp
  ${CLASSES_PATH}/LAppView.hpp
  ${CLASSES_PATH}/SampleScene.h
  ${CLASSES_PATH}/TouchManager.h
)

# Add Cubism Core as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Specify Cubism Core library path for each platform and add source files.
if(ANDROID)
  list(APPEND GAME_SOURCE
    proj.android/app/jni/demo/main.cpp
  )
  set_target_properties(Live2DCubismCore
    PROPERTIES
      IMPORTED_LOCATION
        ${CORE_PATH}/lib/android/${ANDROID_ABI}/libLive2DCubismCore.a
  )

elseif(LINUX)
  list(APPEND GAME_SOURCE
    proj.linux/src/main.cpp)
  set_target_properties(Live2DCubismCore
    PROPERTIES
      IMPORTED_LOCATION
        ${CORE_PATH}/lib/linux/x86_64/libLive2DCubismCore.a
  )

elseif(WINDOWS)
  list(APPEND GAME_HEADER
    proj.win/src/main.h
    proj.win/src/resource.h
  )
  list(APPEND GAME_SOURCE
    proj.win/src/main.cpp
    proj.win/src/game.rc
    ${common_res_files}
  )

  # Detect compiler.
  if(MSVC_VERSION MATCHES 1900)
    # Visual Studio 2015
    set(MSVC_NUM 140)
  elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
    # Visual Studio 2017
    set(MSVC_NUM 141)
  elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
    # Visual Studio 2019
    set(MSVC_NUM 142)
  elseif(MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1940)
    # Visual Studio 2022
    set(MSVC_NUM 143)
  elseif(MSVC)
    message(FATAL_ERROR "[${APP_NAME}] Unsupported Visual C++ compiler used.")
  else()
    message(FATAL_ERROR "[${APP_NAME}] Unsupported compiler used.")
  endif()

  # Detect core crt.
  if(CORE_CRL_MD)
    set(CRT MD)
  else()
    set(CRT MT)
  endif()

  set_target_properties(Live2DCubismCore
    PROPERTIES
      IMPORTED_LOCATION_DEBUG
        ${CORE_PATH}/lib/windows/x86/${MSVC_NUM}/Live2DCubismCore_${CRT}d.lib
      IMPORTED_LOCATION_RELEASE
        ${CORE_PATH}/lib/windows/x86/${MSVC_NUM}/Live2DCubismCore_${CRT}.lib
  )

elseif(APPLE)
  if(IOS)
    list(APPEND GAME_HEADER
      proj.ios/src/AppController.h
      proj.ios/src/RootViewController.h
    )
    set(APP_UI_RES
      proj.ios/src/LaunchScreen.storyboard
      proj.ios/src/LaunchScreenBackground.png
      proj.ios/src/Images.xcassets
    )
    list(APPEND GAME_SOURCE
      proj.ios/src/main.m
      proj.ios/src/AppController.mm
      proj.ios/src/RootViewController.mm
      proj.ios/src/Prefix.pch
      ${APP_UI_RES}
    )

    # iPhone Simulator (i386) is not support on Cubism Core.
    if(IOS_PLATFORM STREQUAL "SIMULATOR")
      message(FATAL_ERROR
        "[CubismNativeSamples] Cubism Core does not support i386 iOS architecture."
      )
    endif()

    set_target_properties(Live2DCubismCore
      PROPERTIES
        IMPORTED_LOCATION_DEBUG
          ${CORE_PATH}/lib/ios/Debug-iphoneos/libLive2DCubismCore.a
        IMPORTED_LOCATION_RELEASE
          ${CORE_PATH}/lib/ios/Release-iphoneos/libLive2DCubismCore.a
    )

  elseif(MACOSX)
    set(APP_UI_RES
      proj.mac/src/Icon.icns
      proj.mac/src/Info.plist
    )
    list(APPEND GAME_SOURCE
      proj.mac/src/main.cpp
      proj.mac/src/Prefix.pch
      ${APP_UI_RES}
    )
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    set_target_properties(Live2DCubismCore
      PROPERTIES
        IMPORTED_LOCATION
          ${CORE_PATH}/lib/macos/${ARCHITECTURE}/libLive2DCubismCore.a
    )

  endif()
  list(APPEND GAME_SOURCE ${common_res_files})
endif()
# Set core include directory.
set_target_properties(Live2DCubismCore
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE Cocos2d)
# Add Cubism Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add definitions for Cubism Framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_COCOS)
use_cocos2dx_compile_define(Framework)
use_cocos2dx_compile_options(Framework)

# Mark app complie info and libs info.
set(all_code_files ${GAME_HEADER} ${GAME_SOURCE})
if(NOT ANDROID)
  add_executable(${APP_NAME} ${all_code_files})
else()
  add_library(${APP_NAME} SHARED ${all_code_files})
  add_subdirectory(
    ${COCOS2DX_ROOT_PATH}/cocos/platform/android
    ${ENGINE_BINARY_PATH}/cocos/platform
  )
  target_link_libraries(${APP_NAME}
    -Wl,--whole-archive
    cpp_android_spec
    -Wl,--no-whole-archive
  )
endif()

# Link libraries.
target_link_libraries(${APP_NAME} Framework Live2DCubismCore cocos2d)
# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${CLASSES_PATH})

# Mark app resources and add definitions for Cubism Framework.
setup_cocos_app_config(${APP_NAME})
if(APPLE)
  set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
  if(MACOSX)
    set_xcode_property(${APP_NAME} INFOPLIST_FILE "${COCOS2DX_ROOT_PATH}/templates/cpp-template-default/proj.ios_mac/mac/Info.plist")
    target_compile_definitions(Framework PUBLIC CSM_TARGET_MAC_GL)
    set(COCOS_GLFW3 ${COCOS2DX_ROOT_PATH}/external/glfw3)
    target_include_directories(Framework PUBLIC ${COCOS_GLFW3}/include/Mac)
    target_include_directories(Framework PUBLIC ${COCOS2DX_ROOT_PATH}/cocos)
  elseif(IOS)
    # Add information aboud XCode project.
    set_xcode_property(${APP_NAME} INFOPLIST_FILE "${COCOS2DX_ROOT_PATH}/templates/cpp-template-default/proj.ios_mac/ios/Info.plist")
    set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
    set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.live2d.demo-4-r6-cocos2dx")
    set_xcode_property(${APP_NAME} TARGETED_DEVICE_FAMILY "1,2")
    set_xcode_property(${APP_NAME} ONLY_ACTIVE_ARCH "NO")
    set_xcode_property(${APP_NAME} ARCHS "arm64")
    target_compile_definitions(Framework PUBLIC CSM_TARGET_IPHONE_ES2)
    target_include_directories(Framework PUBLIC ${COCOS2DX_ROOT_PATH}/cocos)
  endif()
elseif(WINDOWS)
  cocos_copy_target_dll(${APP_NAME})
  target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
  # Specify glew directories for Cubism Framework.
  set(COCOS_GLEW ${COCOS2DX_ROOT_PATH}/external/win32-specific/gles)
  set(COCOS_GLFW3 ${COCOS2DX_ROOT_PATH}/external/glfw3)
  target_include_directories(Framework PUBLIC ${COCOS_GLEW}/include/OGLES)
  target_include_directories(Framework PUBLIC ${COCOS_GLFW3}/include/win32)
  target_include_directories(Framework PUBLIC ${COCOS2DX_ROOT_PATH}/cocos)
elseif(ANDROID)
  target_compile_definitions(Framework PUBLIC CSM_TARGET_ANDROID_ES2)
  target_include_directories(Framework PUBLIC ${COCOS2DX_ROOT_PATH}/cocos)
elseif(LINUX)
  set(COCOS_GLFW3 ${COCOS2DX_ROOT_PATH}/external/glfw3)
  target_include_directories(Framework PUBLIC ${COCOS_GLFW3}/include/linux)
  target_compile_definitions(Framework PUBLIC CSM_TARGET_LINUX_GL)
  target_include_directories(Framework PUBLIC ${COCOS2DX_ROOT_PATH}/cocos)
endif()

# Copy resources.
if(LINUX OR WINDOWS)
  cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
  cocos_copy_target_res(${APP_NAME}
    LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER}
  )
endif()

# When this flag is turned on, the renderer will draw for each texture that has LAppModel.
# Otherwise, will draw to the main framebuffer.
# target_compile_definitions(${APP_NAME} PRIVATE USE_MODEL_RENDER_TARGET)

# When this flag is turned on, the renderer will draw to the texture.
# The texture only has LAppView.
# This flag has higher priority than USE_MODEL_RENDER_TARGET.
# target_compile_definitions(${APP_NAME} PRIVATE USE_RENDER_TARGET)
