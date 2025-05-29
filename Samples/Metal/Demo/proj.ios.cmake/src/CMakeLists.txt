target_sources(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AppDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AppDelegate.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppLive2DManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppLive2DManager.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppModelSprite.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppModelSprite.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/main.m
    ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewController.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/MetalUIView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MetalUIView.m
    ${CMAKE_CURRENT_SOURCE_DIR}/MetalView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MetalView.m
    ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Framework/src/Rendering/Metal/MetalShaders.metal
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Shaders PROPERTIES MACOSX_PACKAGE_LOCATION Shaders)

set(RESOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Framework/src/Rendering/Metal/MetalShaders.metal)

set_target_properties(${APP_NAME} PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_FRAMEWORK_IDENTIFIER org.cmake.ExecutableTarget
  RESOURCE "${RESOURCE_FILES}"
)
