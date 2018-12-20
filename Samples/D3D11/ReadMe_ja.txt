Demo
    Live2Dの各機能を一通り利用したサンプルです。
    モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
    メニューボタンからモデルを切り替えることができます
    このフォルダ内に含まれるものは以下の通りです。
    
        proj.d3d11.cmake
            ソースファイルとCMakeファイル、及びバッチファイルが含まれています。
            
            サンプルにはDirectXTKを利用しているため、GitHubの公開リリース(https://github.com/Microsoft/DirectXTK/releases)より取得し、
            Samples/D3D11/thirdParty/DirectXTKへ展開した後(Samples\D3D11\thirdParty\DirectXTKの直下に解凍したBin,Inc等のフォルダ、各種ソリューションファイルが展開される形となります)、
            Samples/D3D11/thirdPartyにある、makeXTK_2013.bat,makeXTK_2015.batで纏めてビルドするか、
            DirectXTK_Desktop_2013.slnまたはDirectXTK_Desktop_2015.slnを開き、Win32・x64のソリューションターゲット、Debug・Releaseのソリューション構成それぞれでビルドを行ってください。
            (以下の構成で確認を行っております。
              ・VS2013 April 23, 2018 DirectXTK_Desktop_2013.slnでビルド https://github.com/Microsoft/DirectXTK/releases/tag/apr2018
                (DirectXTKのVS2013用ソリューションはApril 23, 2018を最後に提供を終了しています)
              ・VS2015 August 17, 2018 DirectXTK_Desktop_2015.slnでビルド https://github.com/Microsoft/DirectXTK/releases/tag/aug2018 )
            
            "proj_XXXX.bat"は各VisualStudioのソリューションファイルの生成バッチファイルになります。
            "nmake_XXXX.bat"は各VisualStudioのnmake用バッチファイルになります。
            
            バッチ実行にはCMake(version3.10以上)とVisualStudio(2013or2015)のインストールが必要になります。
