[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for DirectX 9.0c

This is a sample implementation of an application implemented with DirectX 9.0c.


## Development environment

| Development kit | Version |
| --- | --- |
| [DirectX SDK] | 9.0c (June2010) |

For other development environments and operation environments, see [README.md](/README.md) in the top directory.


## Directory structure

```
.
└─ Demo
   └─ proj.d3d9.cmake   # CMake project using DirectX 9.0c
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

### proj.d3d9.cmake

Contains source files, CMake files, and batch files.

* Running `scripts/nmake_xxx.bat` will generate an executable application in the `build` directory.
* Running `scripts/proj_xxx.bat` will generate a Visual Studio project in the `build` directory.
* When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

Since [DirectX SDK] is used, it needs to be installed in advance.

If you get a `S1023` error during installation, see [Microsoft Knowledge Base].

The environment variable `DXSDK_DIR` is set to the path where the SDK was installed during installation.
If an environment variable unset error occurs during build, set the above environment variable manually.

[DirectX SDK]: https://www.microsoft.com/en-us/download/details.aspx?id=6812
[Microsoft Knowledge Base]: https://blogs.msdn.microsoft.com/windows_multimedia_jp/2013/09/05/kbdirectx-sdk
