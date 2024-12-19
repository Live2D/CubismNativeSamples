[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# Notices

## [Caution] Regarding Cubism SDK for Native Cocos2d-x Support (2024-12-19)

Support for Cocos2d-x in the Cubism SDK for Native has been discontinued starting with Cubism 5 SDK for Native R2. 
For any inquiries regarding this matter, please contact us through the [contact form]](<https://www.live2d.jp/contact/>).


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


## [Caution] Memory leak in macOS sample builds of Metal (2021-12-09)

A memory leak occurs in the macOS sample builds of Metal.
This happens within the Metal framework, so please wait for official support.
Note that we cannot answer inquiries regarding this matter.

Please check the Apple Developer Forums for more information.

* [Apple Developer Forums - Memory leak in MTLCommandBuffer?](https://developer.apple.com/forums/thread/120931)


## Cubism Core

### [Caution] About using `.bundle` and `.dylib` on macOS Catalina or later

To use `.bundle` and `.dylib` on macOS Catalina, you need to be connected to an online environment to verify your notarization.

Please check the official Apple documentation for details.

* [Apple Official Documentation](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
