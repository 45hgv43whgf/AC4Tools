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
echo AC4Tools v1.02
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
 echo Current feature tabs:
 echo - Ship: Ship Godmode, Ally Godmode, No Cannon Cooldown.
 echo - Player: Player Godmode, Infinite Breath, Stealth Mode, No Reload, No Fall Damage, Allow Eagle Vision while sprinting, Kill civilians without desynchronization, Lock Consumables ^(incl Ship Ammo^), One-time Inventory Refill, Freeze Mission Timer.
 echo - Noclip: configurable speed and boost speed.
 echo - Game: Time Scale and Free Cam.
 echo - Freedom Cry: add 100 Liberated Slaves or 100 Recruited Maroons to the Freedom Cry resistance counters.
 echo - Hotkeys: configurable menu, feature, and Free Cam control hotkeys.
 echo - System: logging/input/window-lock status plus patch diagnostics.
 echo.
 echo Lock Consumables ^(incl Ship Ammo^):
 echo - Enable after the save is fully loaded.
 echo - Uses one shared consume/decrement hook for supported player consumables and ship ammunition.
 echo - It prevents values from going down while enabled; it does not refill values upward.
 echo.
 echo Free Cam:
 echo - Installs camera hooks only when enabled from the Game tab or its assigned hotkey.
 echo - Mouse aims; Num8/Num2 move forward/back, Num4/Num6 strafe, Num7/Num9 up/down.
 echo - Free Cam Speed, Vertical Speed, and Look Speed are separately configurable.
 echo - Hold Shift to speed up and press F10 to exit by default.
 echo - Movement, boost, and exit keys are configurable in the Hotkeys tab under Free Cam Control Hotkeys.
 echo.
 echo One-time Inventory Refill:
 echo - Choose one resource/ammo type from the dropdown and click the dynamic Refill button.
 echo - Uses the previous preset values: Money 999999; Ship Crew 40; Mortar Shot Ammo 15; Heavy Shot Ammo 25; Fire Barrels 25; Sugar/Rum/Wood/Cloth/Metal 2500; Bullets 30; Smoke Bombs/Sleep Darts/Berserk Darts/Rope Darts/Firecrackers/Blunderbuss 15; Throwing Knives 1; Harpoons 40.
 echo - If the button is disabled, load a save and open an inventory or ship menu first.
 echo.
 echo Freedom Cry:
 echo - Adds 100 Liberated Slaves or 100 Recruited Maroons to your Freedom Cry resistance progress.
 echo - Use while loaded into Freedom Cry.
 echo - If the buttons are disabled, open an inventory or ship menu first.
 echo - Freedom Cry-specific refill dropdown entries: Firecrackers and Blunderbuss.
 echo.
 echo Unlocks tab warning:
 echo - Back up your save before using Unlocks.
 echo - Unlocks can become permanent/irreversible once the game saves.
 echo - Unlocks can irreversibly softlock your save if something was meant to unlock later through progression.
 echo - Entries marked ^(CC^) can optionally finish required community completion when that Unlocks-tab option is enabled.
 echo - Global Hidden Unlocks is a broad all-at-once alternative to the targeted unlock checkboxes. It should be installed from the main menu before loading a save, can unlock additional hidden rewards beyond the individual list, and stays active until restart.
 echo.
 echo Selectable Unlocks:
 echo - Pistols: Golden Flintlock Pistols, Captain's Wheellock Pistols ^(CC^), Precision Shooter ^(Freedom Cry^).
 echo - Swords: Pistol Swords, Scottish Broadsword, Persian Scimitars ^(CC^), Crude Iron Machete ^(Freedom Cry^), Mayan Machete ^(Freedom Cry^).
 echo - Outfits: Governor Outfit, Templar Outfit, Stealth Outfit, Explorer Outfit ^(CC^).
 echo - Ship Cosmetics: Gilded Sails, The Ranger Figurehead ^& Queen Anne's Revenge Wheel, Aquila Figurehead, El Impoluto Figurehead ^(CC^), El Impoluto Wheel ^(CC^), Black and Red Sails ^(CC^), Queen Anne Figurehead ^(CC^), Blackwood Wheel ^(CC^), Aquila Wheel ^(CC^), Flower Sails ^(CC^), Grey Sails ^(CC^).
 echo - Elite Unlocks: Elite Hull ^(CC^), Elite Set of Cannons ^(CC^), Elite Ram ^(CC^), Elite Round Shot ^(CC^), Elite Mortars ^(CC^), Elite Swivel Guns ^(CC^), Elite Heavy Shot ^(CC^), Elite Fire Barrel ^(CC^), Elite Heavy Shot Storage ^(CC^), Elite Mortar Storage ^(CC^), Elite Fire Barrel Storage ^(CC^), Elite Harpoon ^(CC^).
) > "%RELEASE_DIR%\README.txt"

if exist "%ASI_LOADER%" (
 copy /y "%ASI_LOADER%" "%RELEASE_DIR%\dinput8.dll" >nul
) else (
 echo Warning: ASI Loader not found at "%ASI_LOADER%".
)
