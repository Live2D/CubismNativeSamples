# Cubism Native Samples for DirectX 9.0c

DirectX 9.0cで実装したアプリケーションのサンプル実装です。


## 開発環境

| 開発キット | バージョン |
| --- | --- |
| DirectX SDK | 9.0c (June2010) |

その他の開発環境・動作確認環境はトップディレクトリにある[README.md](/README.md)を参照してください。


## ディレクトリ構造

```
D3D9
└─ Demo
   └─ proj.d3d9.cmake   # CMake project using DirectX 9.0c
```


## Demo

[Cubism Native Framework]の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.d3d9.cmake

ソースファイルとCMakeファイル、及びバッチファイルが含まれます。

- `nmake_xxx.bat`を実行すると実行可能なアプリケーションが作成されます。
- `proj_xxx.bat`を実行するとVisual Studioプロジェクトが作成されます。

DirectX SDKを利用しているため、事前に下記を行う必要があります。

1. [DirectX SDK]をインストールしてください
   - インストール時に`S1023`エラーが出た場合は、[マイクロソフトのサポート技術情報]を参考にしてください
1. SDKがインストールされたパスを変数名`DXSDK_FOR_CUBISM_D3D9`として環境変数に登録してください

[DirectX SDK]: https://www.microsoft.com/en-us/download/details.aspx?id=6812
[マイクロソフトのサポート技術情報]: https://blogs.msdn.microsoft.com/windows_multimedia_jp/2013/09/05/kbdirectx-sdk
