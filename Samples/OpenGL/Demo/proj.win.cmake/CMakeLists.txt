cmake_minimum_required(VERSION 3.15)

# Suppress the generation of MinSizeRel and RelWithDebInfo.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION TRUE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

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
set(STB_DIR ${THIRDPARTY_DIR}/stb)
set(GLEW_DIR ${THIRDPARTY_DIR}/glew-2.1.0)
set(GLFW_DIR ${THIRDPARTY_DIR}/glfw-3.3)

# Make sure the linker flag for architecture detection.
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86_64)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
    set(CORE_LIB_DIR ${CORE_DIR}/lib/windows/x86)
else()
    message(FATAL_ERROR "[CubismNativeSamples] Invalid linker flag.")
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

# Add GLEW and GLFW
add_subdirectory(${GLFW_DIR} ${CMAKE_CURRENT_BINARY_DIR}/glfw)
add_subdirectory(${GLEW_DIR}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/GLEW)

# Add preprocessor definition.
add_definitions(-DCSM_TARGET_WIN_GL)

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
    link_directories(${CORE_LIB_DIR}/120)
elseif(MSVC_VERSION MATCHES 1900)
    # Visual Studio 2015
    link_directories(${CORE_LIB_DIR}/140)
elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
    # Visual Studio 2017
    link_directories(${CORE_LIB_DIR}/141)
elseif(MSVC)
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported Visual C++ compiler used.")
else()
    message(FATAL_ERROR "[CubismNativeSamples] Unsupported compiler used.")
endif()

# Add parallel build flag.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

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
                      glfw
                      glew_s
                      opengl32.lib)

if(CORE_CRL_MD)
    target_link_libraries(Demo
                          debug Live2DCubismCore_MDd.lib
                          optimized Live2DCubismCore_MD.lib)
else()
    target_link_libraries(Demo
                          debug Live2DCubismCore_MTd.lib
                          optimized Live2DCubismCore_MT.lib)
endif()
