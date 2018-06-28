@echo off

:: Make sure toolchain exists.
if exist "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2015 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win32 with Visual C++ Compiler 2015...

:: Build for native development.
mkdir proj_2015_x86
cd proj_2015_x86

cmake .. -G "Visual Studio 14 2015" -DCMAKE_GENERATOR_PLATFORM="Win32" 

pause