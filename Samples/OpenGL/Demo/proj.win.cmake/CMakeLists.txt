cmake_minimum_required(VERSION 3.10)

# 次の行を追加するとMinSizeRelとRelWithDebInfoの選択肢を生成することが抑制できます。
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# ZERO_CHECKプロジェクトの生成を抑制
set(CMAKE_SUPPRESS_REGENERATION true) 

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
    Demo/main.cpp
    Demo/LAppAllocator.cpp
    Demo/LAppDefine.cpp
    Demo/LAppDelegate.cpp
    Demo/LAppLive2DManager.cpp
    Demo/LAppModel.cpp
    Demo/LAppPal.cpp
    Demo/LAppSprite.cpp
    Demo/LAppTextureManager.cpp
    Demo/LAppView.cpp
    Demo/TouchManager.cpp
)

# Framework側のCMakeFielsのプリプロセッサ定義
set(FRAMEWORK_DEFINITIOINS
    -DCSM_TARGET_WIN_GL
    -DGLEW_STATIC
)

# Framework側のCMakeFilesに渡すgl系のパスを変数にセットしておく
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    set(FRAMEWORK_GLFW_PATH ../Samples/OpenGL/thirdParty/glfw-3.2.1.bin.WIN64/include)
    set(FRAMEWORK_GLEW_PATH ../Samples/OpenGL/thirdParty/glew-2.1.0/include)
else ()
    set(FRAMEWORK_GLFW_PATH ../Samples/OpenGL/thirdParty/glfw-3.2.1.bin.WIN32/include)
    set(FRAMEWORK_GLEW_PATH ../Samples/OpenGL/thirdParty/glew-2.1.0/include)
endif ()

# Frameworkの追加
add_subdirectory(../../../../Framework ${CMAKE_CURRENT_BINARY_DIR}/Framework)


# プリプロセッサ定義の追加
add_definitions(-DCSM_TARGET_WIN_GL -DGLEW_STATIC)

# ここを有効にすると、Cubismモデルをメインフレームバッファではなく各LAppModelが独自に確保するテクスチャへ描画する 
#add_definitions(-DUSE_MODEL_RENDER_TARGET)
# ここを有効にすると、CubismモデルをメインフレームバッファではなくLAppViewが一つだけ確保するテクスチャへ描画する なおDUSE_MODEL_RENDER_TARGETより優先される 
#add_definitions(-DUSE_RENDER_TARGET)


# includeパスの追加
include_directories(../../../../Framework/src)
include_directories(../../../../Core/include)
include_directories(../../thirdParty/stb/include)
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    set(GLFW_HEADER_PATH ../../thirdParty/glfw-3.2.1.bin.WIN64/include)
    set(GLEW_HEADER_PATH ../../thirdParty/glew-2.1.0/include)
else ()
    set(GLFW_HEADER_PATH ../../thirdParty/glfw-3.2.1.bin.WIN32/include)
    set(GLEW_HEADER_PATH ../../thirdParty/glew-2.1.0/include)
endif ()
include_directories("${GLFW_HEADER_PATH}")
include_directories("${GLEW_HEADER_PATH}")


# ライブラリパスの追加

# Visual Studioのソリューション生成
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" AND CMAKE_GENERATOR MATCHES "Visual Studio 14 2015")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/140)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN64/lib-vc2015)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/x64)

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" AND CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/120)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN64/lib-vc2013)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/x64)

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" AND CMAKE_GENERATOR MATCHES "Visual Studio 14 2015")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/140)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN32/lib-vc2015)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/Win32)

elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32" AND CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/120)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN32/lib-vc2013)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/Win32)


# NMake MakeFiles指定
elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1900 AND CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64" )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/140)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN64/lib-vc2015)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/x64)

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1900 )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/140)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN32/lib-vc2015)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/Win32)

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1800 AND CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64" )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86_64/120)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN64/lib-vc2013)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/x64)

elseif (CMAKE_GENERATOR MATCHES "NMake Makefiles" AND MSVC_VERSION MATCHES 1800 )
    link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/windows/x86/120)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glfw-3.2.1.bin.WIN32/lib-vc2013)
    link_directories(${CMAKE_SOURCE_DIR}/../../thirdParty/glew-2.1.0/lib/Release/Win32)
endif ()    


# 競合するライブラリのWarning対策
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRT")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")


# ソースの追加
add_executable(Demo ${Demo})

# 出力ファイル名の指定
if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    SET_TARGET_PROPERTIES(Demo PROPERTIES OUTPUT_NAME "Demo_x64")
endif ()

# ライブラリの追加
target_link_libraries(Demo opengl32.lib)
target_link_libraries(Demo glew32s.lib)
target_link_libraries(Demo glfw3.lib)
target_link_libraries(Demo Live2DCubismCore_MT.lib)
target_link_libraries(Demo Framework)