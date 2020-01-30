@echo off

setlocal

set SCRIPT_PATH=%~dp0
set MSVC_VERSION=2015
set MSVC_NUMBER=14
set VCVARSALL=%VS140COMNTOOLS%..\..\VC\vcvarsall.bat
set GENERATOR=proj

call "%SCRIPT_PATH%_msvc_common.bat"
if %errorlevel% neq 0 pause & exit /b %errorlevel%

pause & exit /b 0
