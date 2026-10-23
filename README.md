# AC4Tools

AC4Tools is a standalone ASI plugin for **Assassin's Creed IV Black Flag**.

It adds an in-game ImGui menu with ship, player, consumable lock, one-time inventory refills, unlocks, Freedom Cry helpers, noclip, time scale, input/system, and hotkey options. The plugin is intended to be loaded through Ultimate ASI Loader.

Current version: **v1.08**

## Game Version

Gameplay addresses were developed for this Steam single-player executable:

```text
AC4BFSP.exe
Size: 45,056,040 bytes
Timestamp: 2023-11-14 14:41:36
SHA256: 732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD
```

All gameplay features require this exact SHA-256 and the original image base (`0x00400000`). A mismatch, failed hash calculation, or relocated image disables gameplay initialization, actions, hotkeys, memory scans, and lazy patch installation before any game addresses are probed. The overlay and System diagnostics remain available. Pattern-based features use the same build gate; there is no unsupported-build override.

On the supported build, individual patch sites must also match their expected bytes. Pattern searches inspect committed executable pages and require a unique match. Unknown or unreadable bytes refuse that feature and are logged. These checks reduce address/build errors; they cannot guarantee compatibility with arbitrary third-party changes to a running game.

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
- Toggle Hood: one-shot action that raises or lowers Edward's hood based on the current state. It can be assigned a hotkey and has no default binding.
- Toggle Crouch: one-shot action that toggles Edward's crouch state. It can be assigned a hotkey and has no default binding.
- Free Cam with separate movement, vertical movement, and mouse-look speed controls
- Optional mouse lock to keep the Windows cursor inside the game window
- Optional mouse input blocking while the AC4Tools UI is open
- Optional keyboard input blocking while the AC4Tools UI is open
- Optional controller input blocking while the AC4Tools UI is open
- Optional AC4Tools hotkey blocking while the AC4Tools UI is open
- Saved menu window position and size after dragging/resizing
- Configurable keyboard and controller hotkeys for the menu, toggleable features, and Free Cam controls
- In-game UI opened with the configured menu hotkey, `B` by default
- Optional console and file logging for patch status and diagnostics
- INFO/WARN/ERROR log levels; console warnings are yellow and errors are red
- DX11 swap-chain resize/reset handling for better overlay compatibility, including ReShade setups

## Notes

`Lock Consumables (incl Ship Ammo)` should be enabled after the save is fully loaded. It does not refill values upward; it prevents the shared consume/decrement function from lowering supported consumables while enabled.

`Toggle Hood` is a one-shot action in the Game tab. It raises or lowers Edward's hood based on the current state. Its hotkey is optional and unbound by default.

`Toggle Crouch` is a one-shot action in the Game tab. It toggles Edward's crouch state directly. Its hotkey is optional and unbound by default.

`Free Cam` installs its camera hooks only when enabled from the Game tab or its assigned hotkey. Use the mouse to aim. Default keyboard controls are `Num8`/`Num2` for forward/back, `Num4`/`Num6` to strafe, `Num7`/`Num9` to move up/down, hold `Shift` to speed up, and press `F10` to exit. Default controller controls are D-pad for movement, `LB`/`RB` for up/down, `LT` for boost, and `B` to exit. Movement, boost, and exit bindings can be changed in the Hotkeys tab under `Free Cam Control Hotkeys`. `Free Cam Look Speed` scales mouse-look sensitivity while Free Cam is enabled.

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
  LOADER-PROVENANCE.json
  Ultimate-ASI-Loader-LICENSE.txt
  README.txt
  SHA256SUMS.txt
  scripts/
    AC4Tools.asi
    AC4Tools.ini
```

Copy the contents of `release/` into the game folder, next to `AC4BFSP.exe`.

The package includes the Win32/x86 `dinput8.dll` from [Ultimate ASI Loader v9.7.1](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/tag/v9.7.1), product version `9.7.1.0-2155f21`. The bundled DLL was compared with the DLL extracted from that exact upstream release. The downloaded ZIP also matched GitHub's published archive SHA-256.

`release_assets/loader-manifest.json` pins the upstream release URL, archive URL/hash, member name, and extracted DLL hash. Packaging and installation reject a missing or mismatched loader. The installer checks the source before changing game files, installs the verified loader if absent, and leaves an already matching loader untouched. If an existing loader differs, installation stops without overwriting it; back it up and resolve the difference before retrying.

```text
DLL SHA256: 7675B24A2E408349C8A59B87E3AE8724AD8A89EFF21C96A9C109650A3C1316E0
ZIP SHA256: B69853DF1099DE97B9622B9F693F2FC62269CD7774A26408974F1BCABF806FC3
```

`LOADER-PROVENANCE.json` and the upstream license accompany the package. `SHA256SUMS.txt` covers the ASI, config, README, DLL, provenance manifest, and license. Builds use the local pinned asset and never download a loader automatically.

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

The install script requires a game path argument. No local game path is hardcoded. Close the game before updating the ASI; existing INI settings are preserved and the previous INI is backed up.

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
DisableControllerInputWhenUiOpen = 0
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
MenuOpenGamepad = 0
ShipGodmode = 0
ShipGodmodeGamepad = 0
NoCannonCooldown = 0
NoCannonCooldownGamepad = 0
AllyGodmode = 0
AllyGodmodeGamepad = 0
PlayerGodmode = 0
PlayerGodmodeGamepad = 0
DesynchronizeYourself = 0
DesynchronizeYourselfGamepad = 0
InfiniteBreath = 0
InfiniteBreathGamepad = 0
StealthMode = 0
StealthModeGamepad = 0
NoReload = 0
NoReloadGamepad = 0
NoFallDamage = 0
NoFallDamageGamepad = 0
EagleVisionSprint = 0
EagleVisionSprintGamepad = 0
KillCiviliansNoDesync = 0
KillCiviliansNoDesyncGamepad = 0
ShowCollectibles = 0
ShowCollectiblesGamepad = 0
LockConsumables = 0
LockConsumablesGamepad = 0
FreezeMissionTimer = 0
FreezeMissionTimerGamepad = 0
Noclip = 0
NoclipGamepad = 0
TimeScale = 0
TimeScaleGamepad = 0
ToggleHood = 0
ToggleHoodGamepad = 0
ToggleCrouch = 0
ToggleCrouchGamepad = 0
FreeCam = 0
FreeCamGamepad = 0

[FreeCamControls]
MoveForward = 104
MoveForwardGamepad = 4101
MoveBackward = 98
MoveBackwardGamepad = 4102
StrafeLeft = 100
StrafeLeftGamepad = 4103
StrafeRight = 102
StrafeRightGamepad = 4104
MoveUp = 103
MoveUpGamepad = 4105
MoveDown = 105
MoveDownGamepad = 4106
Boost = 16
BoostGamepad = 4111
Exit = 121
ExitGamepad = 4098
```

`EnableConsole = 1` opens a console window. `EnableFile = 1` writes timestamped lines to `AC4Tools.log` in the `scripts` folder next to `AC4Tools.asi`. Both are off by default.

Logs include `[INFO]`, `[WARN]`, and `[ERROR]` levels. When console logging is enabled, warnings are yellow and errors are red. Startup diagnostics print the detected `AC4BFSP.exe` size, timestamp, and SHA256; a SHA256 mismatch disables all gameplay features.

`LockMouseToWindow = 1` confines the Windows cursor to the AC4 window while the game is focused. It releases automatically when AC4 loses focus, such as when you alt-tab. This can prevent scrolling or clicking on a second monitor during gameplay.

`DisableMouseInputWhenUiOpen = 1` blocks mouse input from reaching the game while the AC4Tools UI is open. `DisableKeyboardInputWhenUiOpen = 1` does the same for keyboard input. `DisableControllerInputWhenUiOpen = 1` blocks DirectInput and common XInput controller state from reaching the game while the UI is open. `DisableHotkeysWhileUiOpen = 1` stops AC4Tools feature hotkeys, including controller hotkeys, from firing while the UI is open, while still letting the menu open/close hotkey work. All four are off by default.

`WindowPosX`, `WindowPosY`, `WindowSizeX`, and `WindowSizeY` store the last menu position and size after you finish dragging or resizing the window.

Keyboard hotkey values are Win32 virtual-key codes. `MenuOpen = 66` is `B`. Controller hotkey values are AC4Tools controller IDs used for both XInput and DirectInput paths: `4097=A`, `4098=B`, `4099=X`, `4100=Y`, `4101-4104=D-pad Up/Down/Left/Right`, `4105=LB`, `4106=RB`, `4107=L3`, `4108=R3`, `4109=Back`, `4110=Start`, `4111=LT`, `4112=RT`, `4113-4116=Left Stick Up/Down/Left/Right`, and `4117-4120=Right Stick Up/Down/Left/Right`. Free Cam defaults are numpad movement on keyboard and D-pad/LB/RB/LT/B on controller. Use the in-game Hotkeys tab to set them instead of editing by hand.

## Third-Party Components

- Dear ImGui by Omar Cornut and contributors (v1.90.9)
- MinHook by Tsuda Kageyu (Includes HDE disassembler code by Vyacheslav Patkov.)
- Ultimate ASI Loader by ThirteenAG (verified Win32 v9.7.1, MIT license included)

## Compatibility

AC4Tools is designed to run standalone. If another plugin or external tool already owns the same hook address, AC4Tools refuses that feature cleanly instead of trying to co-load with it.

## Runtime and shutdown

C++ entry points from rendering, window messages, input hooks, game callbacks, and the worker serialize shared state through a recursive lock. WndProc uses a try-lock to avoid deadlocking synchronous window messages. Generated x86 caves communicate through aligned four-byte cells with Interlocked access from C++; hit counters use locked increments. Patch writes pause and inspect peer threads, recheck expected bytes before the first write, and refuse to replace instructions that are currently in progress. Hooks are queued during discovery and enabled only after their original-function pointers are initialized.

XInput support hooks only variants already loaded by the process at initialization. There is no static XInput import and no forced loading of XInput DLLs. Existing variants used for hooks are pinned with the ASI.

The ASI is pinned for the process lifetime before its worker starts. **Hot unload/reload is not supported; restart the game to replace the plugin.** Pinning prevents a loader's FreeLibrary call from invalidating installed callbacks. Tools can request a controlled stop through the Win32 `__stdcall` exports `AC4ToolsRequestShutdown` and poll `AC4ToolsIsStopped` (x86 decorated names `_AC4ToolsRequestShutdown@0` and `_AC4ToolsIsStopped@0`). Call these outside DllMain. Shutdown is asynchronous and may wait for a running unlock scan to finish.

Stopping releases cursor/input capture, attempts to disable noclip, clears native feature flags, disables owned MinHook hooks, restores tracked patches only while their bytes still belong to AC4Tools, restores the window procedure when still owned, and releases ImGui, D3D, controller, console, and unpublished cave resources. Published caves and hook trampolines stay mapped for in-flight return paths; the OS reclaims them at exit. Game-owned community allocations and save/inventory/unlock changes are not rolled back. A stopped ASI remains mapped and cannot be restarted in the same session. Diagnostics report incomplete restoration; restart the game before loading a different version.

## Source layout and verification

`AC4Tools.cpp` assembles private feature fragments under `game_addresses/`, `patches/`, `input/`, `overlay/`, `config/`, `unlock/`, and `runtime/`. The `.inl` files intentionally retain one translation unit and internal linkage for the x86 emitters. Safety helpers for build policy, scanning, numeric parsing, state locking, hook management, and patch ownership have separate headers. Changes to the supported build should start in `game_addresses/SupportedBuild.h`; per-site expected bytes remain mandatory.

Run `test.bat` with Visual Studio C++ tools installed. It builds Release Win32 and runs an isolated executable named `AC4BFSP.exe` whose hash is intentionally unsupported. Tests cover build gates and direct actions, the compiled ASI lifecycle in an unsupported process, guarded/ambiguous pattern scans, malformed/nonfinite numeric input and INI files, overflow boundaries, refusing patches inside an executing instruction, patch ownership and restoration, actual MinHook enable/disable behavior, shutdown and cave lifetime, and packaging/install behavior in a temporary fixture, including rejecting missing/corrupt loaders and preserving a matching locked loader. These checks do not replace in-game testing of supported-build gameplay, controller input, resize/reset, and overlay compatibility.
