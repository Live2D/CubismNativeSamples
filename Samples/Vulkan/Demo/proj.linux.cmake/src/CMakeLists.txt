if (CSM_MINIMUM_DEMO)
  target_sources(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppDefine.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppPal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainMinimum.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismUserModelExtend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismUserModelExtend.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VulkanManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VulkanManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MouseActionManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MouseActionManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainManager.hpp
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
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/VulkanManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/VulkanManager.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainManager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/SwapchainManager.hpp
  )
endif ()

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
