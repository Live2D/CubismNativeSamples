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


:: Initialize VS2015 32-bit toolchain.
cd /d "%VS140COMNTOOLS%..\..\VC"
call vcvarsall.bat

cd /d %~d0%~p0

:: Build for native development.
mkdir nmake_2015_x86
cd nmake_2015_x86
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE="Release"
nmake

pause