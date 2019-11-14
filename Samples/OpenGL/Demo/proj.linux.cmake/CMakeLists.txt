cmake_minimum_required(VERSION 3.15)

# Suppress the generation of MinSizeRel and RelWithDebInfo.
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION true)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 14)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set project name.
project(Demo)

# Set output directory paths.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Set directory paths.
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Core)
set(FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Framework)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../thirdParty)
set(STB_DIR ${THIRD_PARTY_DIR}/stb)
set(GLEW_DIR ${THIRD_PARTY_DIR}/glew-2.1.0)
set(GLFW_DIR ${THIRD_PARTY_DIR}/glfw-3.3)

# Add the sourcecodes for this project.
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
    Demo/TouchManager.cpp)

# Set preprocessor definitions for CubismNativeFramework.
set(FRAMEWORK_DEFINITIOINS -DCSM_TARGET_LINUX_GL)

# Give the thirdparty OpenGL library include paths to CubismNativeFramework.
set(FRAMEWORK_GLEW_PATH ${GLEW_DIR}/include)
set(FRAMEWORK_GLFW_PATH ${GLFW_DIR}/include)

# Add GLEW ,GLFW and CubismFramework.
add_subdirectory(${GLFW_DIR} ${CMAKE_CURRENT_BINARY_DIR}/glfw)
add_subdirectory(${GLEW_DIR}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/GLEW)
add_subdirectory(${FRAMEWORK_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Framework)

# Add preprocessor definition.
add_definitions(-DCSM_TARGET_LINUX_GL)

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
find_library(Live2DCubismCore
             NAMES Live2DCubismCore
             PATHS ${CORE_DIR}/lib/linux/x86_64
             PATH_SUFFIXES lib
             NO_DEFAULT_PATH)

# Set sourcecodes.
add_executable(Demo ${Demo})

# Set output file name.
set_target_properties(Demo PROPERTIES OUTPUT_NAME_DEBUG "Demo_Debug")

# Set libraries for linking.
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
target_link_libraries(Demo
                      Framework
                      glew_s
                      glfw
                      ${Live2DCubismCore}
                      ${OPENGL_LIBRARIES})
