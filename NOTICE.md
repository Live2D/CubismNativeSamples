[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# Notices

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


## [Limitation] Support for Apple products (updated 2021-12-09)

macOS Monterey v12.0 is now supported by the Cubism 4 SDK for Native R4 (4-r.4).
In addition, Cubism 4 SDK for Native R4 Beta1 (4-r.4-beta.1) or later versions are compatible with the Apple Silicon version of Mac.
† Please note that Cubism Editor is not currently compatible with Apple Silicon Macs.


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
