[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for DirectX 11

This is a sample implementation of an application implemented with DirectX 11.


## Development environment

| Development Kit | Visual Studio | Version |
| --- | --- | --- |
| [DirectXTK] | 2015 | December 17, 2019 |
| [DirectXTK] | 2017 | March 24, 2022 |
| [DirectXTK] | 2019 / 2022 | December 31, 2023 |

For other development environments and operation environments, see [README.md](/README.md) in the top directory.


## Directory structure

```
.
├─ Demo
│  └─ proj.d3d11.cmake  # CMake project
└─ thirdParty           # Third party libraries and scripts
```


## Demo

A sample that uses each function of [Cubism Native Framework].

There are two types of samples: the full version that uses all the features, and the minimum version that has only minimal features.

In the full version, you can play motions, set facial expressions, switch poses, set physics, etc.
You can also switch models from the menu button.

In the minimum version, you can load and display a single model, play a single motion, set facial expressions, set physics, etc.
Switching poses and models are not supported.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### proj.d3d11.cmake

CMake project.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `nmake_msvcXXXX.bat` | Executable application |
| `proj_msvcXXXX.bat` | Visual Studio project |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

[DirectXTK] is used as a development kit.
Please refer to the [thirdParty](README.md#thirdParty) section and download in advance.


## thirdParty

Contains third-party libraries and build scripts used in the sample project.

### DirectXTK setup

Download DirectXTK and build the library by running the script in the `script` directory.

| Visual Studio | Script Name |
| --- | --- |
| 2015 | `setup_msvc2015.bat` |
| 2017 | `setup_msvc2017.bat` |
| 2019 | `setup_msvc2019.bat` |
| 2022 | `setup_msvc2022.bat` |

You can specify the library version by changing `XTK_VERSION` in the script.

[DirectXTK]: https://github.com/Microsoft/DirectXTK

.NET Framework 4.7.2 must be installed to do an x64 release build.
[DirectXTK Release Notes]: https://github.com/microsoft/DirectXTK#release-notes
