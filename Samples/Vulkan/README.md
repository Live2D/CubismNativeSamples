# Cubism Native Samples for Vulkan

This is a sample implementation of an application implemented with Vulkan.


## Development environment

| Third Party | Version |
| --- | --- |
| [GLFW] | 3.4 |
| [stb_image.h] | 2.29 |
| [Vulkan SDK] | 1.3 |

For other development environment and operation environment, refer to [README.md](/README.md) in the top directory.


## Directory structure

```
.
├─ Demo
│  └─ proj.win.cmake        # CMake project for Windows
└─ thirdParty               # Third party libraries and scripts
```


## Demo

This is a sample that uses each function of [Cubism Native Framework].
You can play motion, set facial expressions, switch poses, set physics, etc.
You can switch models from the menu button.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### proj.win.cmake

CMake project for Windows.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `nmake_msvcXXXX.bat` | Executable application |
| `proj_msvcXXXX.bat` | Visual Studio project |

[GLFW] is used as an additional library.
Please refer to the [thirdParty](README.md#thirdParty) section and download in advance.

## thirdParty

Contains third-party libraries and auto-deployment scripts used in the sample project.

### GLFW Setup

Download GLFW by running the script in the 'script' directory.

| Platform | Script name |
| --- | --- |
| Windows | `setup_glew_glfw.bat` |

You can change the version to download by changing `GLFW_VERSION` in the script.

[GLFW]: https://github.com/glfw/glfw
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h

### Installing the Vulkan SDK

You need to install the Vulkan SDK in advance and set the environment variable VULKAN_SDK.
When building on an x86 platform, you must install the `SDK 32-bit Core Components` when installing the Vulkan SDK.

[Vulkan SDK]: https://www.lunarg.com/vulkan-sdk/
