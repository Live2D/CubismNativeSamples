[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for Metal

This is a sample implementation of an application implemented with Metal.
There are restrictions. Please check [NOTICE.md](/NOTICE.md) in the top directory.


## Development environment

| Third Party | Version |
| --- | --- |
| [ios-cmake]    | 4.5.0     |
| [stb_image.h]  | 2.30      |

For other development environments and operation environments, see [README.md](/README.md) in the top directory.


## Directory structure

```
.
├─ Demo
│  └─ proj.ios.cmake        # CMake project for iOS or Mac Catalyst
└─ thirdParty               # Third party libraries and scripts
```


## Demo

A sample that uses each function of [Cubism Native Framework].
You can play motions, set facial expressions, switch poses, set physics, etc.
You can switch models from the menu button.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### proj.ios.cmake

CMake project for iOS or Mac Catalyst.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

You can choose between iOS, iOS Simulator, or Mac Catalyst when running the script.

| Script name | Product |
| --- | --- |
| `proj_xcode` | Xcode project |

Follow the steps below when building for iOS Simulator on a Mac with Apple silicon.

1. Change the description of Xcode's `Project Settings - TARGETS - Demo` and `Framework`'s `Build Settings - Architectures - Architectures` according to the execution environment.


Follow the steps below when building for Mac Catalyst.

1. In Xcode, under `Project Settings - TARGETS - Demo - General - Supported Destinations`, press `+`, select `Mac > Mac Catalyst` and press `Enable` in the modal view.
2. Change the description of Xcode's `Project Settings - TARGETS - Demo` and `Framework`'s `Build Settings - Architectures - Architectures` according to the execution environment.
3. Change Xcode's `Project Settings - TARGETS - Demo` and `Framework`'s `Build Settings - Architectures - Base SDK` to `iOS`, or refer to the Apple official tutorials.
[turning-on-mac-catalyst]: https://developer.apple.com/tutorials/mac-catalyst/turning-on-mac-catalyst



[ios-cmake] is used as a toolchain for CMake.
Please refer to the [thirdParty](README.md#thirdParty) section and download in advance.

[ios-cmake]: https://github.com/leetal/ios-cmake



## thirdParty

Contains third-party libraries and auto-deployment scripts used in the sample project.

## ios-cmake setup

Download ios-cmake by running `setup_ios_cmake` in the `script` directory.

You can change the version to download by changing `IOS_CMAKE_VERSION` in the script.

[ios-cmake]: https://github.com/leetal/ios-cmake
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h
