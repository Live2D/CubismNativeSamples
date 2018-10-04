@echo off

:: Make sure toolchain exists.
if exist "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2013 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win64 with Visual C++ Compiler 2013...

:: Build for native development.
mkdir proj_2013_x64
cd proj_2013_x64

cmake .. -G "Visual Studio 12 2013" -DCMAKE_GENERATOR_PLATFORM="x64" 

pause