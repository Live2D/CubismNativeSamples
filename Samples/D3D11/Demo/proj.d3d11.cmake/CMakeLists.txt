cmake_minimum_required(VERSION 3.15)

# Suppress the generation of MinSizeRel and RelWithDebInfo.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION ON)

# Set project name.
project(Demo)

# Mark as Startup project.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)

# Set output directory paths.
if(CMAKE_GENERATOR MATCHES "^Visual Studio")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

option(CORE_CRL_MD "Use Cubism Core that is multithread-specific and DLL-specific version" OFF)

# Set library directory paths.
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core)
set(FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Framework)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdParty)
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86_64)
    set(DIRECTX_LIB_DIR x64)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86)
    set(DIRECTX_LIB_DIR Win32)
else()
    message(FATAL_ERROR "[CubismNativeSamples] Invalid linker flag.")
endif()

# Add the sourcecodes for this project.
set(SOURCES
    Demo/LAppAllocator.cpp
    Demo/LAppDefine.cpp
    Demo/LAppDelegate.cpp
    Demo/LAppPal.cpp
    Demo/LAppSprite.cpp
    Demo/LAppTextureManager.cpp
    Demo/LAppView.cpp
    Demo/LAppLive2DManager.cpp
    Demo/LAppModel.cpp
    Demo/TouchManager.cpp
    Demo/main.cpp)

set(FRAMEWORK_SOURCE D3D11)
set(FRAMEWORK_DX11_INCLUDE_PATH ${THIRDPARTY_DIR}/DirectXTK/Inc)

# Add CubismNativeFramework.
add_subdirectory(${FRAMEWORK_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Framework)


# includeパスの追加
include_directories(
    ${FRAMEWORK_DIR}/src
    ${CORE_DIR}/include
    ${FRAMEWORK_DX11_INCLUDE_PATH})

# When this flag is turned on, the renderer will draw for each texture that has LAppModel.
# Otherwise, will draw to the main framebuffer.
#add_definitions(-DUSE_MODEL_RENDER_TARGET)
# When this flag is turned on, the renderer will draw to the texture. The texture only has LAppView.
# This flag has higher priority than DUSE_MODEL_RENDER_TARGET.
#add_definitions(-DUSE_RENDER_TARGET)

# Add include paths.
include_directories(
    ${CORE_DIR}/include
    ${FRAMEWORK_DIR}/src)

#Add library paths.
if(MSVC_VERSION MATCHES 1800)
    # Visual Studio 2013
    link_directories(
        ${CORE_LIB_DIR}/120
        ${THIRDPARTY_DIR}/DirectXTK/Bin/Desktop_2013/${DIRECTX_LIB_DIR}/${CMAKE_BUILD_TYPE})
elseif(MSVC_VERSION MATCHES 1900)
    # Visual Studio 2015
    link_directories(
        ${CORE_LIB_DIR}/140
        ${THIRDPARTY_DIR}/DirectXTK/Bin/Desktop_2015/${DIRECTX_LIB_DIR}/${CMAKE_BUILD_TYPE})
elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
    # Visual Studio 2017
    link_directories(
        ${CORE_LIB_DIR}/141
        ${THIRDPARTY_DIR}/DirectXTK/Bin/Desktop_2017/${DIRECTX_LIB_DIR}/${CMAKE_BUILD_TYPE})
elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
    # Visual Studio 2019
    link_directories(
        ${CORE_LIB_DIR}/142
        ${THIRDPARTY_DIR}/DirectXTK/Bin/Desktop_2019/${DIRECTX_LIB_DIR}/${CMAKE_BUILD_TYPE})
elseif(MSVC)
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported Visual C++ compiler used.")
else()
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported compiler used.")
endif()

# Add parallel build flag.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

# Solve the MSVCRT confliction.
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmtd.lib /SUBSYSTEM:WINDOWS")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib /SUBSYSTEM:WINDOWS")
# Remove /SUBSYSTEM:console
set(CMAKE_CREATE_CONSOLE_EXE "")

# Set sourcecodes.
add_executable(Demo ${SOURCES})

# Set output file name.
set_target_properties(Demo PROPERTIES OUTPUT_NAME_DEBUG "Demo_Debug")


# Set libraries for linking.
target_link_libraries(Demo
                      Framework
                      d3d11.lib
                      d3dcompiler.lib
                      DirectXTK.lib)

if(CORE_CRL_MD)
    target_link_libraries(Demo
                          debug Live2DCubismCore_MDd.lib
                          optimized Live2DCubismCore_MD.lib)
else()
    target_link_libraries(Demo
                          debug Live2DCubismCore_MTd.lib
                          optimized Live2DCubismCore_MT.lib)
endif()
