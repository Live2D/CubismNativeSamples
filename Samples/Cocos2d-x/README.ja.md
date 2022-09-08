[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for Cocos2d-x

Cocos2d-xで実装したアプリケーションのサンプル実装です。


## 開発環境

| フレームワーク | バージョン |
| --- | --- |
| [XCode] | 12.4 |
| [Cocos2d-x] | 4.0 (`95e5d868ce5958c0dadfc485bdda52f1bc404fe0`) |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](../../README.ja.md) を参照してください。

また、[Cocos2d-x ドキュメントの前提条件]も確認してください。

[Cocos2d-x ドキュメントの前提条件]: https://docs.cocos2d-x.org/cocos2d-x/v3/en/installation/prerequisites.html

### Cocos2d-x v4.x に関して

Cocos2d-x v4.0 から Metal API の対応に伴い、Renderer の構成が変更され、OpenGLES の API を直接使用することが出来なくなりました。
詳しくは [Cocos2d-x のドキュメント](https://docs.cocos2d-x.org/cocos2d-x/v4/en/upgradeGuide/) を参照してください。

この変更に伴い Cocos2d-x v4.x 系では Cubism Framework の Renderer に Cocos2d-x　専用の物を用意いたしました。現在は、各プラットフォームで共通の Renderer を使用しており、 macOS, Windows, Linux, iOS, Android と横断的にクロスコンパイルを通すことが可能です。


## ディレクトリ構造

```
.
├─ Demo
│  ├─ Classes       # Sources for sample projects
│  ├─ proj.android  # Android Studio project
│  ├─ proj.ios      # Additional files and build scripts for iOS
│  ├─ proj.linux    # Additional files and build scripts for Linux
│  ├─ proj.mac      # Additional files and build scripts for macOS
│  ├─ proj.win      # Additional files and build scripts for Windows
│  └─ Resources     # Resources of cocos2d-x template project
└─ thirdParty       # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### Classes

サンプルプロジェクトで使用するソースコードが含まれます。

### proj.android

Android 用の Android Studio プロジェクトが含まれます。

NOTE: 事前に下記の SDK のダウンロードが必要です

* Android SDK Build-Tools
* NDK
* CMake

### proj.ios

iOS 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `proj_xcode` | Xcode プロジェクト |

ビルド時に下記の手順を行なってください。

1. XCode の `Project設定 - TARGETS - Demo - Packaging - Info.plist File`　に記載されている `Info.plist` 内の `Executable file` を `$(EXECUTABLE_NAME)` または`Demo`（アプリ名）に書き換えてください

NOTICE: Cubism Core は i386 アーキテクチャをサポートしていないため、**iPhone Simulator 向けのビルドは行えません。**

### proj.linux

Linux 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `make_gcc` | 実行可能なアプリケーション |

ビルド前に下記の手順を行なってください。

1. [Dependencies that you need] ドキュメントを参照して必要なパッケージをダウンロードします

[Dependencies that you need]: https://docs.cocos2d-x.org/cocos2d-x/v4/en/installation/Linux.html#dependencies-that-you-need

NOTICE: Linuxビルドには制限があります。使用する際は必ずトップディレクトリにある[NOTICE.md](../../NOTICE.md)をご確認ください。

### proj.mac

macOS 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `make_xcode` | 実行可能なアプリケーション |
| `proj_xcode` | Xcode プロジェクト |

WARNING: macOSビルドにつきまして、`Cocos2d-x V4.0` に起因する不具合のために正常にビルドができない状態となっております。対処方法につきましては、以下の Issue をご確認ください。

* [cocos2d/cocos2d-x error: Objective-C was disabled in PCH file but is currently enabled
#20607
](https://github.com/cocos2d/cocos2d-x/issues/20607#issuecomment-780266298)

### proj.win

Windows 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `proj_msvcXXXX.bat` | Visual Studio プロジェクト |

NOTICE: nmake を使用したビルドは Cocos2d-x のエラーが発生するため対応していません。

### Resources

Cocos2d-xのテンプレートプロジェクトで参照しているリソースが含まれます。

### CMakeLists.txt

CMake 用の設定ファイルです。

各プラットフォームのビルド設定が記載されています。


## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリとビルドスクリプトが含まれます。

### Cocos2d-x のセットアップ

`script` ディレクトリ内のスクリプトを実行することで Cocos2d-x のダウンロードを行います。

| プラットフォーム | スクリプト名 |
| --- | --- |
| Linux / macOS | `setup_cocos2d` |
| Windows | `setup_cocos2d.bat` |

スクリプト内の `COCOS_COMMIT_HASH` を変更することで、使用する Cocos2d-x の SCM 上のバージョンをコミットハッシュで指定することが出来ます。

ダウンロード後は `thirdParty/cocos2d` というディレクトリ名で展開されます。

[Cocos2d-x]: https://cocos2d-x.org/
