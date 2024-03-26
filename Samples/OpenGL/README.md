[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for OpenGL

This is a sample implementation of an application implemented with OpenGL.


## Development environment

| Third Party | Version |
| --- | --- |
| [GLEW] | 2.2.0 |
| [GLFW] | 3.4 |
| [ios-cmake] | 4.4.1 |
| [stb_image.h] | 2.29 |

For other development environments and operation environments, see [README.md](/README.md) in the top directory.


## Directory structure

```
.
├─ Demo
│  ├─ proj.android.cmake
│  │  ├─Full                # Full Demo Android Studio project
│  │  └─Minimum             # Minimum Demo Android Studio project
│  ├─ proj.ios.cmake        # CMake project for iOS
│  ├─ proj.linux.cmake      # CMake project for Linux
│  ├─ proj.mac.cmake        # CMake project for macOS
│  └─ proj.win.cmake        # CMake project for Windows
└─ thirdParty               # Third party libraries and scripts
```


## Demo

A sample that uses each function of [Cubism Native Framework].

There are two types of samples: the full version that uses all the features, and the minimum version that has only minimal features.
Please refer to the sections of each platform for the selection method.

Samples for other platforms are only available in the full version.

In the full version, you can play motions, set facial expressions, switch poses, set physics, etc.
You can also switch models from the menu button.

In the minimum version, you can load and display a single model, play a single motion, set facial expressions, set physics, etc.
Switching poses and models are not supported.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### proj.android.cmake

This directory contains two Android Studio projects: the full version and the minimum version.

NOTE: You need to download the following SDKs in advance.

* Android SDK Build-Tools
* NDK
* CMake

### proj.ios.cmake

CMake project for iOS.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `proj_xcode` | Xcode project |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

Follow the steps below when building for iOS Simulator on a Mac with Apple silicon.

1. Change the description of Xcode's `Project Settings - TARGETS - Demo` and `Framework`'s `Build Settings - Architectures - Architectures` according to the execution environment.


[ios-cmake] is used as a toolchain for CMake.
Please refer to the [thirdParty](#thirdParty) section and download in advance.

[ios-cmake]: https://github.com/leetal/ios-cmake

### proj.linux.cmake

CMake project for Linux.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `make_gcc` | Executable application |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

[GLEW] and [GLFW] are used as additional libraries.
Please refer to the [thirdParty](#thirdParty) section and download in advance.
During the build process, you may be prompted to install additional packages such as `libpng` or X Windows System dependent libraries. If prompted, please install the required packages.


### proj.mac.cmake

CMake project for macOS.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `make_xcode` | Executable application |
| `proj_xcode` | Xcode project |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

[GLEW] and [GLFW] are used as additional libraries.
Please refer to the [thirdParty](#thirdParty) section and download in advance.

### proj.win.cmake

CMake project for Windows.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `nmake_msvcXXXX.bat` | Executable application |
| `proj_msvcXXXX.bat` | Visual Studio project |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

[GLEW] and [GLFW] are used as additional libraries.
Please refer to the [thirdParty](#thirdParty) section and download in advance.

## thirdParty

Contains third-party libraries and auto-deployment scripts used in the sample project.

### GLEW / GLFW setup

Download GLEW and GLFW by running the script in the `script` directory.

| Platform | Script name |
| --- | --- |
| Linux *1 / macOS | `setup_glew_glfw` |
| Windows *2 | `setup_glew_glfw.bat` |

You can change the version to download by changing `GLEW_VERSION` and `GLFW_VERSION` in the script.

*1 On Linux, it may be necessary to install libraries on which GLFW depends. Please refer to the official page to install all libraries on which GLFW depends.
[GLFW.org Dependencies for X11 on Unix-like systems](https://www.glfw.org/docs/latest/compile_guide.html#compile_deps_x11)
*2 Additional support may be required when using `Visual Studio 2013`.
See [NOTICE.md](/NOTICE.md) for details.

## ios-cmake setup

Download ios-cmake by running `setup_ios_cmake` in the `script` directory.

You can change the version to download by changing `IOS_CMAKE_VERSION` in the script.

[GLEW]: https://github.com/nigels-com/glew
[GLFW]: https://github.com/glfw/glfw
[ios-cmake]: https://github.com/leetal/ios-cmake
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h
