# AC4Tools

AC4Tools is a standalone ASI plugin for **Assassin's Creed IV Black Flag**.

It adds an in-game ImGui menu with ship, player, inventory/resources, unlocks, noclip, time scale, input/system, and hotkey options. The plugin is intended to be loaded through Ultimate ASI Loader.

Current version: **v1.01**

## Game Version

AC4Tools was made and tested for the newest(at time of creation) Steam single-player executable:

```text
AC4BFSP.exe
Size: 45,056,040 bytes
Timestamp: 2023-11-14 14:41:36
SHA256: 732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD
```

Other game builds may use different code addresses or bytes. If a supported patch check does not match, or another tool already owns that address, AC4Tools refuses that feature instead of patching unknown code. For byte mismatches, it logs the expected and found bytes.

## Features

### Ship

- Ship Godmode
- Infinite Ship Crew: keeps ship crew at `40`
- Infinite Mortar Shot Ammo: keeps mortar shot ammo at `15`
- Infinite Heavy Shot Ammo: keeps heavy shot ammo at `25`
- Infinite Fire Barrels: keeps fire barrels at `25`
- Ally Godmode: protects all allies, not just allied ships
- No Cannon Cooldown

### Player

- Player Godmode
- Infinite Breath
- Stealth Mode
- No Reload
- Freeze Mission Timer

### Money and Inventory

- Infinite Money: refills to `999999`
- Infinite Sugar: refills to `2500`
- Infinite Rum: refills to `2500`
- Infinite Wood: refills to `2500`
- Infinite Cloth: refills to `2500`
- Infinite Metal: refills to `2500`
- Infinite Bullets: refills to `30`
- Infinite Smokebombs: refills to `15`
- Infinite Sleep Darts: refills to `15`
- Infinite Berserk Darts: refills to `15`
- Infinite Rope Darts: refills to `15`
- Infinite Throwing Knives: refills to `1`
- Infinite Harpoons: refills to `40`

### Unlocks

The Unlocks tab can temporarily unlock selected loaded records. If the game saves afterward, these unlocks can become permanent in that save.

Back up your save before using Unlocks. Saved unlock changes may be irreversible.

Unlocks can irreversibly softlock your save if an unlocked item was supposed to be granted later by a mission, contract, challenge, or other progression event.

- Global Hidden Unlocks: install from the main menu before loading a save; stays active until restart
- Finish Community Challenges if needed for certain unlocks: optional community-completion handling for entries marked `(CC)`
- Pistols: Golden Flintlock Pistols, Captain's Wheellock Pistols `(CC)`, Precision Shooter
- Swords: Pistol Swords, Scottish Broadsword, Persian Scimitars `(CC)`, Crude Iron Machete, Mayan Machete
- Outfits: Governor Outfit, Templar Outfit, Stealth Outfit, Explorer Outfit `(CC)`
- Ship Cosmetics: Gilded Sails, The Ranger Figurehead & Queen Anne's Revenge Wheel, Aquila Figurehead, plus selected `(CC)` sails/figureheads/wheels
- Elite Unlocks: elite Jackdaw upgrades marked `(CC)`

### Tools

- Noclip with configurable speed and boost speed
- Time Scale with configurable value
- Optional mouse lock to keep the Windows cursor inside the game window
- Optional mouse input blocking while the AC4Tools UI is open
- Optional keyboard input blocking while the AC4Tools UI is open
- Optional AC4Tools hotkey blocking while the AC4Tools UI is open
- Saved menu window position and size after dragging/resizing
- Configurable hotkeys for the menu and toggleable features
- In-game UI opened with the configured menu hotkey, `B` by default
- Optional console and file logging for patch status and diagnostics

## Notes

`Ship Godmode`, `Infinite Ship Crew`, `Infinite Mortar Shot Ammo`, `Infinite Heavy Shot Ammo`, and `Infinite Fire Barrels` may need you to leave the wheel and take control of the ship again before they start applying.

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

[Noclip]
Speed = 1.000000
BoostSpeed = 5.000000

[Hotkeys]
MenuOpen = 66
ShipGodmode = 0
NoCannonCooldown = 0
AllyGodmode = 0
InfiniteShipCrew = 0
InfiniteMortarShotAmmo = 0
InfiniteHeavyShotAmmo = 0
InfiniteFireBarrels = 0
PlayerGodmode = 0
InfiniteBreath = 0
StealthMode = 0
NoReload = 0
FreezeMissionTimer = 0
InfiniteMoney = 0
InfiniteSugar = 0
InfiniteRum = 0
InfiniteBullets = 0
InfiniteRopeDarts = 0
InfiniteWood = 0
InfiniteSleepDarts = 0
InfiniteThrowingKnives = 0
InfiniteMetal = 0
InfiniteSmokebombs = 0
InfiniteBerserkDarts = 0
InfiniteHarpoons = 0
InfiniteCloth = 0
Noclip = 0
TimeScale = 0
```

`EnableConsole = 1` opens a console window. `EnableFile = 1` writes timestamped lines to `AC4Tools.log` next to `AC4BFSP.exe`. Both are off by default.

`LockMouseToWindow = 1` confines the Windows cursor to the AC4 window while the game is focused. It releases automatically when AC4 loses focus, such as when you alt-tab. This can prevent scrolling or clicking on a second monitor during gameplay.

`DisableMouseInputWhenUiOpen = 1` blocks mouse input from reaching the game while the AC4Tools UI is open. `DisableKeyboardInputWhenUiOpen = 1` does the same for keyboard input. `DisableHotkeysWhileUiOpen = 1` stops AC4Tools feature hotkeys from firing while the UI is open, while still letting the menu open/close hotkey work. All three are off by default.

`WindowPosX`, `WindowPosY`, `WindowSizeX`, and `WindowSizeY` store the last menu position and size after you finish dragging or resizing the window.

Hotkey values are Win32 virtual-key codes. `MenuOpen = 66` is `B`. Use the in-game Hotkeys tab to set them instead of editing by hand.

## Third-Party Components

- Dear ImGui by Omar Cornut and contributors (v1.90.9)
- MinHook by Tsuda Kageyu (Includes HDE disassembler code by Vyacheslav Patkov.)
- Ultimate ASI Loader by ThirteenAG (9.7.1.0-2155f21)

## Compatibility

AC4Tools is designed to run standalone. If another plugin or external tool already owns the same hook address, AC4Tools refuses that feature cleanly instead of trying to co-load with it.
