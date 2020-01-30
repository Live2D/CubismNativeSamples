@echo off

setlocal
set VARIABLE_EXISTS=TRUE
if "%MSVC_VERSION%" equ "" set VARIABLE_EXISTS=FALSE
if "%MSVC_NUMBER%" equ "" set VARIABLE_EXISTS=FALSE
if "%VCVARSALL%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [CubismNativeSamples] Invalid batch call.
  exit /b 1
)
endlocal

cd %~dp0

rem ==============
rem Dialog Options
rem ==============

rem Core C Runtime selection
echo.
echo Select which cubism core c runtime library to use
echo **************************************************
echo 1. use the multithread-specific and DLL-specific version of the run-time library (MD)
echo 2. use the multithread, static version of the run-time library (MT)
echo.
choice /c:12 /n /m ">"
set SELECTED=%errorlevel%
if "%SELECTED%" equ "1" (
  set CORE_CRL_MD=ON
) else if "%SELECTED%" equ "2" (
  set CORE_CRL_MD=OFF
) else (
  echo [CubismNativeSamples] Invalid option.
  exit /b 1
)

rem ========
rem Validate
rem ========

rem Make sure toolchain exists.
if not exist "%VCVARSALL%" (
  echo [CubismNativeSamples] Visual C++ Compiler %MSVC_VERSION% not found.
  exit /b 1
)

rem =====
rem Build
rem =====

rem Execute build.
echo Building x86 with Visual C++ Compiler %MSVC_VERSION%...

rem Initialize Visual Studio toolchain.
call "%VCVARSALL%" x86
if %errorlevel% neq 0 exit /b %errorlevel%

rem Build for native development.
if "%CORE_CRL_MD%" equ "ON" (
  set CORE_CRL=md
) else (
  set CORE_CRL=mt
)

set BUILD_PATH=..\build\proj_msvc%MSVC_VERSION%_x86_%CORE_CRL%

cmake -S ..\.. -B "%BUILD_PATH%" ^
  -G "Visual Studio %MSVC_NUMBER% %MSVC_VERSION%" ^
  -A Win32 ^
  -D CORE_CRL_MD=%CORE_CRL_MD%
if %errorlevel% neq 0 exit /b %errorlevel%

exit /b 0
