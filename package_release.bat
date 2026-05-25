@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "ASI_PATH=%~1"
if "%ASI_PATH%"=="" set "ASI_PATH=%PROJECT_DIR%bin\AC4Tools.asi"

set "RELEASE_DIR=%PROJECT_DIR%release"
set "RELEASE_SCRIPTS=%RELEASE_DIR%\scripts"
set "ASI_LOADER=%PROJECT_DIR%release_assets\dinput8.dll"

if not exist "%ASI_PATH%" (
 echo Error: AC4Tools ASI not found at "%ASI_PATH%".
 exit /b 1
)

if exist "%RELEASE_DIR%" rmdir /s /q "%RELEASE_DIR%"
mkdir "%RELEASE_SCRIPTS%"
copy /y "%ASI_PATH%" "%RELEASE_SCRIPTS%\AC4Tools.asi" >nul
copy /y "%PROJECT_DIR%AC4Tools.ini" "%RELEASE_SCRIPTS%\AC4Tools.ini" >nul

(
echo AC4Tools v1.01
 echo.
 echo Made/tested for AC4BFSP.exe:
 echo Size: 45,056,040 bytes
 echo Timestamp: 2023-11-14 14:41:36
 echo SHA256: 732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD
 echo.
 echo Copy these files into the Assassin's Creed IV Black Flag game folder:
 echo - dinput8.dll
 echo - scripts\AC4Tools.asi
 echo - scripts\AC4Tools.ini
 echo.
 echo Open/close menu: configured in scripts\AC4Tools.ini under [Hotkeys] MenuOpen. Default is B.
 echo Logging is off by default. Set [Logging] EnableConsole or EnableFile to 1 in scripts\AC4Tools.ini to enable diagnostics.
 echo.
 echo Unlocks tab warning:
 echo - Back up your save before using Unlocks.
 echo - Unlocks can become permanent/irreversible once the game saves.
 echo - Unlocks can irreversibly softlock your save if something was meant to unlock later through progression.
 echo - Entries marked ^(CC^) can optionally finish required community completion when that Unlocks-tab option is enabled.
 echo - Global Hidden Unlocks should be installed from the main menu before loading a save and stays active until restart.
) > "%RELEASE_DIR%\README.txt"

if exist "%ASI_LOADER%" (
 copy /y "%ASI_LOADER%" "%RELEASE_DIR%\dinput8.dll" >nul
) else (
 echo Warning: ASI Loader not found at "%ASI_LOADER%".
)
