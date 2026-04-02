# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).


## [5-r.5] - 2026-04-02

### Added

* Add support for rendering portrait displays on iOS samples.
* Add support for resizing windows on iPad in iOS samples.

### Changed

* Change Vulkan samples to use `CubismDeviceInfo_Vulkan` instead of singletons.
* Migrated the OpenGL and Metal iOS samples to the scene‑based lifecycle.
* Change to use Vulkan validation layers when the `CSM_DEBUG` preprocessor is defined.
* Change motion calculation order updates to be performed by `CubismUpdateScheduler`.
* Change to configure the parameter IDs and settings used for target tracking in `CubismLook`.
* Change Vulkan sampler settings to align with other graphics APIs.

### Fixed

* Fix a validation error on Vulkan.
* Fix memory leak when resizing window with Metal.
* Fix a memory leak in `ViewInitialize()` where `_deviceToScreen` and `_viewMatrix` were not deleted before being re-allocated in OpenGL, D3D11 and D3D9.
* Fix a memory leak in `InitializeSprite()` where sprites were not deleted before being re-allocated on window resize in OpenGL.
* Fix a memory leak in `CreateShader()` where shader source bytes were not released after loading in D3D11 and D3D9.
* Fix a memory leak where `CubismDirectXView` singleton was not released in D3D11 and D3D9 minimum demos.
* Fix a memory leak in D3D9 minimum demo where `InitializeSprite()` was called twice during device recreation.
* Fix rendering not updating correctly after window resize when `USE_RENDER_TARGET` or `USE_MODEL_RENDER_TARGET` is enabled in OpenGL samples.
* Fix collision detection misalignment in split-screen mode on Android.
* Fix app state reset when entering split-screen mode on Android.
* Remove unnecessary matrix transpose in D3D11 sprites.
* Fix rendering process to pause in the background on Metal.
* Fix shader regeneration to be triggered when OnSurfaceCreate is called on Android OpenGL.
* Fix an issue where the application might not terminate correctly after resizing the window in Vulkan on Windows.


## [5-r.5-beta.3.1] - 2026-02-19

### Fixed

* Fix a bug where rendering results were not as expected when using Blend mode on semi-transparent objects.
  * See `CHANGELOG.md` in Framework.


## [5-r.5-beta.3] - 2026-01-29

### Added

* Add support for rendering portrait and landscape displays on Android.
* Add support for Blend mode and Offscreen drawing in Vulkan.
* Add Arm64 for iPhone Simulator from architecture support.

### Changed

* Change `LAppTextureManager_Common::Premultiply()` to static.
* Change all member functions of `LAppAllocator_Common` to public.

### Fixed

* Fix background image distortion when window size is changed in the OpenGL, D3D9, D3D11 and Metal.
* Fix the app crash caused by a device lost error when pressing Ctrl+Alt+Delete in D3D9.
* Fix an issue that caused performance degradation on Metal.
* Fix reuse of offscreen rendering buffer to avoid redundant allocations.


## [5-r.5-beta.2] - 2025-10-30

### Added

* Add support for Blend modes and Offscreen rendering.
  * In `5-r.5-beta.2`, an OpenGL renderer for Linux and HarmonyOS has been added, alongside the D3D9, D3D11 and Metal renderers.

### Changed

* Rename CubismOffscreenSurface_D3D9 class in Framework to CubismRenderTarget_D3D9 from Framework.
* Rename CubismOffscreenSurface_D3D11 class in Framework to CubismRenderTarget_D3D11 from Framework.
* Rename CubismOffscreenSurface_Metal class in Framework to CubismRenderTarget_Metal from Framework.
* Change to the implementation now enables support for multiple devices instead of a single one.
  * In Metal, the device is now managed on the sample side.

### Fixed

* Fix OpenGL renderer now supports Retina displays on macOS.
* Fix warnings occurs during build on OpenGL-Android.
* Fix rendering corruption when the `USE_MODEL_RENDER_TARGET` macro is enabled in the OpenGL sample on macOS.
* Fix an issue where LAppMinimumView::_spriteShader on OpenGL-Android was not initialized.
* Fix OpenGL background rendering issue on iOS when returning to the app from the home screen.
* Fix model distortion when resizing the window while using `USE_MODEL_RENDER_TARGET` with Metal.

### Removed

* Remove Visual Studio 2015 samples.


## [5-r.5-beta.1] - 2025-08-26

### Added

* Add `Ren` model.
* Add support for Blend mode and Offscreen drawing.
  * In `5-r.5-beta.1`, the OpenGL renderer works only on Windows, macOS, iOS, and Android. The SDK does not function in other environments. Please check `NOTICE.md`
  
### Changed

* Rename CubismOffscreenSurface_OpenGLES2 class in Framework to CubismRenderTarget_OpenGLES2 from Framework.
* Change the compile and target SDK version of Android OS to 16.0 (API 36).
  * Upgrade the version of Android Gradle Plugin from 8.6.1 to 8.9.1.
  * Upgrade the version of Gradle from 8.7 to 8.11.1.

### Fixed

* Fix unintended object recreating when resizing the window in the OpenGL sample program. by [@KhangPham9](https://github.com/Live2D/CubismNativeSamples/pull/50)


## [5-r.4.1] - 2025-07-17

### Changed

* Implement support for Android 16KB page size.

### Fixed

* Fix app crashing when shader files could not be loaded.


## [5-r.4] - 2025-05-29

### Added

* Add a feature to fix the frame rate to a specified value in Windows OpenGL.
* Add a flag to enable the function that verifies the consistency when loading `motion3.json`.

### Changed

* Move some overlapping functions to `Common`.
* Change the default version of the JDK used for compilation to be fixed at 17.
* Change the blend mode when using the USERENDERTARGET or USEMODELRENDER_TARGET macro, and apply Premultiplied Alpha to the color settings of rendering targets.
Also adjust the return value of the GetSproteAlpha function.
* Change to separate shader code from the framework layer.
* Change `LAppPal::LoadFileAsBytes()` behaviour on macOS.
  * Convert to an absolute path, if a relative path is specified.

### Fixed

* Fix the errors that occur when building in a Unicode environment with D3D9 and D3D11.
* Fix an issue where the priority was not reset if the motion was not read correctly.


## [5-r.3] - 2025-02-18

### Fixed

* Fix an issue in the GitHub repository where some files in the `thirdParty` directory were missing and excluded from tracking.


## [5-r.2] - 2024-12-19

### Added

* Add Vulkan support in Ubuntu 22.04 and AlmaLinux 9.
* Add a function to notify when motion playback starts.
* Add shader setup classes for D3D9, D3D11, OpenGL, Vulkan.
* Add the sample for OpenGL API in HarmonyOS.

### Changed

* Change to overwrite motion fade by the value specified in .model3.json on Framework.
* Change to use multi-buffering in Vulkan.
* Change the screen orientation and rotation direction to be unified across Android, iPhone, and iPad.
* Change specify the NDK version of the Android sample.
* Change to read shader source codes from files.
* Change the compile and target SDK version of Android OS to 15.0 (API 35).
  * Upgrade the version of Android Gradle Plugin from 8.1.1 to 8.6.1.
  * Upgrade the version of Gradle from 8.2 to 8.7.
  * Upgrade the version of NDK from 25.2.9519653 to 26.3.11579264.
  * Change the minimum version of Android Studio to Ladybug(2024.2.1 Patch 2).
* Change the minimum support version of Java to 8.
* Change the function for playing back expression motions from CubismExpressionMotionManager::StartMotionPriority() to CubismExpressionMotionManager::StartMotion().
* Change to use GLSurfaceView event queues to handle touch events.

### Fixed

* Fix memory leak in Vulkan.
* Fixed drawing failure when enabling `USE_RENDER_TARGET` macros in Vulkan samples.
* Fix memory leak when exit in D3D9 and D3D11.
* Fix exit error in Vulkan.
* Fix the processing of anisotropy filtering in Vulkan to match that of other renderers.
* Fix an issue on Windows where an error would occur if the model name contained certain characters.
* Implement support for `MSVC19.40` in the VS2022 build. by [@tatsuya-shiozawa](https://github.com/Live2D/CubismNativeSamples/pull/46)
* Fix an issue where a compile error occurred due to missing includes in the OpenGL iOS minimum sample.
* Fix an issue in the Android sample where the model display would reset after performing certain operations.
* Fix a memory leak in the compilation process of shader strings in OpenGL.
* Fix an issue that could cause drawing errors when the application is restored from the background.
* Fix an issue in the OpenGL sample where the textures are not released.
* Fix a warning due to the initial value of programId being NULL.
* Fix an issue that caused white edge-like objects to be drawn when enabling the `USE_RENDER_TARGET` or `USE_MODEL_RENDER_TARGET` flag in `LAppLive2DManager`.
* Fix an issue causing a decrease in rendering resolution when using render targets in the Metal sample project.
* Fix an issue where rendering would break when using the iPad with `USE_RENDER_TARGET` or `USE_MODEL_RENDER_TARGET` defined in OpenGL samples.

### Removed

* Remove Visual Studio 2013 samples.
* Remove armeabi-v7a from architecture support.
* Remove the Cocos2d-x sample project.
* Remove the callback what motion playback finishes on the minimum sample.
* Remove unnecessary declarations in the OpenGL Android sample.


## [5-r.1] - 2024-03-26

### Added

* Add x86_64 architecture to target platforms in Android.

### Changed

* Rename "rootDirectory" to "executeAbsolutePath".
* The log function was changed because it was ambiguous whether or not a newline was inserted.
* Change so that `LAppSprite` is not depend on `LAppDelegate` in D3D11 and D3D9, Metal, Vulkan.
* Some function arguments in `LAppSprite` are changed in D3D11 and D3D9, Metal, Vulkan.
* Change to read shader source codes from files.

### Fixed

* Fixed undefined behavior and memory leak in `LAppPal`.
* Modify code to more closely resemble Java coding style.
* Fix model drawing disappearing when defining `USE_RENDER_TARGET` and `USE_MODEL_RENDER_TARGET`.
* Fix an issue where the OpenGL sample for Visual Studio 2013 was not functioning correctly.
* Fix shader build error when running nmake in Vulkan.

### Removed

* Remove unnecessary pre-API 21 processing.


## [5-r.1-beta.4] - 2024-01-25

### Added

* Add `LAppWavFileHandler` class for Android and iOS.
* Add functions for MotionSyncPlugin to `LAppWavFileHandler` in Windows and macOS.

### Changed

* Change so that `LAppSprite` is not depend on `LAppDelegate` in Android.
* Change so that `ViewController` is not include on `LAppSprite` in iOS.
* Change the path acquisition process to a library function on OpenGL Mac and Linux.
* Change the compile and target SDK version of Android OS to 14.0 (API 34).
  * Upgrade the version of Android Gradle Plugin from 8.0.2 to 8.1.1.
  * Upgrade the version of Gradle from 8.1.1 to 8.2.
  * Change the minimum version of Android Studio to Hedgehog(2023.1.1).

### Fixed

* The version set in `sourceCompatibility` and `targetCompatibility` in `build.gradle`'s `compileOptions` has been lowered to `JavaVersion.VERSION_1_7`.
* The version of CMake set in `externalNativeBuild` was raised to `3.22.1`.
* Fix to check for null when reading json.
* Replace deprecated features in Android.
* Fix an issue that caused some graphics drivers to not render correctly in Vulkan.
* Fix an issue that caused incorrect selection of depth format in Vulkan.
* Fix errors that occurs when building with x86 in vulkan.


## [5-r.1-beta.3] - 2023-10-12

### Added

* Add functions for MotionSyncPlugin to `LAppWavFileHandler`.

### Changed

* Change so that `LAppSprite` is not depend on `LAppDelegate`.


## [5-r.1-beta.2] - 2023-09-28

### Added

* Add English version of README.md for Vulkan.

### Changed

* Replace the sample model `Mao` with the updated version that is compatible with Cubism 5.0.
* The minimum operating system version of Android for OpenGL has been chang to 5.0 (API 21).
  * Cocos2d-x is not supported.
* Change to disable bitcode generation for building with Xcode.

### Fixed

* Fix Metal rendering results on macOS to be similar to OpenGL.
* Fix a memory leak caused by a ViewMatrix variable. by [@COx2](https://github.com/Live2D/CubismNativeSamples/pull/40)


## [5-r.1-beta.1] - 2023-08-17

### Added

* Add Wankoromochi as a model bundled with SDK.
* Add the sample for Vulkan API in Windows.

### Changed

* Update third-party libraries.
* Offscreen drawing-related terminology has been unified with "OffscreenSurface".
* Adjusted to automatically search and use models in the Resource folder.

### Fixed

* Fix a bug that premultiplied alpha not works correctly when `PREMULTIPLIED_ALPHA_ENABLE` is not defined in Metal.


## [4-r.7] - 2023-05-25

### Added

* Add some functions for checking consistency of MOC3 files.
  * Add the function of checking consistency in `LAppModel::SetupModel()`.
  * Add the function of checking consistency before loading a model. (`LAppModel::HasMocConsistencyFromFile()`)
  * This feature is enabled by default.Please see the following manual for more information.
    * https://docs.live2d.com/cubism-sdk-manual/moc3-consistency/

### Changed

* Change so that when `USE_MODEL_RENDER_TARGET` is defined, one model will apply the opacity obtained from the motion.

### Fixed

* Fix a problem in which `haru` motion and voice were incorrect combination.
* Fix opacity of drawn models when using `USE_RENDER_TARGET` macro.


## [4-r.6.2] - 2023-03-16

### Changed

* Change to make it easier to use double and triple buffers on DirectX systems.

### Fixed

* Fix some problems related to Cubism Core.
  * See `CHANGELOG.md` in Core.


## [4-r.6.1] - 2023-03-10

### Added

* Add funciton to validate MOC3 files.
  * See `CHANGELOG.md` in Core and Framework.


## [4-r.6] - 2023-02-21

### Fixed

* Fix a link to the Japanese version of NOTICE.

### Removed

* Remove unnecessary description `glEnable(GL_TEXTURE_2D)` in OpenGL projects.


## [4-r.5.1] - 2022-09-15

### Fixed

* Fix a bug that caused a crash if an empty array existed in json.
  * See `CHANGELOG.md` in Framework.


## [4-r.5] - 2022-09-08

### Added

* Add support for Visual Studio 2022.
* Add the multilingual supported documents.

### Changed

* Update third-party libraries.

### Fixed

* Fix an issue where the window title for the minimum sample of D3D9 was that of D3D11.


## [4-r.5-beta.5] - 2022-08-04

### Changed

* Update `Mao` model.
* Change the Signing setting of the Debugging building in the samples for Android to use the `debug.keystore` in the `app` directory.

### Fixed

* Fix the GLEW version for Visual Studio 2013.
* Fix crash with exception when reading .moc3 files of unsupported versions.


## [4-r.5-beta.4.1] - 2022-07-08

### Fixed

* Fix Core API called in GetDrawableParentPartIndex function.
  * See `CHANGELOG.md` in Framework.


## [4-r.5-beta.4] - 2022-07-07

### Added

* Add `Mao` model.

### Changed

* Disable ARC in Metal projects.


## [4-r.5-beta.3] - 2022-06-16

### Fixed

* Fixed memory leak in Cocos2d-x.


## [4-r.5-beta.2] - 2022-06-02

### Fixed

* Fixed a bug that caused Multiply Color / Screen Color of different objects to be applied.
  * See `CHANGELOG.md` in Core.
  * No modifications to Samples and Framework.


## [4-r.5-beta.1] - 2022-05-19

### Changed

* Change iOS sample to work at 60FPS.
* Support Multiply Color / Screen Color added in Cubism 4.2.
  * See Framework and Core.
* Change to avoid features deprecated since API 30.
* Update libraries used in Android samples.
* Change from `Jcenter` repository to `Maven Central` repository.


## [4-r.4] - 2021-12-09

### Added

* Add the rendering options on Metal:
  * `USE_RENDER_TARGET`
  * `USE_MODEL_RENDER_TARGET`
* Add a sample project for Mac Catalyst.

### Changed

* Update sample models. (Made with Cubism Editor 4.1.02)

### Fixed

* Fixed a graphics bug in Cocos2d-x sample with `USE_RENDER_TARGET` on iOS and macOS.
* Fix opacity was not being applied when using `USE_MODEL_RENDER_TARGET`.


## [4-r.4-beta.1] - 2021-10-07

### Added

* Add the minimum sample into D3D9, D3D11, and all platforms OpenGL samples.
  * This sample support to view single model, playback for single motion and the model looks at point where screen be dragging.
* Add support for OpenGL to work on M1 Mac.
* Add the sample for Metal API in iOS.
  * There are some restrictions, see the [NOTICE.md](NOTICE.md).

### Changed

* Update Visual Studio 2017 and 2019 DirectXTK version to `jun2021`.
* Change the console to show which version of DirectXTK is being used when downloading it.
* Update ios-cmake library version to 4.2.0.

### Fixed

* Fixed a bug where the move process would affect other models while displaying multiple models. by [@catCoder](https://community.live2d.com/discussion/1043/multiple-models-when-using-translaterelative)
* Fix renderer for Cocos2d-x v4.0.
  * Rendering didn't work when using `USE_RENDER_TARGET`.
* Fix the viewport setting for Linux sample.


## [4-r.3] - 2021-06-10

## [4-r.3-beta.1] - 2021-05-13

### Added

* Add the sample for Cocos2d-x v4.0.
* Add the minimum sample of OpenGL sample for Windows.
  * This sample support to view single model, playback for single motion and the model looks at mouse cursor when mouse dragging.

### Removed

* Obsolete the sample for Cocos2d-x v3.x.

### Fixed

* Fix setup scripts for Cocos2d-x.
  * Changed from xcopy to robocopy and improved to be able to place in a deep hierarchy.
* Fixed initial window size display misalignment when scaling is set in D3D11.


## [4-r.2] - 2021-02-17

### Added

* Add the sample to manipulate the lip-sync from a waveform on the audio file(.wav) into Windows, macOS, Linux.
* Add support for Visual Studio 2019 in OpenGL Sample.
  * When using Visual Studio 2013, see the [NOTICE.md](NOTICE.md).
* Add sample voices to `Haru`.
* Add a margin to adjust position of model change icon and exit button on smartphones and other devices for Cocos2d-x Sample.

### Changed

* Optimize processing in setup scripts for third-party libraries on Windows.

### Fixed

* Adjust size calculation for models displayed in a window and fix to use a view matrix.

### Removed

* End support for Amazon Linux AMI.


## [4-r.1] - 2020-01-30

### Added

* Add Linux sample for Cocos2d-x.
* Add setup script for Cocos2d-x.

### Changed

* iOS samples and Cocos2d-x samples are generated using CMake.
* Regenerate Cocos2d-x sample using 3.17.2 template.
  * In Cocos2d-x sample, you can develop all platforms using `CmakeLists.txt`.
* Support Objective-C ARC (Automatic Reference Counting) in OpenGL iOS sample.
* Refactoring `CMakeLists.txt`.
* Change the configuration of some directories.
  * Rename `Demo` directory of each sample project to `src`.
  * Rename `Res` directory to `Resources`.
  * Move `cocos2d` directory into `thirdParty`.
  * Move shell script files to `scripts`.
* Enhance audio features of Cocos2d-x.
* Update third party libraries.
  * Update GLFW to 3.3.2 in OpenGL sample.
  * Update DirectXTK to December 17, 2019.
* Change `minSdkVersion` of Android sample to 16.
  * You can run samples on Android 4.1 or later.

### Fixed

* Fix syntax errors in compiling c++03.


## [4-beta.2.1] - 2019-11-19

This change is only for Cubism Core.
See [Core Changelog] for details.

[Core Changelog]: /Core/CHANGELOG.md


## [4-beta.2] - 2019-11-14

### Added

* Add a build configuration for Linux.
* Add support for Visual Studio 2019 in D3D9 and D3D11.
* Add a build script for macOS using makefile.

### Changed

* Improve the build settings for `GLFW` and `GLEW`.
* Add a setup script for D3D11.
* Set source file encode to UTF-8-BOM.
* Improve the batch scripts.
  Scripts can select the Cubism Core CRT option for linking.


## [4-beta.1] - 2019-09-04

### Added

* Support new Inverted Masking features.
* Add script to prepare third party libraries automatically in OpenGL samples.
* Add `.editorconfig` and `.gitattributes` to manage file formats.
* Add `CHANGELOG.md`.
* Add sample model `./Samples/Res/Rice`.

### Changed

* Update GLFW library version to 3.3.
* Update android sdk and other library version.
* Convert all file formats according to `.editorconfig`.
* Support ndk build in `arm64-v8a` and `x86` with Cocos2d-x sample.
* Rename `proj.android-studio` in `./Samples/Cocos2d-x/Demo/` to `proj.android`.
* Rename `cocos2d-x` in `./Samples/Cocos2d-x/` to `cocos2d`.
* Convert all `Readme.ja.txt` to `README.md` and adjust the appearance.
* What was `Package.json` is currently being changed to`cubism-info.yml`.


[5-r.5]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.5-beta.3.1...5-r.5
[5-r.5-beta.3.1]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.5-beta.3...5-r.5-beta.3.1
[5-r.5-beta.3]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.5-beta.2...5-r.5-beta.3
[5-r.5-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.5-beta.1...5-r.5-beta.2
[5-r.5-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.4.1...5-r.5-beta.1
[5-r.4.1]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.4...5-r.4.1
[5-r.4]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.3...5-r.4
[5-r.3]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.2...5-r.3
[5-r.2]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.1...5-r.2
[5-r.1]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.1-beta.4...5-r.1
[5-r.1-beta.4]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.1-beta.3...5-r.1-beta.4
[5-r.1-beta.3]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.1-beta.2...5-r.1-beta.3
[5-r.1-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/5-r.1-beta.1...5-r.1-beta.2
[5-r.1-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.7...5-r.1-beta.1
[4-r.7]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.6.2...4-r.7
[4-r.6.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.6.1...4-r.6.2
[4-r.6.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.6...4-r.6.1
[4-r.6]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5.1...4-r.6
[4-r.5.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5...4-r.5.1
[4-r.5]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.5...4-r.5
[4-r.5-beta.5]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.4.1...4-r.5-beta.5
[4-r.5-beta.4.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.4...4-r.5-beta.4.1
[4-r.5-beta.4]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.3...4-r.5-beta.4
[4-r.5-beta.3]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.2...4-r.5-beta.3
[4-r.5-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.5-beta.1...4-r.5-beta.2
[4-r.5-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.4...4-r.5-beta.1
[4-r.4]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.4-beta.1...4-r.4
[4-r.4-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.3...4-r.4-beta.1
[4-r.3]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.3-beta.1...4-r.3
[4-r.3-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.2...4-r.3-beta.1
[4-r.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-r.1...4-r.2
[4-r.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.2.1...4-r.1
[4-beta.2.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.2...4-beta.2.1
[4-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.1...4-beta.2
[4-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/9a61d9374317b30f99c5e0ad3e58b675a0a39a32...4-beta.1
