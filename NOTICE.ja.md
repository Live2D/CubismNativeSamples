[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# お知らせ

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
