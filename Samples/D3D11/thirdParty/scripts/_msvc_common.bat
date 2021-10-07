@echo off

setlocal
set VARIABLE_EXISTS=TRUE
if "%MSVC_VERSION%" equ "" set VARIABLE_EXISTS=FALSE
if "%VCVARSALL%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [CubismNativeSamples] Invalid batch call.
  exit /b 1
)
endlocal

set XTK_VERSION=%1
echo XTK version uses %XTK_VERSION%.

cd %~dp0

rem Make sure toolchain exists.
if not exist "%VCVARSALL%" (
  echo [CubismNativeSamples] Visual C++ Compiler %MSVC_VERSION% not found.
  exit /b 1
)


rem Build for native development.
echo Building Win32 and x64 with Visual C++ Compiler %MSVC_VERSION%...

cd ..\DirectXTK

call "%VCVARSALL%" x86
if %errorlevel% neq 0 exit /b %errorlevel%

if "%MSVC_VERSION%" geq "2017" (
msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=X86;Configuration=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=X86;Configuration=Release
if %errorlevel% neq 0 exit /b %errorlevel%
) else (
msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=Win32;Configuration=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=Win32;Configuration=Release
if %errorlevel% neq 0 exit /b %errorlevel%
)

call "%VCVARSALL%" x64
if %errorlevel% neq 0 exit /b %errorlevel%

msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=x64;Configuration=Debug
if %errorlevel% neq 0 exit /b %errorlevel%

msbuild "DirectXTK_Desktop_%MSVC_VERSION%.sln" /v:m /t:build /p:Platform=x64;Configuration=Release
if %errorlevel% neq 0 exit /b %errorlevel%

exit /b 0
