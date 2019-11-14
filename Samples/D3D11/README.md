# Cubism Native Samples for DirectX 11

DirectX 11 で実装したアプリケーションのサンプル実装です。


## 開発環境

| 開発キット | Visual Studio | バージョン |
| --- | --- | --- |
| DirectXTK | 2013 | April 23, 2018 |
| DirectXTK | 2015 / 2017 / 2019 | October 17, 2019 |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.md) を参照してください。


## ディレクトリ構造

```
D3D11
├─ Demo
│  └─ proj.d3d11.cmake  # CMake project using DirectX 11
└─ thirdParty           # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.d3d11.cmake

ソースファイルと CMake ファイル、及びバッチファイルが含まれます。

- `nmake_xxx.bat` を実行すると実行可能なアプリケーションが作成されます。
- `proj_xxx.bat` を実行するとVisual Studioプロジェクトが作成されます。

DirectXTK を利用しているため、事前に thirdParty ディレクトリに展開しておく必要があります。
詳細は下記の [thirdParty](README.md#thirdParty) の項目を参照してください。


## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリとビルドスクリプトが含まれます。

### DirectXTK

サンプルプロジェクトで利用する DirectXTK の自動展開スクリプトが含まれます。

`setup_[利用する Visual Studio のバージョン].bat` を実行することで、
プロジェクトで利用する DirectXTK のソースファイルのダウンロードとビルドを行います。

スクリプト内の `XTK_VERSION` を変更することでライブラリのバージョンを指定することが出来ます。
