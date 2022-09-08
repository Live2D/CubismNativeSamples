[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for Cocos2d-x

This is a sample implementation of an application implemented with Cocos2d-x.


## Development environment

| Framework | Version |
| --- | --- |
| [XCode] | 12.4 |
| [Cocos2d-x] | 4.0 (`95e5d868ce5958c0dadfc485bdda52f1bc404fe0`) |

For other development environments and operation environments, see [README.md](../../README.md) in the top directory.

Please also check the [Cocos2d-x Documentation Prerequisites].

[Cocos2d-x Documentation Prerequisites]: https://docs.cocos2d-x.org/cocos2d-x/v3/en/installation/prerequisites.html

### Regarding Cocos2d-x v4.x

With support for the Metal API since Cocos2d-x v4.0, the configuration of the Renderer has changed to make it impossible to use the OpenGLES API directly.
Please check the [Cocos2d-x Documentation](https://docs.cocos2d-x.org/cocos2d-x/v4/en/upgradeGuide/) for more information.

Along with this change, we have prepared a dedicated Cocos2d-x Renderer for the Cubism Framework in the Cocos2d-x v4.x series. Currently, each platform uses a common Renderer, and it is possible to cross-compile across macOS, Windows, Linux, iOS, and Android.


## Directory structure

```
.
├─ Demo
│  ├─ Classes       # Sources for sample projects
│  ├─ proj.android  # Android Studio project
│  ├─ proj.ios      # Additional files and build scripts for iOS
│  ├─ proj.linux    # Additional files and build scripts for Linux
│  ├─ proj.mac      # Additional files and build scripts for macOS
│  ├─ proj.win      # Additional files and build scripts for Windows
│  └─ Resources     # Resources of cocos2d-x template project
└─ thirdParty       # Third party libraries and scripts
```


## Demo

A sample that uses each function of [Cubism Native Framework]. You can play motions, set facial expressions, switch poses, set physics, etc. You can switch models from the menu button.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### Classes

Contains the source code used in the sample project.

### proj.android

Contains Android Studio projects for Android.

NOTE: You need to download the following SDKs in advance.

* Android SDK Build-Tools
* NDK
* CMake

### proj.ios

CMake project for iOS.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `proj_xcode` | Xcode project |

Follow the steps below when building.

1. Rewrite the `Executable file` in `Info.plist` stated in XCode's `Project Settings - TARGETS - Demo - Packaging - Info.plist File` to `$(EXECUTABLE_NAME)` or `Demo` (app name).

NOTICE: Cubism Core does not support the i386 architecture, so **it is not possible to build for iPhone Simulator.**

### proj.linux

CMake project for Linux.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `make_gcc` | Executable application |

Follow the steps below before building.

1. Refer to the [Dependencies that you need] documentation to download the required packages.

[Dependencies that you need]: https://docs.cocos2d-x.org/cocos2d-x/v4/en/installation/Linux.html#dependencies-that-you-need

NOTICE: Linux builds have restrictions. Please be sure to check [NOTICE.md](../../NOTICE.md) in the top directory when using it.

### proj.mac

CMake project for macOS.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `make_xcode` | Executable application |
| `proj_xcode` | Xcode project |

WARNING: Regarding macOS build, it is not possible to build normally due to a problem caused by `Cocos2d-x V4.0`. Please check the following Issue for the workaround.

* [cocos2d/cocos2d-x error: Objective-C was disabled in PCH file but is currently enabled#20607](https://github.com/cocos2d/cocos2d-x/issues/20607#issuecomment-780266298)

### proj.win

CMake project for Windows.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `proj_msvcXXXX.bat` | Visual Studio project |

NOTICE: Builds using nmake are not supported due to Cocos2d-x errors.

### Resources

Contains the resources referenced in the Cocos2d-x template project.

### CMakeLists.txt

Configuration file for CMake.

The build settings for each platform are listed.


## thirdParty

Contains third-party libraries and build scripts used in the sample project.

### Cocos2d-x setup

Download Cocos2d-x by running the script in the `script` directory.

| Platform | Script name |
| --- | --- |
| Linux / macOS | `setup_cocos2d` |
| Windows | `setup_cocos2d.bat` |

You can specify the version of Cocos2d-x on the SCM to be used with a commit hash by changing `COCOS_COMMIT_HASH` in the script.

After downloading, it will be expanded with the directory name `thirdParty/cocos2d`.

[Cocos2d-x]: https://cocos2d-x.org/
