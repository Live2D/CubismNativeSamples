@echo off

setlocal
set VARIABLE_EXISTS=TRUE
if "%MSVC_VERSION%" equ "" set VARIABLE_EXISTS=FALSE
if "%MSVC_NUMBER%" equ "" set VARIABLE_EXISTS=FALSE
if "%VCVARSALL%" equ "" set VARIABLE_EXISTS=FALSE
if "%GENERATOR%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [CubismNativeSamples] Invalid batch call.
  exit /b 1
)
endlocal

cd %~dp0

rem ==============
rem Dialog Options
rem ==============

rem Architecture selection
echo.
echo Select which architecture to use.
echo **************************************************
echo 1. x86 (Win32)
echo 2. x64 (Win64)
echo.
choice /c:12 /n /m ">"
set SELECTED=%errorlevel%
if "%SELECTED%" equ "1" (
  set ARCHITECTURE=x86
  set CMAKE_A_OPTION=Win32
) else if "%SELECTED%" equ "2" (
  set ARCHITECTURE=x64
  set CMAKE_A_OPTION=x64
) else (
  echo [CubismNativeSamples] Invalid option.
  exit /b 1
)

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
echo Building %ARCHITECTURE% with Visual C++ Compiler %MSVC_VERSION%...

rem Initialize Visual Studio toolchain.
call "%VCVARSALL%" %ARCHITECTURE%
if %errorlevel% neq 0 exit /b %errorlevel%

rem Build for native development.
if "%CORE_CRL_MD%" equ "ON" (
  set CORE_CRL=md
) else (
  set CORE_CRL=mt
)

set BUILD_PATH=..\build\%GENERATOR%_msvc%MSVC_VERSION%_%ARCHITECTURE%_%CORE_CRL%

if "%GENERATOR%" equ "nmake" (
  cmake -S .. -B "%BUILD_PATH%" ^
    -G "NMake Makefiles" ^
    -D CMAKE_BUILD_TYPE="Release" ^
    -D CORE_CRL_MD=%CORE_CRL_MD%
  if %errorlevel% neq 0 exit /b %errorlevel%
  cd "%BUILD_PATH%" && nmake
  if %errorlevel% neq 0 exit /b %errorlevel%
) else if "%GENERATOR%" equ "proj"  (
  cmake -S .. -B "%BUILD_PATH%" ^
    -G "Visual Studio %MSVC_NUMBER% %MSVC_VERSION%" ^
    -A %CMAKE_A_OPTION% ^
    -D CORE_CRL_MD=%CORE_CRL_MD%
  if %errorlevel% neq 0 exit /b %errorlevel%
) else (
  echo [CubismNativeSamples] Invalid option.
  goto :error
)

exit /b 0
