@echo off

:: Make sure toolchain exists.
if exist "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" goto :build

:: Return in error if not...
echo [CubismNativeSamples] Visual C++ Compiler 2015 not found.

pause
exit /b -1

:: Execute build.
:build

echo Building Win32 and x64 with Visual C++ Compiler 2015...

:: Build for native development.

call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"

MSBuild DirectXTK/DirectXTK_Desktop_2015.sln /t:build /p:Configuration=Debug;Platform="Win32"
MSBuild DirectXTK/DirectXTK_Desktop_2015.sln /t:build /p:Configuration=Release;Platform="Win32"
MSBuild DirectXTK/DirectXTK_Desktop_2015.sln /t:build /p:Configuration=Debug;Platform="x64"
MSBuild DirectXTK/DirectXTK_Desktop_2015.sln /t:build /p:Configuration=Release;Platform="x64"

pause
