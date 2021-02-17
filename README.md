# Cubism Native Samples

Live2D Cubism 4 Editor で出力したモデルを表示するアプリケーションのサンプル実装です。

Cubism Native Framework および Live2D Cubism Core と組み合わせて使用します。


## ライセンス

本 SDK を使用する前に[ライセンス](LICENSE.md)をご確認ください。


## 注意事項

本 SDK を使用する前に [注意事項](NOTICE.md)をご確認ください。


## ディレクトリ構成

```
.
├─ Core             # Live2D Cubism Core が含まれるディレクトリ
├─ Framework        # レンダリングやアニメーション機能などのソースコードが含まれるディレクトリ
└─ Samples
   ├─ Cocos2d-x     # Cocos2d-x のサンプルプロジェクトが含まれるディレクトリ
   ├─ D3D9          # DirectX 9.0c のサンプルプロジェクトが含まれるディレクトリ
   ├─ D3D11         # DirectX 11 のサンプルプロジェクトが含まれるディレクトリ
   ├─ OpenGL        # OpenGL のサンプルプロジェクトが含まれるディレクトリ
   └─ Resources     # モデルのファイルや画像などのリソースが含まれるディレクトリ
```


## Cubism Native Framework

モデルを表示、操作するための各種機能を提供します。

[Cubism Native Framework] は、当リポジトリのサブモジュールとして含まれています。
当リポジトリをクローンした後、サブモジュールのクローンを実行することでファイルが追加されます。

[Cubism Native Framework]: (https://github.com/Live2D/CubismNativeFramework)


## Live2D Cubism Core for Native

モデルをロードするためのライブラリです。
当リポジトリには Live2D Cubism Core for Native は同梱されていません。

ダウンロードするには[こちら](https://www.live2d.com/download/cubism-sdk/download-native/)のページを参照ください。
ダウンロードした Zip ファイルの中身を当リポジトリの `Core` ディレクトリにコピーし、プログラムにリンクさせてください。


## ビルド方法

ビルド方法についてはサンプルプロジェクトによって異なります。
各サンプルプロジェクトに同梱された `README.md` を参照ください。

### サンプルプロジェクトの成果物の生成場所

本サンプルでは、Android を除く CMake プロジェクトの成果物は `bin` ディレクトリに生成されます。

例）OpenGL サンプルの macOS プロジェクトのビルドを `make_gcc` スクリプトを使用して行なった場合
```
Demo
└─ proj.mac.cmake
   └─ build
      └─ make_gcc
         └─ bin
            └─ Demo
               ├─ Resources    # Samples/Resources と同じ
               └─ Demo         # 実行可能なアプリケーション
```


## コンパイルオプション

プロジェクトにおいて、マクロ `USE_RENDER_TARGET` または `USE_MODEL_RENDER_TARGET` が有効な場合、
モデルがテクスチャへレンダリングされるようになります。
詳細はサンプルディレクトリ内の `LAppLive2DManager.cpp` を参照ください。


## SDKマニュアル

[Cubism SDK Manual](https://docs.live2d.com/cubism-sdk-manual/top/)


## 変更履歴

当リポジトリの変更履歴については [CHANGELOG.md](CHANGELOG.md) を参照ください。


## 開発環境

| 開発ツール | バージョン |
| --- | --- |
| Android Studio | 4.1.1 |
| CMake | 3.19.3 |
| Visual Studio 2013 | Update 5 |
| Visual Studio 2015 | Update 3 |
| Visual Studio 2017 | 15.9.31 |
| Visual Studio 2019 | 16.8.4 |
| XCode | 12.3 |

### Android

| Android SDK tools | バージョン |
| --- | --- |
| Android NDK | 22.0.7026061 |
| Android SDK | 30.05 |
| CMake | 3.10.2.4988404 |

### Linux

| 系統 | Docker イメージ | GCC |
| --- | --- | --- |
| Red Hat | `amazonlinux:2` | 7.3.1 |
| Red Hat | `centos:7` | 4.8.5 |
| Red Hat | `centos:8` | 8.3.1 |
| Debian | `ubuntu:16.04` | 5.4.0 |
| Debian | `ubuntu:18.04` | 7.4.0 |
| Debian | `ubuntu:20.04` | 8.3.1 |

#### Mesa ライブラリ

* Red Hat
  * `mesa-libGL-devel`
  * `mesa-libGLU-devel`
* Debian
  * `libgl1-mesa-dev`
  * `libglu1-mesa-dev`


## 動作確認環境

| プラットフォーム | バージョン |
| --- | --- |
| iOS / iPadOS | 14.3 |
| macOS | 10.15.7 |
| Windows 10 | 2004 / 20H2 |

### Android

| バージョン | デバイス | Tegra |
| --- | --- | --- |
| 11 | Pixel 3a | |
| 7.1.1 | Nexus 9 | ✔︎ |

### Linux

| 系統 | ディストリビューション | バージョン |
| --- | --- | --- |
| Red Hat | Amazon Linux | 2 |
| Red Hat | CentOS | 8 |
| Debian | Ubuntu | 20.04 |
