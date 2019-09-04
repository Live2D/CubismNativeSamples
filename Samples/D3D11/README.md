# Cubism Native Samples for DirectX 11

DirectX 11で実装したアプリケーションのサンプル実装です。


## 開発環境

| 開発キット | Visual Studio | バージョン |
| --- | --- | --- |
| DirectXTK | 2013 | April 23, 2018 |
| DirectXTK | 2015 / 2017 | June 30, 2019 |

その他の開発環境・動作確認環境はトップディレクトリにある[README.md](/README.md)を参照してください。


## ディレクトリ構造

```
D3D11
├─ Demo
│  └─ proj.d3d11.cmake  # CMake project using DirectX 11
└─ thirdParty           # Third party libraries and scripts
```


## Demo

[Cubism Native Framework]の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.d3d11.cmake

ソースファイルとCMakeファイル、及びバッチファイルが含まれます。

- `nmake_xxx.bat`を実行すると実行可能なアプリケーションが作成されます。
- `proj_xxx.bat`を実行するとVisual Studioプロジェクトが作成されます。

DirectXTKを利用しているため、事前にthirdParty/DirectXTKに展開しておく必要があります。


## thirdParty

サンプルプロジェクトで利用するサードパーティライブラリとビルドスクリプトが含まれます。

### DirectXTK

DirectX 11用のプロジェクトで利用するDirectXTKを[GitHubの公開リリース]からダウンロード・展開したのち、
thirdPartyディレクトリにある`makeXTK_XXXX.bat`で纏めてビルドするか、
DirectXTK内の`DirectXTK_Desktop_XXXX.sln`を開き、
Win32・x64のソリューションターゲットとDebug・Releaseのソリューション構成によるビルドをそれぞれ行ってください。

[GitHubの公開リリース]: https://github.com/microsoft/DirectXTK/releases

