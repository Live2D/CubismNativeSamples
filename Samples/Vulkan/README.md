# Cubism Native Samples for Vulkan

This is a sample implementation of an application implemented with Vulkan.


## Development environment

| Third Party | Version |
| --- | --- |
| [GLFW] | 3.4 |
| [stb_image.h] | 2.30 |
| [Vulkan SDK] | 1.4.313.0 |

For other development environment and operation environment, refer to [README.md](/README.md) in the top directory.


## Directory structure

```
.
├─ Demo
│  ├─ proj.linux.cmake      # CMake project for Linux
│  └─ proj.win.cmake        # CMake project for Windows
└─ thirdParty               # Third party libraries and scripts
```


## Demo

This is a sample that uses each function of [Cubism Native Framework].
You can play motion, set facial expressions, switch poses, set physics, etc.
You can switch models from the menu button.

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

The items contained in this directory are as follows.

### proj.linux.cmake

CMake project for Linux.

Running the script in the `script` directory will generate a CMake deliverable in the `build` directory.

| Script name | Product |
| --- | --- |
| `make_gcc` | Executable application |

When you run these scripts, you can choose to generate the full version or the minimum version of the sample.

[GLFW] are used as additional libraries.
Please refer to the [thirdParty](#thirdParty) section and download in advance.
During the build process, you may be prompted to install additional packages such as `libpng` or X Windows System dependent libraries. If prompted, please install the required packages.

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
| Linux | `setup_glfw` |
| Windows | `set_glfw.bat` |

You can change the version to download by changing `GLFW_VERSION` in the script.

*1 On Linux, it may be necessary to install libraries on which GLFW depends. Please refer to the official page to install all libraries on which GLFW depends.
[GLFW.org Dependencies for X11 on Unix-like systems](https://www.glfw.org/docs/latest/compile_guide.html#compile_deps_x11)
See [NOTICE.md](/NOTICE.md) for details.

### Installing the Vulkan SDK

You need to install the Vulkan SDK in advance and set the environment variable VULKAN_SDK.  
If you are building on an x86 platform, you must install the `SDK 32-bit Core Components` when installing the Vulkan SDK.  
[Installation details on windows](https://vulkan-tutorial.com/Development_environment#page_Windows)

On Linux, the following packages must be installed  
[Installation details on linux](https://vulkan-tutorial.com/Development_environment#page_Linux)
| Distribution | Package |
| --- | --- |
| `Ubuntu` | `vulkan-tools` `libvulkan-dev` `vulkan-validationlayers-dev` `spirv-tools` |
| `AlmaLinux` | `vulkan-tools` `vulkan-loader-devel` `vulkan-validation-layers` |

[GLFW]: https://github.com/glfw/glfw
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h
[Vulkan SDK]: https://www.lunarg.com/vulkan-sdk/
