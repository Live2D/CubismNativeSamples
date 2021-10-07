# Cubism Native Samples for Metal

Metal で実装したアプリケーションのサンプル実装です。
制限事項があります、トップディレクトリにある [NOTICE.md](/NOTICE.md) を参照してください。


## 開発環境

| サードパーティ | バージョン |
| --- | --- |
| [ios-cmake]    | 3.1.2      |
| [stb_image.h]  | 2.23       |

その他の開発環境・動作確認環境はトップディレクトリにある [README.md](/README.md) を参照してください。


## ディレクトリ構造

```
.
├─ Demo
│  └─ proj.ios.cmake        # CMake project for iOS
└─ thirdParty               # Third party libraries and scripts
```


## Demo

[Cubism Native Framework] の各機能を一通り使用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### proj.ios.cmake

iOS 用の CMake プロジェクトです。

`script` ディレクトリのスクリプトを実行すると `build` ディレクトリに CMake 成果物が生成されます

| スクリプト名 | 生成物 |
| --- | --- |
| `proj_xcode` | Xcode プロジェクト |

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

