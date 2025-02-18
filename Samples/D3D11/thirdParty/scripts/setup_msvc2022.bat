@echo off

setlocal

set XTK_VERSION=dec2023

set SCRIPT_PATH=%~dp0

:: Download and archive XTK.
call  "%SCRIPT_PATH%_setup_common.bat"
if %errorlevel% neq 0 pause & exit /b %errorlevel%

set SCRIPT_PATH=%~dp0
set MSVC_VERSION=2022
set MSVC_NUMBER=17
set VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
set /a _MSVC_RANGE_S=MSVC_NUMBER
set /a _MSVC_RANGE_E=MSVC_NUMBER+1
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -legacy -version [%_MSVC_RANGE_S%^,%_MSVC_RANGE_E%^) -property installationPath`) do (
  set VCVARSALL=%%i\VC\Auxiliary\Build\vcvarsall.bat
)

:: Build XTK project.
call "%SCRIPT_PATH%_msvc_common.bat" %XTK_VERSION%
if %errorlevel% neq 0 pause & exit /b %errorlevel%

pause & exit /b 0
