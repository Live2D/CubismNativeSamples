[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for OpenGL

OpenGL で実装したアプリケーションのサンプル実装です。


## 開発環境

| サードパーティ | バージョン |
| --- | --- |
| [GLEW] | 2.2.0 |
| [GLFW] | 3.4 |
| [ios-cmake] | 4.4.1 |
| [stb_image.h] | 2.29 |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.ja.md) を参照してください。


## ディレクトリ構造

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

[Cubism Native Framework] の各機能を一通り使用したサンプルです。

全ての機能を利用するフルバージョンのサンプルと、最小限の機能のみが備わったミニマムバージョンのサンプルの二通りがあります。
選択方法については各プラットフォームの項目を参照してください。

その他のプラットフォーム向けのサンプルはフルバージョンのみが利用できます。

フルバージョンではモーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
また、メニューボタンからモデルを切り替えることができます。

ミニマムバージョンでは単一のモデルの読み込み・表示、単一のモーションの再生、表情の設定、物理演算の設定などを行います。
ポーズの切り替えやモデルの切り替えには対応していません。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.android.cmake

このディレクトリにはフルバージョンとミニマムバージョンの二通りの Android Studio プロジェクトが含まれます。

NOTE: 事前に下記の SDK のダウンロードが必要です

* Android SDK Build-Tools
* NDK
* CMake

### proj.ios.cmake

iOS 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `proj_xcode` | Xcode プロジェクト |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

Appleシリコン搭載のMacでのiOS Simulator用ビルド時は下記の手順を行ってください。

1. Xcode の `Project設定 - TARGETS - Demo` および `Framework` の `Build Settings - Architectures - Architectures` の記載を実行環境に合わせて変更してください。


CMake のツールチェーンとして [ios-cmake] を使用しています。
[thirdParty](#thirdParty) の項目を参照して事前にダウンロードを行なってください。

[ios-cmake]: https://github.com/leetal/ios-cmake

### proj.linux.cmake

Linux 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `make_gcc` | 実行可能なアプリケーション |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

追加ライブラリとして [GLEW] と [GLFW] を使用しています。
[thirdParty](#thirdParty) の項目を参照して事前にダウンロードを行なってください。
ビルド時、これ以外に`libpng`やX Windows System依存ライブラリ等必要なパッケージのインストールを求められる場合がございますので、その際は求められたパッケージをインストールしてください。


### proj.mac.cmake

macOS 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `make_xcode` | 実行可能なアプリケーション |
| `proj_xcode` | Xcode プロジェクト |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

追加ライブラリとして [GLEW] と [GLFW] を使用しています。
[thirdParty](#thirdParty) の項目を参照して事前にダウンロードを行なってください。

### proj.win.cmake

Windows 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `nmake_msvcXXXX.bat` | 実行可能なアプリケーション |
| `proj_msvcXXXX.bat` | Visual Studio プロジェクト |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

追加ライブラリとして [GLEW] と [GLFW] を使用しています。
[thirdParty](#thirdParty) の項目を参照して事前にダウンロードを行なってください。

## thirdParty

サンプルプロジェクトで使用するサードパーティライブラリと自動展開スクリプトが含まれます。

### GLEW / GLFW のセットアップ

`script` ディレクトリ内のスクリプトを実行することで GLEW と GLFW のダウンロードを行います。

| プラットフォーム | スクリプト名 |
| --- | --- |
| Linux *1 / macOS | `setup_glew_glfw` |
| Windows *2 | `setup_glew_glfw.bat` |

スクリプト内の `GLEW_VERSION` 及び `GLFW_VERSION` を変更することで、ダウンロードするバージョンを変更できます。

*1 Linuxでは GLFW が依存するライブラリのインストールが必要になる場合がございますので、公式ページを参考に依存関係にあるライブラリをすべてインストールしてください。
[GLFW.org Dependencies for X11 on Unix-like systems](https://www.glfw.org/docs/latest/compile_guide.html#compile_deps_x11)
*2 `Visual Studio 2013` をご利用の際、追加対応が必要となる場合がございます。
詳しくは [NOTICE.md](/NOTICE.ja.md) をご確認ください。

## ios-cmake のセットアップ

`script` ディレクトリ内の `setup_ios_cmake` を実行することで ios-cmake のダウンロードを行います。

スクリプト内の `IOS_CMAKE_VERSION` を変更することで、ダウンロードするバージョンを変更できます。

[GLEW]: https://github.com/nigels-com/glew
[GLFW]: https://github.com/glfw/glfw
[ios-cmake]: https://github.com/leetal/ios-cmake
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h
