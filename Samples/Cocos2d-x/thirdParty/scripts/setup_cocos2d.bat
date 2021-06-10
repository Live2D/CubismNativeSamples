@echo off

set COCOS_COMMIT_HASH=95e5d868ce5958c0dadfc485bdda52f1bc404fe0

set SCRIPT_PATH=%~dp0

:::::::::::::::::::
:: Setup Cocos2d ::
:::::::::::::::::::

cd %SCRIPT_PATH%\..

:: Download and extract the archive.
echo - Setup Cocos2d
cd ..\..\..\
mkdir _l2d_tmp
cd _l2d_tmp
git init
git remote add origin https://github.com/cocos2d/cocos2d-x.git
git fetch --depth 1 origin %COCOS_COMMIT_HASH%
git reset --hard FETCH_HEAD
python "download-deps.py" "--remove-download" "yes"
git submodule update --init
rmdir /s /q .git
cd ..
robocopy _l2d_tmp %SCRIPT_PATH%\..\cocos2d /MIR
rmdir /s /q _l2d_tmp

echo.
pause
