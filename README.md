# AC4Tools

AC4Tools is a standalone ASI plugin for **Assassin's Creed IV Black Flag**.

It adds an in-game ImGui menu with ship, player, consumable lock, one-time inventory refills, unlocks, Freedom Cry helpers, noclip, time scale, input/system, and hotkey options. The plugin is intended to be loaded through Ultimate ASI Loader.

Current version: **v1.05**

## Game Version

AC4Tools was made and tested for the newest(at time of creation) Steam single-player executable:

```text
AC4BFSP.exe
Size: 45,056,040 bytes
Timestamp: 2023-11-14 14:41:36
SHA256: 732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD
```

Other game builds may use different code addresses or bytes. If a supported patch check does not match, or another tool already owns that address, AC4Tools refuses that feature instead of patching unknown code. For byte mismatches, it logs the expected and found bytes. Startup diagnostics also log the detected executable name, size, timestamp, and SHA256, and warn when the SHA256 does not match the supported build above.

## Features

### Ship

- Ship Godmode
- Ally Godmode: protects all allies, not just allied ships
- No Cannon Cooldown

### Player

- Player Godmode
- Desynchronize yourself: one-shot action to trigger player desynchronization
- Infinite Breath
- Stealth Mode
- No Reload
- No Fall Damage
- Allow Eagle Vision while sprinting
- Kill civilians without desynchronization: also suppresses the related on-screen warning while enabled
- Show collectibles
- Lock Consumables (incl Ship Ammo): prevents the shared consumable decrement from lowering values while enabled. This covers player consumables and ship ammunition through one shared hook. Enable it after the save is fully loaded.
- One-time Inventory Refill: choose one resource/ammo type from a dropdown and refill it once to its preset or a custom amount. Optional setter mode writes the exact amount instead of only raising lower values.
- Freeze Mission Timer

### One-Time Refill Presets

- Money: `999999`
- Ship Crew: `40`
- Mortar Shot Ammo: `15`
- Heavy Shot Ammo: `25`
- Fire Barrels: `25`
- Sugar, Rum, Wood, Cloth, Metal: `2500`
- Bullets: `30`
- Smoke Bombs, Sleep Darts, Berserk Darts, Rope Darts, Firecrackers, Blunderbuss: `15`
- Throwing Knives: `1`
- Harpoons: `40`

### Unlocks

The Unlocks tab can temporarily unlock selected loaded records. If the game saves afterward, these unlocks can become permanent in that save.

Back up your save before using Unlocks. Saved unlock changes may be irreversible.

Unlocks can irreversibly softlock your save if an unlocked item was supposed to be granted later by a mission, contract, challenge, or other progression event.

- Global Hidden Unlocks: broad all-at-once alternative to the targeted unlock checkboxes. Install from the main menu before loading a save; it can unlock additional hidden rewards beyond the individually selectable list and stays active until restart.
- Finish Community Challenges if needed for certain unlocks: optional community-completion handling for entries marked `(CC)`
- Pistols: Golden Flintlock Pistols, Captain's Wheellock Pistols `(CC)`, Precision Shooter `(Freedom Cry)`
- Swords: Pistol Swords, Scottish Broadsword, Persian Scimitars `(CC)`, Crude Iron Machete `(Freedom Cry)`, Mayan Machete `(Freedom Cry)`
- Outfits: Governor Outfit, Templar Outfit, Stealth Outfit, Explorer Outfit `(CC)`
- Ship Cosmetics: Gilded Sails, The Ranger Figurehead & Queen Anne's Revenge Wheel, Aquila Figurehead, El Impoluto Figurehead `(CC)`, El Impoluto Wheel `(CC)`, Black and Red Sails `(CC)`, Queen Anne Figurehead `(CC)`, Blackwood Wheel `(CC)`, Aquila Wheel `(CC)`, Flower Sails `(CC)`, Grey Sails `(CC)`
- Elite Unlocks: Elite Hull `(CC)`, Elite Set of Cannons `(CC)`, Elite Ram `(CC)`, Elite Round Shot `(CC)`, Elite Mortars `(CC)`, Elite Swivel Guns `(CC)`, Elite Heavy Shot `(CC)`, Elite Fire Barrel `(CC)`, Elite Heavy Shot Storage `(CC)`, Elite Mortar Storage `(CC)`, Elite Fire Barrel Storage `(CC)`, Elite Harpoon `(CC)`

### Freedom Cry

- Add `100` Liberated Slaves to the Freedom Cry resistance counter
- Add `100` Recruited Maroons to the Freedom Cry resistance counter
- Use these while loaded into Freedom Cry

### Tools

- Noclip with configurable speed and boost speed
- Time Scale with configurable value
- Free Cam with separate movement, vertical movement, and mouse-look speed controls
- Optional mouse lock to keep the Windows cursor inside the game window
- Optional mouse input blocking while the AC4Tools UI is open
- Optional keyboard input blocking while the AC4Tools UI is open
- Optional AC4Tools hotkey blocking while the AC4Tools UI is open
- Saved menu window position and size after dragging/resizing
- Configurable hotkeys for the menu, toggleable features, and Free Cam controls
- In-game UI opened with the configured menu hotkey, `B` by default
- Optional console and file logging for patch status and diagnostics
- INFO/WARN/ERROR log levels; console warnings are yellow and errors are red
- DX11 swap-chain resize/reset handling for better overlay compatibility, including ReShade setups

## Notes

`Lock Consumables (incl Ship Ammo)` should be enabled after the save is fully loaded. It does not refill values upward; it prevents the shared consume/decrement function from lowering supported consumables while enabled.

`Free Cam` installs its camera hooks only when enabled from the Game tab or its assigned hotkey. Use the mouse to aim. Default controls are `Num8`/`Num2` for forward/back, `Num4`/`Num6` to strafe, `Num7`/`Num9` to move up/down, hold `Shift` to speed up, and press `F10` to exit. Movement, boost, and exit keys can be changed in the Hotkeys tab under `Free Cam Control Hotkeys`. `Free Cam Look Speed` scales mouse-look sensitivity while Free Cam is enabled.

If the `One-time Inventory Refill` buttons are disabled, load a save and open an inventory or ship menu first.

If the `Freedom Cry` buttons are disabled, load Freedom Cry and open an inventory or ship menu first.

Freedom Cry-specific refill dropdown entries include Firecrackers and Blunderbuss.

`Ally Godmode` affects all allies, not just allied ships, even though it is grouped under the Ship tab in the UI.

Most gameplay feature on/off states are session-only and are not saved. Unlocks are different: if the game saves after an unlock is applied, that unlock can persist in the save. The INI saves numeric settings, hotkey bindings, System-tab options, and the menu window position/size.

## Release Layout

Building the project creates a ready-to-copy folder:

```text
release/
  dinput8.dll
  README.txt
  scripts/
    AC4Tools.asi
    AC4Tools.ini
```

Copy the contents of `release/` into the game folder, next to `AC4BFSP.exe`.

## Building

### Visual Studio

Open:

```text
AC4Tools.sln
```

Build `Release|Win32`. The project targets Visual Studio 2022 toolset `v143`.

The build output is written to:

```text
bin/AC4Tools.asi
```

The post-build step refreshes the local `release/` folder. It does not copy anything into your game directory.

### Command Line

Run:

```bat
build.bat
```

The script locates Visual Studio C++ tools through `vswhere`, builds the ASI, and refreshes `release/`.

## Optional Install Script

To install from the prepared release folder:

```powershell
.\install_to_game.ps1 -GameDir "C:\Path\To\Assassin's Creed IV Black Flag"
```

The install script requires a game path argument. No local game path is hardcoded.

## Configuration

Default config:

```ini
[Logging]
EnableConsole = 0
EnableFile = 0

[System]
LockMouseToWindow = 0
DisableMouseInputWhenUiOpen = 0
DisableKeyboardInputWhenUiOpen = 0
DisableHotkeysWhileUiOpen = 0

[UI]
WindowPosX = 60
WindowPosY = 40
WindowSizeX = 680
WindowSizeY = 420

[Game]
TimeScale = 0.010000
FreeCamSpeed = 1.000000
FreeCamVerticalSpeed = 1.000000
FreeCamLookSpeed = 1.000000

[Noclip]
Speed = 1.000000
BoostSpeed = 5.000000

[Hotkeys]
MenuOpen = 66
ShipGodmode = 0
NoCannonCooldown = 0
AllyGodmode = 0
PlayerGodmode = 0
InfiniteBreath = 0
StealthMode = 0
NoReload = 0
NoFallDamage = 0
EagleVisionSprint = 0
KillCiviliansNoDesync = 0
ShowCollectibles = 0
LockConsumables = 0
FreezeMissionTimer = 0
Noclip = 0
TimeScale = 0
FreeCam = 0

[FreeCamControls]
MoveForward = 104
MoveBackward = 98
StrafeLeft = 100
StrafeRight = 102
MoveUp = 103
MoveDown = 105
Boost = 16
Exit = 121
```

`EnableConsole = 1` opens a console window. `EnableFile = 1` writes timestamped lines to `AC4Tools.log` next to `AC4BFSP.exe`. Both are off by default.

Logs include `[INFO]`, `[WARN]`, and `[ERROR]` levels. When console logging is enabled, warnings are yellow and errors are red. Startup diagnostics print the detected `AC4BFSP.exe` size, timestamp, and SHA256; a SHA256 mismatch is a warning that the game executable may not be the fully supported build.

`LockMouseToWindow = 1` confines the Windows cursor to the AC4 window while the game is focused. It releases automatically when AC4 loses focus, such as when you alt-tab. This can prevent scrolling or clicking on a second monitor during gameplay.

`DisableMouseInputWhenUiOpen = 1` blocks mouse input from reaching the game while the AC4Tools UI is open. `DisableKeyboardInputWhenUiOpen = 1` does the same for keyboard input. `DisableHotkeysWhileUiOpen = 1` stops AC4Tools feature hotkeys from firing while the UI is open, while still letting the menu open/close hotkey work. All three are off by default.

`WindowPosX`, `WindowPosY`, `WindowSizeX`, and `WindowSizeY` store the last menu position and size after you finish dragging or resizing the window.

Hotkey values are Win32 virtual-key codes. `MenuOpen = 66` is `B`. Free Cam control defaults are numpad movement, `Boost = 16` for `Shift`, and `Exit = 121` for `F10`. Use the in-game Hotkeys tab to set them instead of editing by hand.

## Third-Party Components

- Dear ImGui by Omar Cornut and contributors (v1.90.9)
- MinHook by Tsuda Kageyu (Includes HDE disassembler code by Vyacheslav Patkov.)
- Ultimate ASI Loader by ThirteenAG (9.7.1.0-2155f21)

## Compatibility

AC4Tools is designed to run standalone. If another plugin or external tool already owns the same hook address, AC4Tools refuses that feature cleanly instead of trying to co-load with it.
