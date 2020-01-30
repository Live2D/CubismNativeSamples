# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).


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


[4-r.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.2.1...4-r.1
[4-beta.2.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.2...4-beta.2.1
[4-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.1...4-beta.2
[4-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/9a61d9374317b30f99c5e0ad3e58b675a0a39a32...4-beta.1
