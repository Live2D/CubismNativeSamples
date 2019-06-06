@echo off

if "%VSWHERE%"=="" set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

:: VS2017U2 contains vswhere.exe.
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -legacy -version [15^,16^) -property installationPath`) do (
    if exist "%%i" (
        set VS150VCVARSALLDIR=%%i\VC\Auxiliary\Build
    )
)

:: Make sure toolchain exists.
if exist "%VS150VCVARSALLDIR%\vcvarsall.bat" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2017 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win64 with Visual C++ Compiler 2017...


:: Initialize VS2017 64-bit toolchain.
cd /d %VS150VCVARSALLDIR%
call vcvarsall.bat amd64

cd /d %~d0%~p0

:: Build for native development.
mkdir nmake_2017_x64
cd nmake_2017_x64
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE="Release"
nmake

pause