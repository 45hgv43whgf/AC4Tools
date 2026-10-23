@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0package_release.ps1" -AsiPath "%~1"
exit /b %errorlevel%
