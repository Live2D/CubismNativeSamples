cmake_minimum_required(VERSION 3.10)

# 次の行を追加するとMinSizeRelとRelWithDebInfoの選択肢を生成することが抑制できます。
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# ZERO_CHECKプロジェクトの生成を抑制
set(CMAKE_SUPPRESS_REGENERATION true) 

# Frameworkのファイル設定 
set(FRAMEWORK_SOURCE "D3D9")

# プロジェクト名
project(Demo)

# 作成したプロジェクトをスタートアッププロジェクトに変更する
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo )

# 出力フォルダの指定
if (CMAKE_GENERATOR STREQUAL "NMake Makefiles")
    #デフォルトのまま
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

# ここにプロジェクトに追加するソースファイルを追加
set(Demo
    Demo/LAppAllocator.cpp
    Demo/LAppAllocator.hpp
    Demo/LAppDefine.cpp
    Demo/LAppDefine.hpp
    Demo/LAppDelegate.cpp
    Demo/LAppDelegate.hpp
    Demo/LAppPal.cpp
    Demo/LAppPal.hpp
    Demo/LAppSprite.cpp
    Demo/LAppSprite.hpp
    Demo/LAppTextureManager.cpp
    Demo/LAppTextureManager.hpp
    Demo/LAppView.cpp
    Demo/LAppView.hpp
    Demo/LAppLive2DManager.cpp
    Demo/LAppLive2DManager.hpp
    Demo/LAppModel.cpp
    Demo/LAppModel.hpp
    Demo/TouchManager.cpp
    Demo/TouchManager.hpp
    Demo/main.cpp
)

set(FRAMEWORK_DX9_INCLUDE_PATH "$ENV{DXSDK_FOR_CUBISM_D3D9}/Include")

# Frameworkの追加
add_subdirectory(../../../../Framework ${CMAKE_CURRENT_BINARY_DIR}/Framework)


# includeパスの追加
include_directories(../../../../Framework/src)
include_directories(../../../../Core/include)

include_directories("${FRAMEWORK_DX9_INCLUDE_PATH}")

# ここを有効にすると、Cubismモデルをメインフレームバッファではなく各LAppModelが独自に確保するテクスチャへ描画する 
#add_definitions(-DUSE_MODEL_RENDER_TARGET)
# ここを有効にすると、CubismモデルをメインフレームバッファではなくLAppViewが一つだけ確保するテクスチャへ描画する なおDUSE_MODEL_RENDER_TARGETより優先される 
#add_definitions(-DUSE_RENDER_TARGET)


# ライブラリパスの追加

# Visual Studioのソリューション生成
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" AND CMAKE_GENERATOR MATCHES "Visual Studio 14 2015")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/140)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x64")

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" AND CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/120)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x64")

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" AND CMAKE_GENERATOR MATCHES "Visual Studio 14 2015")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/140)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x86")

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" AND CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/120)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x86")


# NMake MakeFiles指定
elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1900 AND CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64" )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/140)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x64")

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1900 )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/140)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x86")

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1800 AND CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64" )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/120)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x64")

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1800 )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/120)
    link_directories("$ENV{DXSDK_FOR_CUBISM_D3D9}/Lib/x86")
endif ()

# ソースの追加
add_executable(Demo ${Demo})

# WinMainを使用
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
# CMakeの設定によっては/SUBSYSTEM:consoleがセットされてしまうので消す 
set(CMAKE_CREATE_CONSOLE_EXE "")

# 出力ファイル名の指定1
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    SET_TARGET_PROPERTIES(Demo PROPERTIES OUTPUT_NAME "Demo_x64")
endif ()

# ライブラリの追加
target_link_libraries(Demo d3d9.lib)
target_link_libraries(Demo d3dx9.lib)

if (CMAKE_BUILD_TYPE MATCHES "Release" )
target_link_libraries(Demo Live2DCubismCore_MD.lib)
elseif (CMAKE_BUILD_TYPE MATCHES "Debug" )
target_link_libraries(Demo Live2DCubismCore_MDd.lib)
else ()
target_link_libraries(Demo debug Live2DCubismCore_MDd.lib)
target_link_libraries(Demo optimized Live2DCubismCore_MD.lib)
endif ()

target_link_libraries(Demo Framework)
