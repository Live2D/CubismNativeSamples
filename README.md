# Cubism Native Samples

Live2D Cubism 4 Editorで出力したモデルを表示するアプリケーションのサンプル実装です。

Cubism Native FrameworkおよびCubism Coreライブラリと組み合わせて使用します。


## ディレクトリ構成

```
.
├─ Core             # Coreライブラリが含まれるディレクトリ
├─ Framework        # レンダリングやアニメーション機能などのソースコードが含まれるディレクトリ
└─ Samples
   ├─ Cocos2d-x     # Cocos2d-xのサンプルプロジェクトが含まれるディレクトリ
   ├─ D3D9          # DirectX 9.0cのサンプルプロジェクトが含まれるディレクトリ
   ├─ D3D11         # DirectX 11のサンプルプロジェクトが含まれるディレクトリ
   ├─ OpenGL        # OpenGLのサンプルプロジェクトが含まれるディレクトリ
   └─ Res           # モデルのファイルや画像などのリソースが含まれるディレクトリ
```


## Cubism Native Framework

モデルを表示、操作するための各種機能を提供します。

[Cubism Native Framework](https://github.com/Live2D/CubismNativeFramework)は、当リポジトリのサブモジュールとして含まれています。
当リポジトリをクローンした後、サブモジュールのクローンを実行することでファイルが追加されます。


## Live2D Cubism Core for Native

モデルをロードするためのライブラリです。
当リポジトリにはLive2D Cubism Core for Nativeは同梱されていません。

ダウンロードするには[こちら](https://www.live2d.com/download/cubism-sdk/download-native/)のページを参照ください。
ダウンロードしたZipファイルの中身を当リポジトリの`Core`ディレクトリにコピーし、プログラムにリンクさせてください。


## ビルド方法

ビルド方法についてはサンプルプロジェクトによって異なります。
各サンプルプロジェクトに同梱された`README.md`を参照ください。


## コンパイルオプション

プロジェクトにおいて、マクロ `USE_RENDER_TARGET` または `USE_MODEL_RENDER_TARGET` が有効な場合、
モデルがテクスチャへレンダリングされるようになります。
詳細はサンプルディレクトリ内の`LAppLive2DManager.cpp`を参照ください。


## SDKマニュアル

[Cubism SDK Manual](https://docs.live2d.com/cubism-sdk-manual/top/)


## 変更履歴

当リポジトリの変更履歴については[CHANGELOG.md](/CHANGELOG.md)を参照ください。


## 開発環境

| 開発ツール | バージョン |
| --- | --- |
| Android Studio | 3.5 |
| CMake | 3.15.2 |
| Visual Studio 2013 | Update 5 |
| Visual Studio 2015 | Update 3 |
| Visual Studio 2017 | 15.9.15 |
| XCode | 10.3 |

| Android SDK Tools | バージョン |
| --- | --- |
| Android NDK | r20 |
| Android SDK | 29.0.2 |
| CMake | 3.10.2 |

## 動作確認環境

| プラットフォーム | バージョン |
| --- | --- |
| Android | 9 |
| iOS | 12.4.1 |
| macOS | 10.14.6 |
| Windows 10 | 1903 |


## ライセンス

Cubism Native Samples および Cubism Native Framework は Live2D Open Software License で提供しています。
- Live2D Open Software License

  [日本語](https://www.live2d.com/eula/live2d-open-software-license-agreement_jp.html)
  [English](https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html)

Live2D Cubism Core for Native は Live2D Proprietary Software License で提供しています。
-  Live2D Proprietary Software License

   [日本語](https://www.live2d.com/eula/live2d-proprietary-software-license-agreement_jp.html)
   [English](https://www.live2d.com/eula/live2d-proprietary-software-license-agreement_en.html)

Live2D のサンプルモデルは Free Material License で提供しています。
- Free Material License

  [日本語](https://www.live2d.com/eula/live2d-free-material-license-agreement_jp.html)
  [English](https://www.live2d.com/eula/live2d-free-material-license-agreement_en.html)
  -  `./Samples/Res/Haru`
  -  `./Samples/Res/Hiyori`
  -  `./Samples/Res/Mark`
  -  `./Samples/Res/Natori`
  -  `./Samples/Res/Rice`

上記のモデルをご利用になられる場合、[こちら](https://docs.live2d.com/cubism-editor-manual/sample-model/)で各モデルに設定された利用条件に同意して頂く必要がございます。

直近会計年度の売上高が 1000 万円以上の事業者様がご利用になる場合は、SDKリリース(出版許諾)ライセンスに同意していただく必要がございます。
- [SDKリリース(出版許諾)ライセンス](https://www.live2d.com/ja/products/releaselicense)

*All business* users must obtain a Publication License. "Business" means an entity with the annual gross revenue more than ten million (10,000,000) JPY for the most recent fiscal year.
- [SDK Release (Publication) License](https://www.live2d.com/en/products/releaselicense)
