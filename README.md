[English](README.md) / [日本語](README.ja.md)

---

![image](logor.png)

# Cubism Native Samples

This is a sample implementation of an application that displays models output by Live2D Cubism 4 Editor.

It is used in conjunction with Cubism Native Framework and Live2D Cubism Core.


## License

Please check the [license](LICENSE.md) before using this SDK.


## Notices

Please check the [notices](NOTICE.md) before using this SDK.


## Directory structure

```
.
├─ Core             # Directory containing Live2D Cubism Core
├─ Framework        # Directory containing source code for rendering and animation functions
└─ Samples
   ├─ Cocos2d-x     # Directory containing the Cocos2d-x sample project
   ├─ D3D9          # Directory containing the DirectX 9.0c sample project
   ├─ D3D11         # Directory containing the DirectX 11 sample project
   ├─ Metal         # Directory containing the Metal sample project
   ├─ OpenGL        # Directory containing the OpenGL sample project
   └─ Resources     # Directory containing resources such as model files and images
```


## Cubism Native Framework

Provides various functions for displaying and manipulating the model.

[Cubism Native Framework] is included as a submodule of this repository.
After cloning this repository, the files will be added by cloning the submodule.

[Cubism Native Framework]: (https://github.com/Live2D/CubismNativeFramework)


## Live2D Cubism Core for Native

A library for loading the model.
Live2D Cubism Core for Native is not included in this repository.

To download, please refer to [this](https://www.live2d.com/download/cubism-sdk/download-native/) page.
Copy the contents of the downloaded ZIP file to the `Core` directory of this repository and link it to the program.


## How to build

The build method depends on the sample project.
Please refer to the `README.md` included with each sample project.

### Where to generate the sample project deliverables

In this sample, the deliverables of CMake projects except Android are generated in the `bin` directory.

E.g. When building a macOS project of the OpenGL sample using the `make_gcc` script
```
Demo
└─ proj.mac.cmake
   └─ build
      └─ make_gcc
         └─ bin
            └─ Demo
               ├─ Resources    # Same as Samples/Resources
               └─ Demo         # Executable applications
```


## Compile options

If the macro `USE_RENDER_TARGET` or `USE_MODEL_RENDER_TARGET` is enabled in your project, the model will be rendered to the texture.
Please refer to 'LAppLive2DManager.cpp' in the sample directory for details.


## SDK manual

[Cubism SDK Manual](https://docs.live2d.com/cubism-sdk-manual/top/)


## Changelog

Please refer to [CHANGELOG.md](CHANGELOG.md) for the changelog of this repository.


## Development environment

| Development Tool | Version |
| --- | --- |
| Android Studio | Electric Eel 2022.1.1 Patch 1 |
| CMake | 3.25.2 |
| Visual Studio 2013 | Update 5 |
| Visual Studio 2015 | Update 3 |
| Visual Studio 2017 | 15.9.51 |
| Visual Studio 2019 | 16.11.23 |
| Visual Studio 2022 | 17.4.4 |
| XCode | 14.2 |

### Android

| Android SDK tools | Version |
| --- | --- |
| Android NDK | 25.2.9519653 |
| Android SDK | 33.0.2 |
| CMake | 3.22.1 |

### Linux

| System | Distribution version | Docker image | GCC |
| --- | --- | --- | --- |
| Red Hat | Amazon Linux 2 |  `amazonlinux:2` | 7.3.1 |
| Red Hat | CentOS 7 | `centos:7` | 4.8.5 |
| Red Hat | CentOS Stream 8 *1 | `centos:latest` | 8.5.0 |
| Red Hat | CentOS Stream 9 | - | 11.3.1 |
| Debian | Ubuntu 18.04 | `ubuntu:18.04` | 7.5.0 |
| Debian | Ubuntu 22.04.1 | `ubuntu:22.04.1` | 11.3.0 |

*1 CentOS 8 is not supported

#### Mesa library

* Red Hat
  * `mesa-libGL-devel`
  * `mesa-libGLU-devel`
* Debian
  * `libgl1-mesa-dev`
  * `libglu1-mesa-dev`


## Operation environment

| Platform | Version |
| --- | --- |
| iOS / iPadOS | 16.3 |
| macOS | 13.2 |
| Windows 10 | 21H2 |
| Windows 11 | 21H2 |

### Android

| Version | Device | Tegra |
| --- | --- | --- |
| 13 | Pixel 6a | |
| 7.1.1 | Nexus 9 | ✔︎ |

### Linux

| System | Distribution | Version |
| --- | --- | --- |
| Red Hat | Amazon Linux | 2 |
| Red Hat | CentOS Stream | 8 |
| Red Hat | CentOS Stream | 9 |
| Debian | Ubuntu | 22.04.1 |

## Contributing

There are many ways to contribute to the project: logging bugs, submitting pull requests on this GitHub, and reporting issues and making suggestions in Live2D Community.

### Forking And Pull Requests

We very much appreciate your pull requests, whether they bring fixes, improvements, or even new features. Note, however, that the wrapper is designed to be as lightweight and shallow as possible and should therefore only be subject to bug fixes and memory/performance improvements. To keep the main repository as clean as possible, create a personal fork and feature branches there as needed.

### Bugs

We are regularly checking issue-reports and feature requests at Live2D Community. Before filing a bug report, please do a search in Live2D Community to see if the issue-report or feature request has already been posted. If you find your issue already exists, make relevant comments and add your reaction.

### Suggestions

We're also interested in your feedback for the future of the SDK. You can submit a suggestion or feature request at Live2D Community. To make this process more effective, we're asking that you include more information to help define them more clearly.

## Community

If you want to suggest or ask questions about how to use the Cubism SDK between users, please use the community.

- [Live2D community](https://community.live2d.com/)
- [Live2D 公式コミュニティ (Japanese)](https://creatorsforum.live2d.com/)


![image](logos.png)
