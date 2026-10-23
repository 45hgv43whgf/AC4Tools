@echo off
setlocal
set "PROJECT_DIR=%~dp0"
call "%PROJECT_DIR%tools\setup_x86.bat"
if errorlevel 1 exit /b %errorlevel%
if "%~1"=="--no-build" goto Test
call "%PROJECT_DIR%build.bat"
if errorlevel 1 exit /b %errorlevel%
:Test
if not exist "%PROJECT_DIR%obj\tests" mkdir "%PROJECT_DIR%obj\tests"
cl /nologo /O2 /EHsc /MT /W4 ^
 /I"%PROJECT_DIR%third_party\imgui" /I"%PROJECT_DIR%third_party\minhook\include" ^
 /Fo"%PROJECT_DIR%obj\tests\\" /Fe:"%PROJECT_DIR%obj\tests\AC4BFSP.exe" ^
 "%PROJECT_DIR%tests\native.cpp" ^
 "%PROJECT_DIR%obj\imgui.obj" "%PROJECT_DIR%obj\imgui_draw.obj" ^
 "%PROJECT_DIR%obj\imgui_tables.obj" "%PROJECT_DIR%obj\imgui_widgets.obj" ^
 "%PROJECT_DIR%obj\imgui_impl_dx11.obj" "%PROJECT_DIR%obj\imgui_impl_win32.obj" ^
 "%PROJECT_DIR%obj\buffer.obj" "%PROJECT_DIR%obj\hook.obj" ^
 "%PROJECT_DIR%obj\trampoline.obj" "%PROJECT_DIR%obj\hde32.obj" ^
 /link /NOLOGO user32.lib gdi32.lib d3d11.lib dxgi.lib dinput8.lib dxguid.lib advapi32.lib
if errorlevel 1 exit /b %errorlevel%
"%PROJECT_DIR%obj\tests\AC4BFSP.exe"
if errorlevel 1 exit /b %errorlevel%
if not exist "%PROJECT_DIR%obj\tests\smoke" mkdir "%PROJECT_DIR%obj\tests\smoke"
cl /nologo /O2 /EHsc /MT /W4 /Fo"%PROJECT_DIR%obj\tests\smoke\\" ^
 /Fe:"%PROJECT_DIR%obj\tests\smoke\AC4BFSP.exe" "%PROJECT_DIR%tests\dll_smoke.cpp" /link /NOLOGO
if errorlevel 1 exit /b %errorlevel%
"%PROJECT_DIR%obj\tests\smoke\AC4BFSP.exe" "%PROJECT_DIR%bin\AC4Tools.asi"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%PROJECT_DIR%tests\release.ps1"
exit /b %errorlevel%
