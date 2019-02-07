cmake_minimum_required(VERSION 3.10)

# 次の行を追加するとMinSizeRelとRelWithDebInfoの選択肢を生成することが抑制できます。
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# ZERO_CHECKプロジェクトの生成を抑制
set(CMAKE_SUPPRESS_REGENERATION true) 

# compilerのバージョン指定
SET(CMAKE_CXX_STANDARD 14)

# プロジェクト名
project(Demo)

# 出力フォルダの指定
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

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
    -DCSM_TARGET_MAC_GL
    -DGLEW_STATIC
)

# Framework側のCMakeFilesに渡すgl系のパスを変数にセットしておく
set(FRAMEWORK_GLFW_PATH /usr/local/include)
set(FRAMEWORK_GLEW_PATH /usr/local/include)

# Frameworkの追加
add_subdirectory(../../../../Framework ${CMAKE_CURRENT_BINARY_DIR}/Framework)


# プリプロセッサ定義の追加
add_definitions(-DCSM_TARGET_MAC_GL -DGLEW_STATIC)

# ここを有効にすると、Cubismモデルをメインフレームバッファではなく各LAppModelが独自に確保するテクスチャへ描画する 
#add_definitions(-DUSE_MODEL_RENDER_TARGET)
# ここを有効にすると、CubismモデルをメインフレームバッファではなくLAppViewが一つだけ確保するテクスチャへ描画する なおDUSE_MODEL_RENDER_TARGETより優先される 
#add_definitions(-DUSE_RENDER_TARGET)

# includeパスの追加
include_directories(../../../../Framework/src)
include_directories(../../../../Core/include)
include_directories(../../thirdParty/stb/include)
include_directories(/usr/local/include)

# ライブラリパスの追加

# Visual Studioのソリューション生成
link_directories(${CMAKE_SOURCE_DIR}/../../../../Core/lib/macos)
link_directories(/usr/local/lib)

# ソースの追加
add_executable(Demo ${Demo})

# ライブラリの追加
find_package(OpenGL REQUIRED)
target_link_libraries(Demo ${OPENGL_LIBRARIES})
target_link_libraries(Demo libGLEW.2.1.dylib)
target_link_libraries(Demo libglfw.3.dylib)
target_link_libraries(Demo Live2DCubismCore.a)
target_link_libraries(Demo Framework)