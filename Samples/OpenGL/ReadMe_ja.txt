Demo
    Live2Dの各機能を一通り利用したサンプルです。
    モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
    メニューボタンからモデルを切り替えることができます
    このフォルダ内に含まれるものは以下の通りです。
    
        proj.win.cmake
            Windows用のソースファイルとCMakeファイル、及びバッチファイルが含まれています。
            "proj_XXXX.bat"は各VisualStudioのソリューションファイルの生成バッチファイルになります。
            "nmake_XXXX.bat"は各VisualStudioのnmake用バッチファイルになります。
            
            バッチ実行にはCMake(version3.10以上)とVisualStudio(2013or2015)のインストールが必要になります。
            
            またサンプルにはglewとglfwを利用しているため
            下記githubからバイナリを取得しthirdPartyフォルダに展開してください。
            https://github.com/glfw/glfw
            glfw-3.2.1.bin.WIN32.zip(x86)
            glfw-3.2.1.bin.WIN64.zip(x64)
            
            https://github.com/nigels-com/glew
            glew-2.1.0-win32.zip
            
        proj.mac.cmake
            Mac用のソースファイルとCMakeファイル、及びシェルスクリプトが含まれています。
            
            シェルスクリプトの実行にはCMake(version3.10以上)とXCode及び、homebrewのインストールが必要になります。

        proj.android
            AndroidStudio用のプロジェクトファイルが含まれています。

        proj.ios
            iOS用のXcodeプロジェクトが含まれています。

thirdParty
    サンプルプロジェクトで使用するサードパーティライブラリが含まれるフォルダ


