[English](NOTICE.md) / [日本語](NOTICE.ja.md)

---

# お知らせ

## [注意事項] Cocos2d-xセットアップのエラーについて (2022-03-15)

Cocos2d-xサンプルプロジェクトのセットアップ時に実行するsetup-cocos2dバッチにおいてエラーが発生いたします。
これはGitHubがGitプロトコルセキュリティを改善したことに起因し、GitにてCocos2d-xのsubmodule取得が失敗するためです。
具体的には`git://`経由での接続ができなくなったためエラーが発生しています。

詳しくはGitHub公式ブログをご確認ください。

* [GitHub 公式ブログ](https://github.blog/2021-09-01-improving-git-protocol-security-github/)

### 回避方法

* ターミナルもしくはコマンドプロンプトにて以下のコマンドを実行することで、`git://`を用いた接続を`https://`に置き換える
  * `git config --global url."https://github.com/".insteadOf git://github.com/`
    * このコマンドは実行端末の全ユーザのgitconfigに影響します。ご了承ください。


## [注意事項] Metal及びCocos2d-x v4.0 の macOSサンプルビルドにおけるメモリリークについて (2021-12-09)

Metal及びCocos2d-x v4.0 の macOSサンプルビルドにおいてメモリリークが発生いたします。
これはMetalフレームワーク内で発生するため、公式の対応をお待ち下さい。
本件のお問い合わせについて弊社ではお答えいたしかねますので、ご了承ください。

詳細については Apple Developer Forums をご確認ください。

* [Apple Developer Forums - Memory leak in MTLCommandBuffer?](https://developer.apple.com/forums/thread/120931)


## [制限事項] Apple製品の対応状況について (2021-12-09 更新)

macOS Monterey v12.0につきまして、Cubism 4 SDK for Native R4 (4-r.4) にて対応いたしました。
また、Cubism 4 SDK for Native R4 Beta1 (4-r.4-beta.1) 以降のバージョンでApple Silicon版のmacに対応しております。
※Cubism Editorは現在Apple Silicon製のMacに対応しておりません、ご了承ください。


## [制限事項] Cocos2d-x v4.0 の Linux(Ubuntu)サンプルビルドについて (2021-04-15)

Cocos2d-x v4.0 は `Ubuntu 20.04` ビルドに対応しておりません。
使用する際は以下の回避方法を確認し、いずれかを適用してご利用いただきますようお願いいたします。

### 回避方法

* `Ubuntu 16.04` または `Ubuntu 18.04` を使用する

* 以下issueを確認し、`Cocos2d-x v4.0` で使用されている `libchipmunk` ライブラリを差し替える
  * [cocos2d/cocos2d-x linking error when integrating with libchipmunk on linux#20471](https://github.com/cocos2d/cocos2d-x/issues/20471)
  * WARNING: この回避方法を使用してビルドしたプロジェクトはいかなる場合でも保守いたしかねます


## [注意事項] Visual Studio 2013 ご利用時の OpenGL サンプルビルドについて (2021-02-17)

`Visual Studio 2013` を使用した `OpenGL` サンプルビルドでは、`setup_glew_glfw.bat` をそのまま使用してビルドすると `GLEW` にてリンクエラーが発生します。
以下回避方法を確認し、ご利用いただきますようお願いいたします。

### 回避方法

* `GLEW 2.2.0` を使用する場合
  * `glew` プロジェクトから `libvcruntime.lib` の参照を削除する
  * `Visual Studio 2013` 以降の Visual Studio を使用する

* `GLEW 2.1.0` を使用する場合
  * `setup_glew_glfw_vs2013.bat` を使用して `thirdParty` のセットアップを行う


## Cubism Core

### [注意事項] macOS Catalina 以降のバージョンでの `.bundle` と `.dylib` の利用について

macOS Catalina 上で `.bundle` と `.dylib` を利用する際、公証の確認のためオンライン環境に接続している必要があります。

詳しくは、Apple社 公式ドキュメントをご確認ください。

* [Apple社 公式ドキュメント](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
