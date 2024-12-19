[English](README.md) / [日本語](README.ja.md)

---

# Cubism Native Samples for Metal

Metal で実装したアプリケーションのサンプル実装です。
制限事項があります、トップディレクトリにある [NOTICE.md](/NOTICE.ja.md) を参照してください。


## 開発環境

| サードパーティ | バージョン |
| --- | --- |
| [ios-cmake]    | 4.5.0     |
| [stb_image.h]  | 2.30      |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.ja.md) を参照してください。


## ディレクトリ構造

```
.
├─ Demo
│  └─ proj.ios.cmake        # CMake project for iOS or Mac Catalyst
└─ thirdParty               # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り使用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.ios.cmake

iOS もしくは Mac Catalyst用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます。

スクリプト実行時にiOS用かiOS Simulator用か、またはMac Catalyst用か選択できます。

| スクリプト名 | 生成物 |
| --- | --- |
| `proj_xcode` | Xcode プロジェクト |

Appleシリコン搭載のMacでのiOS Simulator用ビルド時は下記の手順を行ってください。

1. Xcode の `Project設定 - TARGETS - Demo` および `Framework` の `Build Settings - Architectures - Architectures` の記載を実行環境に合わせて変更してください。


Mac Catalyst用ビルド時は下記の手順を行なってください。

1. Xcode の `Project設定 - TARGETS - Demo - General - Supported Destinations` の `+` を押下し、`Mac > Mac Catalyst` を選択し、モーダルビューの`Enable`を押下してください。
2. Xcode の `Project設定 - TARGETS - Demo` および `Framework` の `Build Settings - Architectures - Architectures` の記載を実行環境に合わせて変更してください。
3. Xcode の `Project設定 - TARGETS - Demo` および `Framework` の `Build Settings - Architectures - Base SDK` を `iOS` に変更してください。
もしくはApple公式Tutorialsを参考ください。
[turning-on-mac-catalyst]:https://developer.apple.com/tutorials/mac-catalyst/turning-on-mac-catalyst



CMake のツールチェーンとして [ios-cmake] を使用しています。
[thirdParty](README.md#thirdParty) の項目を参照して事前にダウンロードを行なってください。

[ios-cmake]: https://github.com/leetal/ios-cmake



## thirdParty

サンプルプロジェクトで使用するサードパーティライブラリと自動展開スクリプトが含まれます。

## ios-cmake のセットアップ

`script` ディレクトリ内の `setup_ios_cmake` を実行することで ios-cmake のダウンロードを行います。

スクリプト内の `IOS_CMAKE_VERSION` を変更することで、ダウンロードするバージョンを変更できます。

[ios-cmake]: https://github.com/leetal/ios-cmake
[stb_image.h]: https://github.com/nothings/stb/blob/master/stb_image.h

