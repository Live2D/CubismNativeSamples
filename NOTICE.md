## [注意事項] Visual Studio 2013 ご利用時の OpenGL サンプルビルドについて (2021-02-17)

`Visual Studio 2013` を使用した `OpenGL` サンプルビルドでは、`setup_glew_glfw.bat` をそのまま使用してビルドすると `GLEW` にてリンクエラーが発生します。
以下回避方法を確認し、ご利用いただきますようお願いいたします。

### 回避方法

* `GLEW 2.2.0` を使用する場合
  * `glew` プロジェクトから `libvcruntime.lib` の参照を削除する
  * `Visual Studio 2013` 以降の Visual Studio を使用する

* `GLEW 2.1.0` を使用する場合
  * `setup_glew_glfw_vs2013.bat` を使用して `thirdParty` のセットアップを行う


## [制限事項] macOS 11.0 Big Surへの対応状況について (2021-01-12)

現在公開中のCubism SDKは、macOS 11.0 Big Surには対応しておりません。
正常に動作できない可能性がありますので、OSのアップグレードをお控えいただきご利用いただきますようお願いいたします。
現在対応検討中となりますが、対応バージョンや時期につきましては改めてお知らせいたします。

またApple Sillicon版のmacにつきましても、全てのCubism 製品において対応しておりませんのでこちらも合わせてご了承ください。



## Cubism Core

### [注意事項] macOS Catalina での `.bundle` と `.dylib` の利用について

macOS Catalina 上で `.bundle` と `.dylib` を利用する際、公証の確認のためオンライン環境に接続している必要があります。

詳しくは、Apple社 公式ドキュメントをご確認ください。

* [Apple社 公式ドキュメント](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
