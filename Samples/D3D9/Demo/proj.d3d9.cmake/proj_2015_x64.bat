@echo off

:: Make sure toolchain exists.
if exist "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2015 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win64 with Visual C++ Compiler 2015...

:: Build for native development.
mkdir proj_2015_x64
cd proj_2015_x64

cmake .. -G "Visual Studio 14 2015" -DCMAKE_GENERATOR_PLATFORM="x64" 

pause