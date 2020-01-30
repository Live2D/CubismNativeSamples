@echo off

setlocal
set VARIABLE_EXISTS=TRUE
if "%XTK_VERSION%" equ "" set VARIABLE_EXISTS=FALSE
if "%VARIABLE_EXISTS%" equ "FALSE" (
  echo [CubismNativeSamples] Invalid batch call.
  exit /b 1
)
endlocal

setlocal

cd %~dp0

:: Download and extract the archive.
echo - Setup DirectXTK
echo Downloading...
cd ..
curl -fsSL -o xtk.zip ^
  https://github.com/microsoft/DirectXTK/archive/%XTK_VERSION%.zip
if %errorlevel% neq 0 pause & exit /b %errorlevel%
echo Extracting...
powershell "$progressPreference = 'silentlyContinue'; expand-archive -force xtk.zip ."
if %errorlevel% neq 0 pause & exit /b %errorlevel%
mkdir DirectXTK >nul 2>&1
pushd DirectXTK-* && xcopy /y /e /q * ..\DirectXTK\ >nul && popd
del xtk.zip
for /f %%a in ('dir /ad /b /w DirectXTK-*') do (rd /s /q %%a)
cd scripts

endlocal

exit /b 0
