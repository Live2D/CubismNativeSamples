[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for DirectX 9.0c

DirectX 9.0c で実装したアプリケーションのサンプル実装です。


## 開発環境

| 開発キット | バージョン |
| --- | --- |
| [DirectX SDK] | 9.0c (June2010) |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.ja.md) を参照してください。


## ディレクトリ構造

```
.
└─ Demo
   └─ proj.d3d9.cmake   # CMake project using DirectX 9.0c
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

### proj.d3d9.cmake

ソースファイルと CMake ファイル、及びバッチファイルが含まれます。

* `scripts/nmake_xxx.bat` を実行すると `build` ディレクトリに実行可能なアプリケーションが生成されます。
* `scripts/proj_xxx.bat` を実行すると `build` ディレクトリにVisual Studioプロジェクトが生成されます。
* これらのスクリプトを実行した際にフルバージョンのサンプルを生成するか、ミニマムバージョンのサンプルを生成するかを選択することができます。

[DirectX SDK] を利用しているため、事前にインストールが必要です。

インストール時に `S1023` エラーが出た場合は、[マイクロソフトのサポート技術情報]を参考にしてください

インストール時に SDK がインストールされたパスが環境変数 `DXSDK_DIR` が設定されます。
ビルド時に環境変数未設定エラーが発生した場合は手動で上記の環境変数を設定してください。

[DirectX SDK]: https://www.microsoft.com/en-us/download/details.aspx?id=6812
[マイクロソフトのサポート技術情報]: https://blogs.msdn.microsoft.com/windows_multimedia_jp/2013/09/05/kbdirectx-sdk
