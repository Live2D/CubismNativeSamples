@echo off

setlocal

set XTK_VERSION=apr2018

:: Download and archive XTK.
call .\_setup_common.bat
if %errorlevel% neq 0 pause & exit /b %errorlevel%

set MSVC_VERSION=2013
set VCVARSALL=%VS120COMNTOOLS%..\..\VC\vcvarsall.bat

:: Build XTK project.
call .\_msvc_common.bat
if %errorlevel% neq 0 pause & exit /b %errorlevel%

pause & exit /b 0
