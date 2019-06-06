@echo off

if "%VSWHERE%"=="" set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

:: VS2017U2 contains vswhere.exe.
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -legacy -version [15^,16^) -property installationPath`) do (
    if exist "%%i" (
        set VS150VCVARSALL=%%i\VC\Auxiliary\Build\vcvarsall.bat
    )
)

:: Make sure toolchain exists.
if exist "%VS150VCVARSALL%" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2017 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win64 with Visual C++ Compiler 2017...

:: Build for native development.
mkdir proj_2017_x64
cd proj_2017_x64

cmake .. -G "Visual Studio 15 2017" -DCMAKE_GENERATOR_PLATFORM="x64" 

pause