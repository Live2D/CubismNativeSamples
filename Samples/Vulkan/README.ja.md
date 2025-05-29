# Cubism Native Samples for Vulkan

Vulkan で実装したアプリケーションのサンプル実装です。


## 開発環境

| サードパーティ | バージョン |
| --- | --- |
| [GLFW] | 3.4 |
| [stb_image.h] | 2.30 |
| [Vulkan SDK] | 1.4.313.0 |

その他の開発環境・動作確認環境はトップディレクトリにある [README.ja.md](/README.ja.md) を参照してください。


## ディレクトリ構造

```
.
├─ Demo
│  ├─ proj.linux.cmake      # CMake project for Linux
│  ├─ proj.win.cmake        # CMake project for Windows
└─ thirdParty               # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り使用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.linux.cmake

Linux 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `make_gcc` | 実行可能なアプリケーション |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

追加ライブラリとして [GLFW] を使用しています。
[thirdParty](#thirdParty) の項目を参照して事前にダウンロードを行なってください。
ビルド時、これ以外に`libpng`やX Windows System依存ライブラリ等必要なパッケージのインストールを求められる場合がございますので、その際は求められたパッケージをインストールしてください。


### proj.win.cmake

Windows 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `nmake_msvcXXXX.bat` | 実行可能なアプリケーション |
| `proj_msvcXXXX.bat` | Visual Studio プロジェクト |

追加ライブラリとして [GLFW] を使用しています。
[thirdParty](README.ja.md#thirdParty) の項目を参照して事前にダウンロードを行なってください。

## thirdParty

サンプルプロジェクトで使用するサードパーティライブラリと自動展開スクリプトが含まれます。

### GLFW のセットアップ

`script` ディレクトリ内のスクリプトを実行することで GLFW のダウンロードを行います。

| プラットフォーム | スクリプト名 |
| --- | --- |
| Linux *1 | `setup_glfw` |
| Windows | `setup_glfw.bat` |

スクリプト内の `GLFW_VERSION` を変更することで、ダウンロードするバージョンを変更できます。

*1 Linuxでは GLFW が依存するライブラリのインストールが必要になる場合がございますので、公式ページを参考に依存関係にあるライブラリをすべてインストールしてください。
[GLFW.org Dependencies for X11 on Unix-like systems](https://www.glfw.org/docs/latest/compile_guide.html#compile_deps_x11)
詳しくは [NOTICE.md](/NOTICE.ja.md) をご確認ください。

### Vulkan SDK のインストール

あらかじめVulkan SDKインストールしておき、環境変数VULKAN_SDKを設定しておく必要があります。  
x86プラットフォームでビルドする場合、Vulkan SDKインストールの際に `SDK 32-bit Core Components` をインストールする必要があります。  
[Windowsのインストールの詳細](https://vulkan-tutorial.com/Development_environment#page_Windows)

Linuxでは以下のVulkanパッケージをインストールする必要があります。   
[Linuxのインストールの詳細](https://vulkan-tutorial.com/Development_environment#page_Linux)
| ディストリビューション | パッケージ |
| --- | --- |
| `Ubuntu` | `vulkan-tools` `libvulkan-dev` `vulkan-validationlayers-dev` `spirv-tools` |
| `AlmaLinux` | `vulkan-tools` `vulkan-loader-devel` `vulkan-validation-layers` |

[GLFW]: https://github.com/glfw/glfw
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h
[Vulkan SDK]: https://www.lunarg.com/vulkan-sdk/
