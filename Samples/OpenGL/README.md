# Cubism Native Samples for OpenGL

OpenGLで実装したアプリケーションのサンプル実装です。


## 開発環境

| ライブラリ | バージョン |
| --- | --- |
| GLEW | 2.1.0 |
| GLFW | 3.3 |

その他の開発環境・動作確認環境はトップディレクトリにある[README.md](/README.md)を参照してください。


## ディレクトリ構造

```
OpenGL
├─ Demo
│  ├─ proj.android      # Android Studio project
│  ├─ proj.ios          # Xcode project for iOS
│  ├─ proj.mac.cmake    # CMake project for macOS 
│  └─ proj.win.cmake    # CMake project for Windows
└─ thirdParty           # Third party libraries and scripts
```


## Demo

[Cubism Native Framework]の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.android

Android用のAndroid Studioプロジェクトが含まれます。

### proj.ios

iOS用のXcodeプロジェクトが含まれます。

### proj.mac.cmake

macOS用のソースファイルとCMakeファイル、及びシェルスクリプトが含まれます。

- `build_xcode.sh`を実行するとXcodeプロジェクトが作成されます。

GLEWとGLFWを利用しているため、事前にthirdPartyディレクトリに展開しておく必要があります。
詳細は下記の[thirdParty](README.md#thirdParty)の項目を参照してください。

### proj.win.cmake

Windows用のソースファイルとCMakeファイル、及びバッチファイルが含まれます。

- `nmake_xxx.bat`を実行すると実行可能なアプリケーションが作成されます。
- `proj_xxx.bat`を実行するとVisual Studioプロジェクトが作成されます。

GLEWとGLFWを利用しているため、事前にthirdPartyディレクトリに展開しておく必要があります。
詳細は下記の[thirdParty](README.md#thirdParty)の項目を参照してください。

## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリと自動展開スクリプトが含まれます。

### `setup_mac.sh`

macOS用のプロジェクトで利用するGLEWとGLFWのソースのダウンロードとバイナリのビルドを行います。

シェルスクリプト内の`GLEW_VERSION`と`GLFW_VERSION`を変更することで、特定のバージョンを指定することが出来ます。

### `setup_win.bat`

Windows用のプロジェクトで利用するGLEWとGLFWのバイナリのダウンロードを行います。

バッチスクリプト内の`GLEW_VERSION`と`GLFW_VERSION`を変更することで、特定のバージョンを指定することが出来ます。

スクリプト内ではzipの解凍にPowerShellコマンドを使用しています。
