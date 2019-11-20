# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


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


[4-beta.2.1]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.2...4-beta.2.1
[4-beta.2]: https://github.com/Live2D/CubismNativeSamples/compare/4-beta.1...4-beta.2
[4-beta.1]: https://github.com/Live2D/CubismNativeSamples/compare/9a61d9374317b30f99c5e0ad3e58b675a0a39a32...4-beta.1
