# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).


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
