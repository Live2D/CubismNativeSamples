[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# Notices

## [Caution] Support for Apple's Privacy Manifest Policy

This product does not use the APIs or third-party products specified in Apple's privacy manifest policy.
This will be addressed in future updates if this product requires such support.
Please check the documentation published by Apple for details.

* [Privacy updates for App Store submissions](https://developer.apple.com/news/?id=3d8a9yyh)


## [Caution] Support for Wayland in Linux OpenGL Environment (2024-03-26)

`GLFW 3.4`, which this product uses in the Linux OpenGL environment, can now be compiled for Wayland.
However, this sample is not compatible with Wayland because `glewInit` is currently not working properly when compiling for Wayland (EGL) in `GLEW 2.2.0`.

If you are using the Linux OpenGL environment, please continue to use the X11 build as usual.

Please refer to the issue below for details.

* [GLEW issue](https://github.com/nigels-com/glew/issues/172)


## [Caution] Mac Catalyst build error for Metal (2024-01-25)

There is an error when building Mac Catalyst for Metal.
Specifically, when generating a project using `cmake3.27.0` or higher, errors such as `'Foundation/NSURLHandle.h' file not found` will occur.
We will continue to investigate the details.

### Workaround

* Use cmake with versions less than 3.27


## [Caution] About Cocos2d-x setup error (2022-03-15)

An error occurs in the setup-cocos2d batch that is executed when setting up the Cocos2d-x sample project.
This is because GitHub has improved the security of the Git protocol, and Git fails to get the Cocos2d-x submodule.
Specifically, an error has occurred because the connection via `git://` is no longer possible.

Please check the official GitHub blog for details.

* [GitHub Official Blog](https://github.blog/2021-09-01-improving-git-protocol-security-github/)

### Workaround

* Replace the connection using `git://` with `https://` by executing the following command in the terminal or command prompt.
  * `git config --global url."https://github.com/".insteadOf git://github.com/`
    * Please note that this command affects gitconfig for all users of the execution terminal.


## [Caution] Memory leak in macOS sample builds of Metal and Cocos2d-x v4.0 (2021-12-09)

A memory leak occurs in the macOS sample builds of Metal and Cocos2d-x v4.0.
This happens within the Metal framework, so please wait for official support.
Note that we cannot answer inquiries regarding this matter.

Please check the Apple Developer Forums for more information.

* [Apple Developer Forums - Memory leak in MTLCommandBuffer?](https://developer.apple.com/forums/thread/120931)


## [Limitation] About the Linux (Ubuntu) sample builds of Cocos2d-x v4.0 (2021-04-15)

Cocos2d-x v4.0 does not support `Ubuntu 20.04` builds.
Please check and apply one of the following workarounds when using it.

### Workaround

* Use `Ubuntu 16.04` or `Ubuntu 18.04`

* Check the following issue and replace the `libchipmunk` library used in `Cocos2d-x v4.0`
  * [cocos2d/cocos2d-x linking error when integrating with libchipmunk on linux#20471](https://github.com/cocos2d/cocos2d-x/issues/20471)
  * WARNING: Projects built using this workaround cannot be maintained under any circumstances.


## [Caution] About OpenGL sample builds when using Visual Studio 2013 (2021-02-17)

In the `OpenGL` sample builds using `Visual Studio 2013`, a link error will occur in `GLEW` if you build using `setup_glew_glfw.bat` as is.
Please check and use the workaround below.

### Workaround

* When using `GLEW 2.2.0`
  * Remove the reference to `libvcruntime.lib` from the `glew` project
  * Use Visual Studio later than `Visual Studio 2013`

* When using `GLEW 2.1.0`
  * Use `setup_glew_glfw_vs2013.bat` to set up `thirdParty`


## Cubism Core

### [Caution] About using `.bundle` and `.dylib` on macOS Catalina or later

To use `.bundle` and `.dylib` on macOS Catalina, you need to be connected to an online environment to verify your notarization.

Please check the official Apple documentation for details.

* [Apple Official Documentation](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
