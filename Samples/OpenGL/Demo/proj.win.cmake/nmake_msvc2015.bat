@echo off

setlocal

set MSVC_VERSION=2015
set MSVC_NUMBER=14
set VCVARSALL=%VS140COMNTOOLS%..\..\VC\vcvarsall.bat
set GENERATOR=nmake

call .\_msvc_common.bat
if %errorlevel% neq 0 pause & exit /b %errorlevel%

pause & exit /b 0