if (CSM_MINIMUM_DEMO)
  target_sources(${APP_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/CubismSampleViewMatrix_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/CubismSampleViewMatrix_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/MouseActionManager_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/MouseActionManager_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager_Common.hpp
  )
else ()
  target_sources(${APP_NAME}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppAllocator_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppModel_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppSprite_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppTextureManager_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppView_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppWavFileHandler_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/LAppWavFileHandler_Common.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager_Common.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/TouchManager_Common.hpp
  )
endif ()


target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
