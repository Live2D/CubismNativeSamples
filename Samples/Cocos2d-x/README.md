# Cubism Native Samples for Cocos2d-x

Cocos2d-xで実装したアプリケーションのサンプル実装です。


## 開発環境

| フレームワーク | バージョン |
| --- | --- |
| Cocos2d-x | 3.17.2 |

その他の開発環境・動作確認環境はトップディレクトリにある[README.md](/README.md)を参照してください。


## ディレクトリ構造

```
Cocos2d-x
├─ cocos2d              # Cocos2d-x libraries
└─ Demo
   ├─ Classes           # Sources for sample projects
   ├─ proj.android      # Android Studio project
   ├─ proj.ios_mac      # Xcode project for iOS and macOS
   ├─ proj.win32        # Visual Studio project for Windows
   └─ Resources         # Resources of cocos2d-x template project
```


## cocos2d

サンプルプロジェクトで使用するCocos2d-xライブラリ用のディレクトリです。 
[公式サイトのダウンロードページ]もしくは[GitHubの公開リリース]からライブラリをダウンロードして、
本ディレクトリで展開したのち`download-deps.py`を実行して依存ライブラリのダウンロードを行ってください。

[公式サイトのダウンロードページ]: https://cocos2d-x.org/download/version
[GitHubの公開リリース]: https://github.com/cocos2d/cocos2d-x/releases

Note: 本ディレクトリ名は最新のCocos2d-xで生成されるプロジェクトの命名規則に合わせて
`cocos2d-x`から変更しました。


## Demo

[Cubism Native Framework]の各機能を一通り利用したサンプルです。
モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
メニューボタンからモデルを切り替えることができます。

[Cubism Native Framework]: https://github.com/Live2D/CubismNativeFramework

このディレクトリ内に含まれるものは以下の通りです。

### Classes

サンプルプロジェクトで使用するソースコードが含まれます。

### proj.android

Android用のAndroid Studioプロジェクトが含まれます。

Note: 本ディレクトリ名は最新のCocos2d-xで生成されるプロジェクトの命名規則に合わせて
`proj.android-studio`から変更しました。

### proj.ios_mac

iOSおよびmacOS用のXcodeプロジェクトが含まれます。

### proj.win32

Windows(x86)用のVisual Studio 2017プロジェクトが含まれます。

### Resources

Cocos2d-xのテンプレートプロジェクトで参照しているリソースが含まれます。
