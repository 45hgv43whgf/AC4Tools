@echo off
if /i "%VSCMD_ARG_TGT_ARCH%"=="x86" exit /b 0
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto Missing
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL goto Missing
call "%VSINSTALL%\VC\Auxiliary\Build\vcvars32.bat"
exit /b %errorlevel%
:Missing
echo Error: Visual Studio x86 C++ tools not found. Install Visual Studio Build Tools.
exit /b 1
