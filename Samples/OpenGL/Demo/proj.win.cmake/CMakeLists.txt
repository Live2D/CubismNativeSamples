cmake_minimum_required(VERSION 3.15)

# Suppress the generation of MinSizeRel and RelWithDebInfo.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# Set project name.
project(Demo)

# Mark as Startup project.
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Demo)

# Set output directory paths.
if(CMAKE_GENERATOR MATCHES "^Visual Studio")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Make sure the linker flag for architecture detection.
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    set(WIN_64 TRUE)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
    set(WIN_64 FALSE)
else()
    message(FATAL_ERROR "[CubismNativeSamples] Invalid linker flag.")
endif()

# Set library directory paths.
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core)
set(FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Framework)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdParty)
set(STB_DIR ${THIRDPARTY_DIR}/stb)
set(GLEW_DIR ${THIRDPARTY_DIR}/glew-2.1.0)
if(${WIN_64})
    set(GLFW_DIR ${THIRDPARTY_DIR}/glfw-3.3.bin.WIN64)

    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86_64)
    set(GLEW_LIB_DIR ${GLEW_DIR}/lib/Release/x64)
    set(GLFW_LIB_DIR ${GLFW_DIR})
else()
    set(GLFW_DIR ${THIRDPARTY_DIR}/glfw-3.3.bin.WIN32)

    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86)
    set(GLEW_LIB_DIR ${GLEW_DIR}/lib/Release/Win32)
    set(GLFW_LIB_DIR ${GLFW_DIR})
endif()

# Add the sourcecodes for this project.
set(SOURCES
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
    Demo/TouchManager.cpp)

# Set preprocessor definitions for CubismNativeFramework.
set(FRAMEWORK_DEFINITIOINS -DCSM_TARGET_WIN_GL -DGLEW_STATIC)

# Give the thirdparty OpenGL library include paths to CubismNativeFramework.
set(FRAMEWORK_GLEW_PATH ${GLEW_DIR}/include)
set(FRAMEWORK_GLFW_PATH ${GLFW_DIR}/include)

# Add CubismNativeFramework.
add_subdirectory(${FRAMEWORK_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Framework)

# Add preprocessor definition.
add_definitions(-DCSM_TARGET_WIN_GL -DGLEW_STATIC)

# When this flag is turned on, the renderer will draw for each texture that has LAppModel.
# Otherwise, will draw to the main framebuffer.
#add_definitions(-DUSE_MODEL_RENDER_TARGET)
# When this flag is turned on, the renderer will draw to the texture. The texture only has LAppView.
# This flag has higher priority than DUSE_MODEL_RENDER_TARGET.
#add_definitions(-DUSE_RENDER_TARGET)

# Add include paths.
include_directories(
    ${CORE_DIR}/include
    ${FRAMEWORK_DIR}/src
    ${STB_DIR}/include
    ${GLEW_DIR}/include
    ${GLFW_DIR}/include)

# Add library paths.
if(MSVC_VERSION MATCHES 1800)
    # Visual Studio 2013
    link_directories(
        ${CORE_LIB_DIR}/120
        ${GLEW_LIB_DIR}
        ${GLFW_LIB_DIR}/lib-vc2013)
elseif(MSVC_VERSION MATCHES 1900)
    # Visual Studio 2015
    link_directories(
        ${CORE_LIB_DIR}/140
        ${GLEW_LIB_DIR}
        ${GLFW_LIB_DIR}/lib-vc2015)
elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
    # Visual Studio 2017
    link_directories(
        ${CORE_LIB_DIR}/141
        ${GLEW_LIB_DIR}
        ${GLFW_LIB_DIR}/lib-vc2017)
elseif(MSVC)
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported Visual C++ compiler used.")
else()
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported compiler used.")
endif()

# Solve the MSVCRT confliction.
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmtd.lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")

# Set sourcecodes.
add_executable(Demo ${SOURCES})

# Set output file name.
set_target_properties(Demo PROPERTIES OUTPUT_NAME_DEBUG "Demo_Debug")

# Set libraries for linking.
target_link_libraries(Demo
                      Framework
                      glew32s.lib
                      glfw3.lib
                      opengl32.lib
                      debug Live2DCubismCore_MTd.lib
                      optimized Live2DCubismCore_MT.lib)
