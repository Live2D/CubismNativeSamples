if (CSM_MINIMUM_DEMO)
  target_sources(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainMinimum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismSprite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismSprite.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismSpriteShader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismSpriteShader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismTextureManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismTextureManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismUserModelExtend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismUserModelExtend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDirectXView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDirectXView.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDirectXRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDirectXRenderer.hpp
  )
else ()
  target_sources(${APP_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppDelegate.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppDelegate.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppLive2DManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppLive2DManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSpriteShader.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSpriteShader.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  )
endif ()

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
