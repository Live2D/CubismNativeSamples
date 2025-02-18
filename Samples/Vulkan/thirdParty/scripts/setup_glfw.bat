@echo off

set GLFW_VERSION=3.4

set SCRIPT_PATH=%~dp0

cd %SCRIPT_PATH%/..

::::::::::::::::
:: Setup GLFW ::
::::::::::::::::

echo.

:: Download and extract the archive.
echo - Setup GLFW %GLFW_VERSION%
echo Downloading...
curl -fsSL -o glfw.zip ^
  https://github.com/glfw/glfw/releases/download/%GLFW_VERSION%/glfw-%GLFW_VERSION%.zip
if %errorlevel% neq 0 pause & exit /b %errorlevel%
echo Extracting...
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force glfw.zip ."
if %errorlevel% neq 0 pause & exit /b %errorlevel%
ren glfw-%GLFW_VERSION% glfw
del glfw.zip

echo.
pause
