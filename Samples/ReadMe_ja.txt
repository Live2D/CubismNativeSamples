対応プラットフォームは以下の通りです。
    - Cocos2d-x ...確認済みver 3.16
    - D3D9 ...DirectX9 確認済み DirectX9.0c SDK June 2010 CMake 確認済み 3.12.0-rc1
    - D3D11 ...DirectX11 確認済み WindowsSDK 10.0.14393.0 CMake 確認済み 3.12.0-rc1
    - OpenGL  -Windows 確認済み VisualStudio2013,2015 CMake 確認済み 3.11.3
              -macOS   確認済み XCode 9.3,9.4 CMake 確認済み 3.12.0-rc1
              -Android 確認済み Android Studio 3.1.3
              -iOS     確認済み Xcode 9.3,9.4 iOS 11.4 

    - Res このフォルダには各サンプルで使用されるリソースが含まれています


既知の不具合

macOSのMojave(10.14)にて、glfw3.2.1を使用してビルドを行った場合、
ウィンドウをマウス等で動かすまで画面が黒いまま更新されないという不具合があります