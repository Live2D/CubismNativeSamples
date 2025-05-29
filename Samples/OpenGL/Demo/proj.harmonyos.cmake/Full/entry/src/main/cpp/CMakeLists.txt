# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)

# Set app name.
set(APP_NAME Demo)
# Set directory paths.
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../../..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)
set(THIRD_PARTY_PATH ${SDK_ROOT_PATH}/Samples/OpenGL/thirdParty)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(COMMON_SRC_PATH ${SDK_ROOT_PATH}/Samples/Common)
set(RES_PATH ${SDK_ROOT_PATH}/Samples/Resources)
set(SHADER_PATH ${SDK_ROOT_PATH}/Samples/OpenGL/Shaders/StandardES)
set(FRAMEWORK_SHADER_PATH ${FRAMEWORK_PATH}/src/Rendering/OpenGL/Shaders/StandardES)
set(COPY_RES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The flag is enabled by default, so it needs to be disabled.
add_compile_options(-Wno-error=format-security)

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION
      ${CORE_PATH}/lib/harmonyos/${OHOS_ARCH}/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE OpenGL)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_HARMONYOS_ES3)

# Make library for android app.
add_library(${APP_NAME} SHARED)
# Add common source files.
add_subdirectory(${COMMON_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/src/Common)
# Add source files.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
# Link libraries to app.
target_link_libraries(${APP_NAME}
  Framework
  Live2DCubismCore
  EGL
  GLESv3
  hilog_ndk.z
  ace_ndk.z
  ace_napi.z
  uv
  librawfile.z.so
)
# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${STB_PATH})

# Copy resource directory to build directory.
add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} ${COPY_RES_PATH}/Resources
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${SHADER_PATH} ${COPY_RES_PATH}/Shader
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${FRAMEWORK_SHADER_PATH} ${COPY_RES_PATH}/FrameworkShaders
)

#
# * USE_RENDER_TARGET
#   Renderer draws to target of LAppView.
# * USE_MODEL_RENDER_TARGET
#   Renderer draws to target of each LAppModel.
# * default
#   Renderer draws to default main framebuffer.
#
# INFO: USE_RENDER_TARGET has higher priority than USE_MODEL_RENDER_TARGET.
#
# target_compile_definitions(${APP_NAME}
#   PRIVATE
#     USE_RENDER_TARGET
#     USE_MODEL_RENDER_TARGET
# )
