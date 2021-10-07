## [制限事項] Metal APIの対応状況について (2021-10-07)

iOSのみの対応となっております。MacOSについては現在対応中となります。
レンダーターゲット機能及び異方性フィルタリング機能が未対応となっております。


## [制限事項] Apple製品の対応状況について (2021-10-07 更新)

Cubism SDK for Native 4-r.4-beta.1 にてApple Sillicon版のmacに対応いたしました。ぜひご利用ください。
※Cubism Editorは現在対応しておりませんので、ご了承ください。


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

### [注意事項] macOS Catalina での `.bundle` と `.dylib` の利用について

macOS Catalina 上で `.bundle` と `.dylib` を利用する際、公証の確認のためオンライン環境に接続している必要があります。

詳しくは、Apple社 公式ドキュメントをご確認ください。

* [Apple社 公式ドキュメント](https://developer.apple.com/documentation/security/notarizing_your_app_before_distribution)

---

©Live2D
