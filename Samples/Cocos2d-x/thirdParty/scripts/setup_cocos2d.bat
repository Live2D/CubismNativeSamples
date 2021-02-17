@echo off

set COCOS_VERSION=3.17.2

set SCRIPT_PATH=%~dp0

:::::::::::::::::::
:: Setup Cocos2d ::
:::::::::::::::::::

cd %SCRIPT_PATH%\..

:: Download and extract the archive.
echo - Setup Cocos2d
echo Downloading...
curl -fsSL -o cocos2d.zip ^
  "https://digitalocean.cocos2d-x.org/Cocos2D-X/cocos2d-x-%COCOS_VERSION%.zip"
if %errorlevel% neq 0 pause & exit /b %errorlevel%
echo Extracting...
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force cocos2d.zip ."
if %errorlevel% neq 0 pause & exit /b %errorlevel%
ren cocos2d-x-%COCOS_VERSION% cocos2d
del cocos2d.zip

echo.
pause
