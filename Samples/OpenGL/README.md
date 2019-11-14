# Cubism Native Samples for OpenGL

OpenGL で実装したアプリケーションのサンプル実装です。


## 開発環境

| ライブラリ | バージョン |
| --- | --- |
| GLEW | 2.1.0 |
| GLFW | 3.3 |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.md) を参照してください。


## ディレクトリ構造

```
OpenGL
├─ Demo
│  ├─ proj.android      # Android Studio project
│  ├─ proj.ios          # Xcode project for iOS
│  ├─ proj.linux.cmake  # CMake project for Linux
│  ├─ proj.mac.cmake    # CMake project for macOS
│  └─ proj.win.cmake    # CMake project for Windows
└─ thirdParty           # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.android

Android 用の Android Studio プロジェクトが含まれます。

### proj.ios

iOS 用の Xcode プロジェクトが含まれます。

### proj.linux.cmake

Linux 用のソースファイルと CMake ファイル、及びシェルスクリプトが含まれます。

- `make_gcc.sh` を実行すると実行可能なアプリケーションが作成されます。

GLEW と GLFW を利用しているため、事前に thirdParty ディレクトリに展開しておく必要があります。
詳細は下記の [thirdParty](README.md#thirdParty) の項目を参照してください。

### proj.mac.cmake

macOS 用のソースファイルと CMake ファイル、及びシェルスクリプトが含まれます。

- `make_xcode.sh` を実行すると実行可能なアプリケーションが作成されます。
- `proj_xcode.sh` を実行すると Xcode プロジェクトが作成されます。

GLEW と GLFW を利用しているため、事前にthirdPartyディレクトリに展開しておく必要があります。
詳細は下記の [thirdParty](README.md#thirdParty) の項目を参照してください。

### proj.win.cmake

Windows 用のソースファイルとCMakeファイル、及びバッチファイルが含まれます。

- `nmake_msvcXXXX.bat` を実行すると実行可能なアプリケーションが作成されます。
- `proj_msvcXXXX.bat` を実行すると Visual Studio プロジェクトが作成されます。

GLEW と GLFW を利用しているため、事前に thirdParty ディレクトリに展開しておく必要があります。
詳細は下記の [thirdParty](README.md#thirdParty) の項目を参照してください。

Visual Studio 2019 でのビルドは、GLEW が対応していないためサポートしておりません。


## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリと自動展開スクリプトが含まれます。

`setup.bat` (Windows) または `setup.sh` (macOS / Linux) を実行することで、
プロジェクトで利用する GLEW と GLFW のソースファイルのダウンロードを行います。

スクリプト内の `GLEW_VERSION` と `GLFW_VERSION` を変更することでライブラリのバージョンを指定することが出来ます。
