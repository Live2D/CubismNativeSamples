#! /bin/bash

#brewでglfwとglewをインストール
brew install glew
brew install glfw


#作業ディレクトリを生成
mkdir build
cd build

#cmakeの実行
cmake .. -GXcode

