@echo off

set GLEW_VERSION=2.1.0
set GLFW_VERSION=3.3

set SETUP_DIR=%~dp0

::::::::::::::::
:: Setup GLEW ::
::::::::::::::::

cd %SETUP_DIR%

:: Download and extract the archive.
echo - Setup GLEW
echo Downloading...
curl -fsSL -o glew.zip ^
  https://github.com/nigels-com/glew/releases/download/glew-%GLEW_VERSION%/glew-%GLEW_VERSION%.zip
echo Extracting...
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force glew.zip ."
del glew.zip

::::::::::::::::
:: Setup GLFW ::
::::::::::::::::

echo.

:: Download and extract the archive.
echo - Setup GLFW
echo Downloading...
curl -fsSL -o glfw.zip ^
  https://github.com/glfw/glfw/releases/download/%GLFW_VERSION%/glfw-%GLFW_VERSION%.zip
echo Extracting...
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force glfw.zip ."
del glfw.zip

echo.
pause
