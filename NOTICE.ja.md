[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# お知らせ

## [注意事項] Visual Studio 2017 ビルドについて (2025-05-29)

Visual Studio 2017 は Windows 10 SDK 10.0.17763.0 までの提供となっており、その状態では実行に問題が発生する場合がございます。
Visual Studio 2017 で動作させる場合は手動でWindows SDKの最新版をインストールしてご利用ください。

## [注意事項] Mac CatalystにおけるMetalレンダラのビルドについて (2025-05-29)

Mac CatalystにおけるMetalレンダラのビルドにつきまして、CMakeが3.27未満でないとNSURLが見つからないというエラーが発生する現象を確認しております。
本プラットフォーム利用時はCMakeを3.26以前をご利用ください。

## [注意事項] CMake 4.0 の対応について (2025-05-29)

CMake 4.0対応につきまして、本製品で利用している `GLEW 2.2.0` がCMake 4.0に対応していないため、本サンプルはCMake 4.0に対応しておりません。
GLEWが対応され次第追従いたしますのであらかじめご了承ください。

詳しくは以下のissueをご参照ください。

* [GLEW issue](https://github.com/nigels-com/glew/issues/432)

## [注意事項] iOS ARM ビルドについて (2025-05-29)

iOS実機端末で利用する ARM ビルドにつきまして、Cubism SDK for Nativeで使用している ios-cmake 4.5.0 の影響で実機端末へのインストール時にエラーが発生することを確認しております。
詳細については継続して調査いたします。

### 回避方法

1. ios-cmake のバージョンを 4.4.1 等に戻す

2. .xcodeproj の設定画面にて TARGETS で Demo 等ビルドするアプリを選択し、Build Setting > User-Defined から以下をすべてYESに変更する
  2.1. CODE_SIGNING_ALLOWED 及びそのツリー内部
  2.2. CODE_SIGNING_REQUIED 及びそのツリー内部


## [注意事項] Cubism SDK for NativeのCocos2d-xサポートについて (2024-12-19)

Cubism SDK for NativeのCocos2d-x対応につきまして、Cubism 5 SDK for Native R2でサポートを終了いたしました。
本件のお問い合わせにつきましては、[お問合せフォーム](<https://www.live2d.jp/contact/>)からご連絡ください。　


## [注意事項] Apple社のPrivacy Manifest Policy対応について

Apple社が対応を必要としているPrivacy Manifest Policyについて、本製品では指定されているAPI及びサードパーティ製品を使用しておりません。
もし本製品で対応が必要と判断した場合、今後のアップデートにて順次対応する予定です。
詳しくはApple社が公開しているドキュメントをご確認ください。

* [Privacy updates for App Store submissions](https://developer.apple.com/news/?id=3d8a9yyh)


## [注意事項] Linux OpenGL環境でのWaylandのサポートについて (2024-03-26)

本製品がLinux OpenGL環境で利用している `GLFW 3.4` が、Wayland向けにコンパイルできるようになりました。
しかし、 `GLEW 2.2.0` では現在Wayland(EGL)向けのコンパイルを行うと `glewInit` が正常に動作しない状態であるため、本サンプルはWaylandに対応しておりません。

Linux OpenGL環境をご利用の方は、引き続き通常通りX11向けビルドをご利用ください。

詳しくは以下のissueを参照ください。

* [GLEW issue](https://github.com/nigels-com/glew/issues/172)


## [注意事項] Metal の Mac Catalyst のビルドエラーについて (2024-01-25)

MetalのMac Catalystのビルド時にエラーが発生する場合がございます。
具体的には`cmake3.27.0`以上を使用してプロジェクトを生成した際に`'Foundation/NSURLHandle.h' file not found`といったエラーが発生いたします。
詳細については継続して調査いたします。

### 回避方法

* cmakeを3.27未満のバージョンで使用する


## [注意事項] Metal の macOSサンプルビルドにおけるメモリリークについて (2021-12-09)

Metal の macOSサンプルビルドにおいてメモリリークが発生いたします。
これはMetalフレームワーク内で発生するため、公式の対応をお待ち下さい。
本件のお問い合わせについて弊社ではお答えいたしかねますので、ご了承ください。

詳細については Apple Developer Forums をご確認ください。

* [Apple Developer Forums - Memory leak in MTLCommandBuffer?](https://developer.apple.com/forums/thread/120931)


## Cubism Core

### [注意事項] macOS Catalina 以降のバージョンでの `.bundle` と `.dylib` の利用について

macOS Catalina 上で `.bundle` と `.dylib` を利用する際、公証の確認のためオンライン環境に接続している必要があります。

詳しくは、Apple社 公式ドキュメントをご確認ください。

* [Apple社 公式ドキュメント](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
