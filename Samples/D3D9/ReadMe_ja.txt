Demo
    Live2Dの各機能を一通り利用したサンプルです。
    モーションの再生、表情の設定、ポーズの切り替え、物理演算の設定などを行います。
    メニューボタンからモデルを切り替えることができます
    このフォルダ内に含まれるものは以下の通りです。
    
        proj.d3d9.cmake
            ソースファイルとCMakeファイル、及びバッチファイルが含まれています。
            
            サンプルにはDirectX9.0c SDKを利用しているため、
            DirectX SDK9.0cインストーラをダウンロードしてインストールしてください。
            (https://www.microsoft.com/en-us/download/details.aspx?id=6812)
            (※インストール時にS1023 エラーが出た場合は下記ページを参考にしてください。
            https://blogs.msdn.microsoft.com/windows_multimedia_jp/2013/09/05/kbdirectx-sdk/
            )
            
            インストール後、SDKがインストールされた箇所へ、変数名 DXSDK_FOR_CUBISM_D3D9 としてパスを通してください。
            (例・変数名 DXSDK_FOR_CUBISM_D3D9  変数値 C:\Program Files (x86)\Microsoft DirectX SDK (June 2010))
            
            "proj_XXXX.bat"は各VisualStudioのソリューションファイルの生成バッチファイルになります。
            "nmake_XXXX.bat"は各VisualStudioのnmake用バッチファイルになります。
            (上記の環境変数を設定した後に実行してください)
            
            バッチ実行にはCMake(version3.10以上)とVisualStudio(2013or2015)のインストールが必要になります。

