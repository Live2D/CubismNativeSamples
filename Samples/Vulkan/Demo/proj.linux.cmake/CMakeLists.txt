cmake_minimum_required(VERSION 3.16)

# Set app name.
set(APP_NAME Demo)
# Set directory paths.
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)
set(THIRD_PARTY_PATH ${SDK_ROOT_PATH}/Samples/Vulkan/thirdParty)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(GLFW_PATH ${THIRD_PARTY_PATH}/glfw)
set(RES_PATH ${SDK_ROOT_PATH}/Samples/Resources)
set(SAMPLE_SHADER_PATH ${SDK_ROOT_PATH}/Samples/Vulkan/Shaders)
set(FRAMEWORK_SHADER_PATH ${FRAMEWORK_PATH}/src/Rendering/Vulkan/Shaders)
set(COMMON_SRC_PATH ${SDK_ROOT_PATH}/Samples/Common)

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

# Surpress GLFW process.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)

# Specify version of compiler.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add Cubism Core.
# Import as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
# Find library path.
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION ${CORE_PATH}/lib/linux/x86_64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Add GLFW.
add_subdirectory(${GLFW_PATH} ${CMAKE_CURRENT_BINARY_DIR}/glfw)

# Specify Cubism Framework rendering.
set(FRAMEWORK_SOURCE Vulkan)
# Add Cubism Native Framework.
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
# Link libraries to framework.
target_link_libraries(Framework Live2DCubismCore)

# Find vulkan libraries.
find_package(Vulkan REQUIRED)

# Make executable app.
add_executable(${APP_NAME})
# Add common source files.
add_subdirectory(${COMMON_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/src/Common)
# Add source files.
add_subdirectory(src)
# Add shader files.
add_subdirectory(${SAMPLE_SHADER_PATH} ${CMAKE_CURRENT_BINARY_DIR}/SampleShaders)
add_subdirectory(${FRAMEWORK_SHADER_PATH} ${CMAKE_CURRENT_BINARY_DIR}/FrameworkShaders)
add_dependencies(Framework SampleShaders)
add_dependencies(Framework FrameworkShaders)

# Link libraries to app.
target_link_libraries(${APP_NAME}
  Framework
  glfw
  ${Vulkan_LIBRARIES}
)
# Specify include directories.
target_include_directories(${APP_NAME} PRIVATE ${STB_PATH})
target_include_directories(${APP_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

# Link libraries to framework.
target_include_directories(Framework PUBLIC ${Vulkan_INCLUDE_DIRS})
# Link libraries to framework.
target_link_libraries(Framework Live2DCubismCore ${Vulkan_LIBRARIES})

# Copy resource directory to build directory.
add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/Resources
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${CMAKE_CURRENT_BINARY_DIR}/SampleShaders/compiledShaders $<TARGET_FILE_DIR:${APP_NAME}>/SampleShaders
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${CMAKE_CURRENT_BINARY_DIR}/FrameworkShaders/compiledShaders $<TARGET_FILE_DIR:${APP_NAME}>/FrameworkShaders

)

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
