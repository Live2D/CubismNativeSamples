cmake_minimum_required(VERSION 3.16)

# Set app name.
set(APP_NAME Demo)
# Set directory paths.
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)
set(THIRD_PARTY_PATH ${SDK_ROOT_PATH}/Samples/OpenGL/thirdParty)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(RES_PATH ${SDK_ROOT_PATH}/Samples/Resources)

# Set project.
project(${APP_NAME})

# Define output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${APP_NAME})
# Set configuration (Release and Debug only).
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)
# Suppress generation of ZERO_CHECK project.
set(CMAKE_SUPPRESS_REGENERATION ON)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")

# Detect platform.
if(PLATFORM STREQUAL "OS64")
  set(PLATFORM_NAME iphoneos)
elseif(PLATFORM STREQUAL "SIMULATOR64")
  set(PLATFORM_NAME iphonesimulator)
else()
  message(FATAL_ERROR "Invalid PLATFORM ${PLATFORM_NAME}.")
endif()

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Set library path and inlude path.
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION_DEBUG
      ${CORE_PATH}/lib/ios/Debug-${PLATFORM_NAME}/libLive2DCubismCore.a
    IMPORTED_LOCATION_RELEASE
      ${CORE_PATH}/lib/ios/Release-${PLATFORM_NAME}/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE OpenGL)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Add rendering definition to framework.
target_compile_definitions(Framework PUBLIC CSM_TARGET_IPHONE_ES2)
# Add include path of GLEW to framework.
target_include_directories(Framework PUBLIC ${GLEW_PATH}/include)
# Link libraries to framework.
target_link_libraries(Framework Live2DCubismCore)

# Find libraries.
find_library(COREGRAPHICS_LIBRARY CoreGraphics)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(GLKIT_LIBRARY GLKit)
find_library(OPENGLES_LIBRARY OpenGLES)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(UIKIT_LIBRARY UIKit)

# Make executable app.
add_executable(${APP_NAME})
# Add source files.
add_subdirectory(src)
# Link libraries to app.
target_link_libraries(${APP_NAME}
  Framework
  ${COREGRAPHICS_LIBRARY}
  ${FOUNDATION_LIBRARY}
  ${GLKIT_LIBRARY}
  ${OPENGLES_LIBRARY}
  ${QUARTZCORE_LIBRARY}
  ${UIKIT_LIBRARY}
)
# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${STB_PATH})

# Copy resource directory to build directory.
# NOTE: "Resources" is a reserved word of Xcode.
# Therefore, copy destination directory name to "Res".
add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/Res
)

# Add information aboud XCode project.
set_xcode_property(${APP_NAME}
  INFOPLIST_FILE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist"
  "All"
)
set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "" "All")
set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer" "All")
set_xcode_property(${APP_NAME}
  PRODUCT_BUNDLE_IDENTIFIER "com.live2d.demo-4-r7" "All"
)
set_xcode_property(${APP_NAME} TARGETED_DEVICE_FAMILY "1,2" "All")
if(PLATFORM STREQUAL "OS64")
  set_xcode_property(${APP_NAME} ARCHS "arm64" "All")
elseif(PLATFORM STREQUAL "SIMULATOR64")
  set_xcode_property(${APP_NAME} ARCHS "x86_64" "All")
endif()

# You can change target that renderer draws by enabling following definition.
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
