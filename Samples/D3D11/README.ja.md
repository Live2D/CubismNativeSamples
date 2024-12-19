[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for DirectX 11

DirectX 11 で実装したアプリケーションのサンプル実装です。


## 開発環境

| 開発キット | Visual Studio | バージョン |
| --- | --- | --- |
| [DirectXTK] | 2015 | December 17, 2019 |
| [DirectXTK] | 2017 | March 24, 2022 |
| [DirectXTK] | 2019 / 2022 | December 31, 2023 |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.ja.md) を参照してください。


## ディレクトリ構造

```
.
├─ Demo
│  └─ proj.d3d11.cmake  # CMake project
└─ thirdParty           # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り利用したサンプルです。

全ての機能を利用するフルバージョンのサンプルと最小限の機能のみが備わったミニマムバージョンのサンプルの二通りがあります。

フルバージョンではモーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
また、メニューボタンからモデルを切り替えることができます。

ミニマムバージョンでは単一のモデルの読み込み・表示、単一のモーションの再生、表情の設定、物理演算の設定などを行います。
ポーズの切り替えやモデルの切り替えには対応していません。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.d3d11.cmake

CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `nmake_msvcXXXX.bat` | 実行可能なアプリケーション |
| `proj_msvcXXXX.bat` | Visual Studio プロジェクト |

これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

開発キットとして [DirectXTK] を利用しています。
[thirdParty](README.md#thirdParty) の項目を参照して事前にダウンロードを行なってください。


## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリとビルドスクリプトが含まれます。

### DirectXTK のセットアップ

`script` ディレクトリ内のスクリプトを実行することで DirectXTK のダウンロード及びライブラリのビルドを行います。

| Visual Studio | スクリプト名 |
| --- | --- |
| 2015 | `setup_msvc2015.bat` |
| 2017 | `setup_msvc2017.bat` |
| 2019 | `setup_msvc2019.bat` |
| 2022 | `setup_msvc2022.bat` |

スクリプト内の `XTK_VERSION` を変更することでライブラリのバージョンを指定することが出来ます。

[DirectXTK]: https://github.com/Microsoft/DirectXTK

x64のリリースビルドを行うには.NET Framework 4.7.2のインストールが必要となります。
[DirectXTK Release Notes]: https://github.com/microsoft/DirectXTK#release-notes
