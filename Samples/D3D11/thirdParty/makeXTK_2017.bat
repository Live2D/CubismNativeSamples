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

echo Building Win32 and x64 with Visual C++ Compiler 2017...

:: Build for native development.

call "%VS150VCVARSALL%" x86 

MSBuild DirectXTK/DirectXTK_Desktop_2017.sln /t:build /p:Configuration=Debug;Platform="Win32"
MSBuild DirectXTK/DirectXTK_Desktop_2017.sln /t:build /p:Configuration=Release;Platform="Win32"

call "%VS150VCVARSALL%" amd64

MSBuild DirectXTK/DirectXTK_Desktop_2017.sln /t:build /p:Configuration=Debug;Platform="x64"
MSBuild DirectXTK/DirectXTK_Desktop_2017.sln /t:build /p:Configuration=Release;Platform="x64"

pause
