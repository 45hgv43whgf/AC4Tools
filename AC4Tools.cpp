#include <windows.h>
#include <d3d11.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include <dxgi.h>

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

#include "MinHook.h"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")

extern "C" IMAGE_DOS_HEADER __ImageBase;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace {

constexpr const char* kToolName = "AC4Tools";
constexpr const char* kToolVersion = "v1.01";
constexpr const char* kToolTitle = "AC4Tools v1.01";
constexpr const char* kSupportedGameExe = "AC4BFSP.exe";
constexpr const char* kSupportedGameSize = "45,056,040 bytes";
constexpr const char* kSupportedGameTimestamp = "2023-11-14 14:41:36";
constexpr const char* kSupportedGameSha256 =
    "732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD";

constexpr std::uintptr_t kPatchHealthAddress = 0x01225C3F;
constexpr std::uintptr_t kPatchHealthReturnAddress = 0x01225C47;
constexpr std::uint8_t kOriginalHealthBytes[] = {
    0xF3, 0x0F, 0x11, 0x86, 0xD0, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchNoCannonCooldownAddress = 0x01C97031;
constexpr std::uint8_t kOriginalNoCannonCooldownByte = 0x77;
constexpr std::uint8_t kEnabledNoCannonCooldownByte = 0xEB;

constexpr std::uintptr_t kPatchStealthModeAddress = 0x0101CB66;
constexpr std::uint8_t kOriginalStealthModeByte = 0x75;
constexpr std::uint8_t kEnabledStealthModeByte = 0xEB;

constexpr std::uintptr_t kPatchAllyGodmodeAddress = 0x01BFF520;
constexpr std::uint8_t kOriginalAllyGodmodeBytes[] = {
    0x0F, 0xBF, 0x41, 0x5C, 0xC3,
};

constexpr std::uintptr_t kPatchTimeIntervalAddress = 0x00A047EA;
constexpr std::uintptr_t kPatchTimeIntervalReturnAddress = 0x00A047F0;
constexpr std::uint8_t kOriginalTimeIntervalBytes[] = {
    0x8B, 0x15, 0x68, 0x3E, 0xDD, 0x04,
};

constexpr std::uintptr_t kPatchPlayerHealthAddress = 0x0115AD07;
constexpr std::uintptr_t kPatchPlayerHealthReturnAddress = 0x0115AD0D;
constexpr std::uint8_t kOriginalPlayerHealthBytes[] = {
    0x66, 0x89, 0x46, 0x5C, 0x85, 0xFF,
};

constexpr std::uintptr_t kPatchInfiniteBreathAddress = 0x01148936;
constexpr std::uintptr_t kPatchInfiniteBreathReturnAddress = 0x0114893C;
constexpr std::uint8_t kOriginalInfiniteBreathBytes[] = {
    0xD9, 0x80, 0x80, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchNoReloadAddress = 0x016B8A96;
constexpr std::uintptr_t kPatchNoReloadReturnAddress = 0x016B8A9C;
constexpr std::uint8_t kOriginalNoReloadBytes[] = {
    0x8B, 0x50, 0x10, 0x8B, 0x48, 0x08,
};

constexpr std::uintptr_t kPatchMissionTimerAddress = 0x019446C3;
constexpr std::uintptr_t kPatchMissionTimerReturnAddress = 0x019446CB;
constexpr std::uint8_t kOriginalMissionTimerBytes[] = {
    0x51, 0x56, 0x8B, 0xF1, 0x80, 0x7E, 0x58, 0x00,
};

constexpr std::uintptr_t kPatchMissionTimer2Address = 0x016869B7;
constexpr std::uintptr_t kPatchMissionTimer2ReturnAddress = 0x016869BD;
constexpr std::uint8_t kOriginalMissionTimer2Bytes[] = {
    0xD9, 0x9E, 0xEC, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchInventoryPointerAddress = 0x01CFD381;
constexpr std::uintptr_t kPatchInventoryPointerReturnAddress = 0x01CFD387;
constexpr std::uint8_t kOriginalInventoryPointerBytes[] = {
    0x8B, 0x82, 0xE8, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchInventorySetAddress = 0x011A1F3D;
constexpr std::uintptr_t kPatchInventorySetReturnAddress = 0x011A1F42;
constexpr std::uint8_t kOriginalInventorySetBytes[] = {
    0x89, 0x4E, 0x0C, 0x5E, 0x5D,
};

constexpr std::uintptr_t kPatchInventorySetAltAddress = 0x011A1F6F;
constexpr std::uintptr_t kPatchInventorySetAltReturnAddress = 0x011A1F74;
constexpr std::uint8_t kOriginalInventorySetAltBytes[] = {
    0x89, 0x41, 0x0C, 0xB0, 0x01,
};

constexpr std::uintptr_t kPatchInventoryEntrySubtractAddress = 0x011A1FA3;
constexpr std::uintptr_t kPatchInventoryEntrySubtractReturnAddress = 0x011A1FA8;
constexpr std::uint8_t kOriginalInventoryEntrySubtractBytes[] = {
    0x89, 0x46, 0x0C, 0xB0, 0x01,
};

constexpr std::uintptr_t kPatchNoclipUpdateAddress = 0x016FAACD;
constexpr std::uintptr_t kPatchNoclipUpdateReturnAddress = 0x016FAAD3;
constexpr std::uint8_t kOriginalNoclipUpdateBytes[] = {
    0x8B, 0x8E, 0x08, 0x01, 0x00, 0x00,
};

constexpr std::uint8_t kGlobalHiddenUnlockWrite1Pattern[] = {
    0xCC, 0x8A, 0x41, 0x3D, 0x8B, 0x51, 0x24, 0x88, 0x41,
};
constexpr std::uint8_t kGlobalHiddenUnlockWrite2Pattern[] = {
    0x53, 0x56, 0x8B, 0xF1, 0x57, 0x88, 0x46, 0x38, 0xE8,
};
constexpr std::uint8_t kGlobalHiddenUnlockVisibilityPattern[] = {
    0x32, 0xC0, 0x5D, 0xC2, 0x04, 0x00, 0x8A, 0x40, 0x25, 0x5D, 0xC2,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockWrite1Bytes[] = {
    0x88, 0x41, 0x38, 0x89, 0x51, 0x0C,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockWrite2Bytes[] = {
    0x8B, 0xF1, 0x57, 0x88, 0x46, 0x38,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockVisibilityBytes[] = {
    0x8A, 0x40, 0x25,
};
constexpr std::uint8_t kEnabledGlobalHiddenUnlockVisibilityBytes[] = {
    0xB0, 0x01, 0x90,
};

bool g_enabled = false;
bool g_noCannonCooldown = false;
bool g_allyGodmode = false;
bool g_playerGodmode = false;
bool g_infiniteBreath = false;
bool g_stealthMode = false;
bool g_noReload = false;
bool g_freezeMissionTimer = false;
bool g_infiniteMoney = false;
bool g_infiniteShipCrew = false;
bool g_infiniteMortarShotAmmo = false;
bool g_infiniteHeavyShotAmmo = false;
bool g_infiniteFireBarrels = false;
bool g_infiniteSugar = false;
bool g_infiniteRum = false;
bool g_infiniteWood = false;
bool g_infiniteMetal = false;
bool g_infiniteCloth = false;
bool g_infiniteSmokeBombs = false;
bool g_infiniteBullets = false;
bool g_infiniteSleepDarts = false;
bool g_infiniteBerserkDarts = false;
bool g_infiniteRopeDarts = false;
bool g_infiniteHarpoons = false;
bool g_infiniteThrowingKnives = false;
bool g_timeScaleEnabled = false;
bool g_globalHiddenUnlockInstalled = false;
bool g_finishCommunityChallengesForUnlocks = false;
float g_timeScale = 0.01f;
int g_timeScaleInterval = 10000;
bool g_noclipEnabled = false;
bool g_noclipActive = false;
float g_noclipSpeed = 1.0f;
float g_noclipBoostSpeed = 5.0f;
bool g_installed = false;
bool g_consoleLoggingEnabled = false;
bool g_fileLoggingEnabled = false;
bool g_lockMouseToWindow = false;
bool g_disableMouseInputWhenUiOpen = false;
bool g_disableKeyboardInputWhenUiOpen = false;
bool g_disableHotkeysWhileUiOpen = false;
volatile LONG g_timeIntervalHits = 0;
volatile LONG g_playerHealthHits = 0;
volatile LONG g_infiniteBreathHits = 0;
volatile LONG g_noReloadHits = 0;
volatile LONG g_inventorySetHits = 0;
volatile LONG g_inventorySetAltHits = 0;
volatile LONG g_inventoryEntrySubtractHits = 0;
volatile LONG g_inventoryPointerHits = 0;
volatile LONG g_missionTimerHits = 0;
volatile LONG g_missionTimer2Hits = 0;
volatile LONG g_noclipUpdateHits = 0;
std::uint8_t* g_cave = nullptr;
std::uint8_t* g_allyGodmodeCave = nullptr;
std::uint8_t* g_timeIntervalCave = nullptr;
std::uint8_t* g_playerHealthCave = nullptr;
std::uint8_t* g_infiniteBreathCave = nullptr;
std::uint8_t* g_noReloadCave = nullptr;
std::uint8_t* g_inventoryPointerCave = nullptr;
std::uint8_t* g_missionTimerCave = nullptr;
std::uint8_t* g_missionTimer2Cave = nullptr;
std::uint8_t* g_inventorySetCave = nullptr;
std::uint8_t* g_inventorySetAltCave = nullptr;
std::uint8_t* g_inventoryEntrySubtractCave = nullptr;
std::uint8_t* g_noclipUpdateCave = nullptr;
std::uint8_t* g_globalHiddenUnlockWrite1Address = nullptr;
std::uint8_t* g_globalHiddenUnlockWrite2Address = nullptr;
std::uint8_t* g_globalHiddenUnlockVisibilityAddress = nullptr;
std::uint8_t* g_globalHiddenUnlockCave = nullptr;
std::uint8_t* g_communityChallengeStorage = nullptr;
std::uint8_t* g_communityChallengeNext = nullptr;
int g_inventoryItemId = 0;
int g_inventoryValue = 0;
int g_inventoryLastItemId = 0;
int g_inventoryLastOriginalValue = 0;
int g_inventoryLastAppliedValue = 0;
std::uintptr_t g_inventoryBase = 0;
std::uintptr_t g_bhvAssassin = 0;
std::uintptr_t g_playerEntity = 0;
int g_inventoryPointerLastWrites = 0;
int g_unlockPistolsFound = 0;
int g_unlockPistolsPatched = 0;
DWORD g_unlockPistolsLastScan = 0;
volatile LONG g_unlockScanRequested = 0;
volatile LONG g_unlockScanRunning = 0;
int g_missionTimerDiffer = 0;
int g_missionTimer2Differ = 0;
bool g_shipPatchReady = false;
bool g_noCannonCooldownPatchReady = false;
bool g_allyGodmodePatchReady = false;
bool g_timeScalePatchReady = false;
bool g_playerHealthPatchReady = false;
bool g_infiniteBreathPatchReady = false;
bool g_stealthModePatchReady = false;
bool g_noReloadPatchReady = false;
bool g_missionTimerPatchReady = false;
bool g_missionTimer2PatchReady = false;
bool g_missionTimersPatchReady = false;
bool g_inventoryPatchReady = false;
bool g_inventoryPointerPatchReady = false;
bool g_inventorySetPatchReady = false;
bool g_inventorySetAltPatchReady = false;
bool g_inventoryEntrySubtractPatchReady = false;
bool g_noclipPatchReady = false;
constexpr int kMenuHotkeyCapture = -2;
int g_menuHotkey = 'B';
int g_hotkeyCaptureAction = -1;
int g_suppressedHotkeyVk = 0;
char g_moduleDir[MAX_PATH]{};
char g_gameExePath[MAX_PATH]{};
char g_gameExeName[64]{};
char g_gameExeSizeText[48]{"unknown"};
char g_gameExeTimestampText[64]{"unknown"};
bool g_menuOpen = false;
bool g_inputCaptured = false;
bool g_imguiReady = false;
bool g_draggingWindow = false;
bool g_menuPosDirty = false;
bool g_menuSizeDirty = false;
ImVec2 g_menuPos = ImVec2(60.0f, 40.0f);
ImVec2 g_menuSize = ImVec2(680.0f, 420.0f);
HWND g_gameWindow = nullptr;
WNDPROC g_originalWndProc = nullptr;
ID3D11Device* g_device = nullptr;
ID3D11DeviceContext* g_context = nullptr;
ID3D11RenderTargetView* g_renderTarget = nullptr;
FILE* g_consoleOut = nullptr;
HANDLE g_consoleMutex = nullptr;
volatile LONG g_mainStarted = 0;
bool g_consoleReady = false;
char g_pendingConsoleLines[96][192]{};
int g_pendingConsoleLineCount = 0;
int g_pendingConsoleLineStart = 0;

using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
PresentFn g_originalPresent = nullptr;

using GetDeviceStateFn = HRESULT(__stdcall*)(IDirectInputDevice8A*, DWORD, LPVOID);
using GetDeviceDataFn = HRESULT(__stdcall*)(IDirectInputDevice8A*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
using GetAsyncKeyStateFn = SHORT(WINAPI*)(int);
using GetKeyStateFn = SHORT(WINAPI*)(int);
using GetKeyboardStateFn = BOOL(WINAPI*)(PBYTE);
GetDeviceStateFn g_originalGetDeviceStateMouse = nullptr;
GetDeviceStateFn g_originalGetDeviceStateKeyboard = nullptr;
GetDeviceDataFn g_originalGetDeviceDataMouse = nullptr;
GetDeviceDataFn g_originalGetDeviceDataKeyboard = nullptr;
GetAsyncKeyStateFn g_originalGetAsyncKeyState = nullptr;
GetKeyStateFn g_originalGetKeyState = nullptr;
GetKeyboardStateFn g_originalGetKeyboardState = nullptr;

void InitConsole();
void __stdcall UpdateNoclipState();
void UpdateTimeScaleInterval();
void ApplyNoCannonCooldownPatch();
void ApplyStealthModePatch();

struct ToggleAction {
    const char* id;
    const char* label;
    bool* value;
    bool* ready;
    int hotkey;
    void (*afterToggle)();
};

void AfterNoclipToggle() {
    UpdateNoclipState();
}

void AfterTimeScaleToggle() {
    UpdateTimeScaleInterval();
}

void AfterNoCannonCooldownToggle() {
    ApplyNoCannonCooldownPatch();
}

void AfterStealthModeToggle() {
    ApplyStealthModePatch();
}

ToggleAction g_actions[] = {
    {"ShipGodmode", "Ship Godmode", &g_enabled, &g_shipPatchReady, 0, nullptr},
    {"NoCannonCooldown", "No Cannon Cooldown", &g_noCannonCooldown, &g_noCannonCooldownPatchReady, 0, &AfterNoCannonCooldownToggle},
    {"AllyGodmode", "Ally Godmode", &g_allyGodmode, &g_allyGodmodePatchReady, 0, nullptr},
    {"InfiniteShipCrew", "Infinite Ship Crew", &g_infiniteShipCrew, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteMortarShotAmmo", "Infinite Mortar Shot Ammo", &g_infiniteMortarShotAmmo, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteHeavyShotAmmo", "Infinite Heavy Shot Ammo", &g_infiniteHeavyShotAmmo, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteFireBarrels", "Infinite Fire Barrels", &g_infiniteFireBarrels, &g_inventoryPatchReady, 0, nullptr},
    {"PlayerGodmode", "Player Godmode", &g_playerGodmode, &g_playerHealthPatchReady, 0, nullptr},
    {"InfiniteBreath", "Infinite Breath", &g_infiniteBreath, &g_infiniteBreathPatchReady, 0, nullptr},
    {"StealthMode", "Stealth Mode", &g_stealthMode, &g_stealthModePatchReady, 0, &AfterStealthModeToggle},
    {"NoReload", "No Reload", &g_noReload, &g_noReloadPatchReady, 0, nullptr},
    {"FreezeMissionTimer", "Freeze Mission Timer", &g_freezeMissionTimer, &g_missionTimersPatchReady, 0, nullptr},
    {"InfiniteMoney", "Infinite Money", &g_infiniteMoney, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteSugar", "Infinite Sugar", &g_infiniteSugar, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteRum", "Infinite Rum", &g_infiniteRum, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteBullets", "Infinite Bullets", &g_infiniteBullets, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteRopeDarts", "Infinite Rope Darts", &g_infiniteRopeDarts, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteWood", "Infinite Wood", &g_infiniteWood, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteSleepDarts", "Infinite Sleep Darts", &g_infiniteSleepDarts, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteThrowingKnives", "Infinite Throwing Knives", &g_infiniteThrowingKnives, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteMetal", "Infinite Metal", &g_infiniteMetal, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteSmokebombs", "Infinite Smokebombs", &g_infiniteSmokeBombs, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteBerserkDarts", "Infinite Berserk Darts", &g_infiniteBerserkDarts, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteHarpoons", "Infinite Harpoons", &g_infiniteHarpoons, &g_inventoryPatchReady, 0, nullptr},
    {"InfiniteCloth", "Infinite Cloth", &g_infiniteCloth, &g_inventoryPatchReady, 0, nullptr},
    {"Noclip", "Noclip", &g_noclipEnabled, &g_noclipPatchReady, 0, &AfterNoclipToggle},
    {"TimeScale", "Time Scale", &g_timeScaleEnabled, &g_timeScalePatchReady, 0, &AfterTimeScaleToggle},
};

constexpr int kActionCount = sizeof(g_actions) / sizeof(g_actions[0]);

void SetGameInputCaptured(bool captured) {
    if (g_inputCaptured == captured) {
        return;
    }
    g_inputCaptured = captured;
    ClipCursor(nullptr);
}

bool IsMouseInputCaptureActive() {
    return g_menuOpen && g_disableMouseInputWhenUiOpen;
}

bool IsKeyboardInputCaptureActive() {
    return g_menuOpen && g_disableKeyboardInputWhenUiOpen;
}

void RefreshInputCaptureState() {
    SetGameInputCaptured(IsMouseInputCaptureActive() || IsKeyboardInputCaptureActive());
}

bool IsMouseDevice(IDirectInputDevice8A* device) {
    if (!device) {
        return false;
    }
    DIDEVCAPS caps{};
    caps.dwSize = sizeof(caps);
    if (FAILED(device->GetCapabilities(&caps))) {
        return false;
    }
    return GET_DIDEVICE_TYPE(caps.dwDevType) == DI8DEVTYPE_MOUSE;
}

bool IsKeyboardDevice(IDirectInputDevice8A* device) {
    if (!device) {
        return false;
    }
    DIDEVCAPS caps{};
    caps.dwSize = sizeof(caps);
    if (FAILED(device->GetCapabilities(&caps))) {
        return false;
    }
    return GET_DIDEVICE_TYPE(caps.dwDevType) == DI8DEVTYPE_KEYBOARD;
}

bool IsMouseVirtualKey(int vk) {
    return vk >= VK_LBUTTON && vk <= VK_XBUTTON2;
}

bool ShouldBlockPolledKeyboardKey(int vk) {
    if (!IsKeyboardInputCaptureActive()) {
        return false;
    }
    if (vk <= 0 || vk >= 256) {
        return false;
    }
    if (vk == g_menuHotkey || IsMouseVirtualKey(vk)) {
        return false;
    }
    return true;
}

SHORT QueryPhysicalKeyState(int vk) {
    if (g_originalGetAsyncKeyState) {
        return g_originalGetAsyncKeyState(vk);
    }
    return GetAsyncKeyState(vk);
}

void UpdateMouseWindowLock() {
    if (!g_lockMouseToWindow || !g_gameWindow || GetForegroundWindow() != g_gameWindow) {
        ClipCursor(nullptr);
        return;
    }

    RECT rect{};
    if (!GetClientRect(g_gameWindow, &rect)) {
        ClipCursor(nullptr);
        return;
    }

    POINT topLeft{rect.left, rect.top};
    POINT bottomRight{rect.right, rect.bottom};
    if (!ClientToScreen(g_gameWindow, &topLeft) ||
        !ClientToScreen(g_gameWindow, &bottomRight)) {
        ClipCursor(nullptr);
        return;
    }

    RECT clip{topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
    ClipCursor(&clip);
}

void ReleaseMenuInputState() {
    g_draggingWindow = false;
    SetGameInputCaptured(false);
    if (g_imguiReady) {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
        io.MouseDown[0] = false;
        io.MouseDown[1] = false;
        io.MouseDown[2] = false;
        io.ClearInputKeys();
    }
}

void Log(const char* message) {
    if (!g_consoleLoggingEnabled && !g_fileLoggingEnabled) {
        return;
    }

    SYSTEMTIME now{};
    GetLocalTime(&now);
    char stamped[224]{};
    sprintf_s(stamped,
              "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s",
              now.wYear,
              now.wMonth,
              now.wDay,
              now.wHour,
              now.wMinute,
              now.wSecond,
              now.wMilliseconds,
              message);

    if (g_consoleLoggingEnabled && g_consoleOut) {
        fprintf(g_consoleOut, "%s\n", stamped);
        fflush(g_consoleOut);
    } else if (g_consoleLoggingEnabled) {
        const int index = (g_pendingConsoleLineStart + g_pendingConsoleLineCount) % 96;
        strncpy_s(g_pendingConsoleLines[index], stamped, _TRUNCATE);
        if (g_pendingConsoleLineCount < 96) {
            ++g_pendingConsoleLineCount;
        } else {
            g_pendingConsoleLineStart = (g_pendingConsoleLineStart + 1) % 96;
        }
    }

    if (!g_fileLoggingEnabled) {
        return;
    }

    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    char* slash = strrchr(path, '\\');
    if (slash) {
        slash[1] = '\0';
    }
    strcat_s(path, "AC4Tools.log");

    FILE* file = nullptr;
    fopen_s(&file, path, "a");
    if (!file) {
        return;
    }
    fputs(stamped, file);
    fputc('\n', file);
    fclose(file);
}

void Logf(const char* format, ...) {
    char message[160]{};
    va_list args;
    va_start(args, format);
    vsprintf_s(message, format, args);
    va_end(args);
    Log(message);
}

bool IsTargetGameProcess() {
    char path[MAX_PATH]{};
    if (!GetModuleFileNameA(nullptr, path, MAX_PATH)) {
        return false;
    }
    const char* name = strrchr(path, '\\');
    name = name ? name + 1 : path;
    return _stricmp(name, "AC4BFSP.exe") == 0;
}

void ConfigPath(char* path, std::size_t size) {
    strcpy_s(path, size, g_moduleDir);
    strcat_s(path, size, "AC4Tools.ini");
}

const char* HotkeyName(int vk) {
    static char names[4][32]{};
    static int index = 0;
    char* out = names[index++ % 4];
    out[0] = '\0';

    if (vk == 0) {
        strcpy_s(out, 32, "None");
        return out;
    }
    if (vk >= 'A' && vk <= 'Z') {
        out[0] = static_cast<char>(vk);
        out[1] = '\0';
        return out;
    }
    if (vk >= '0' && vk <= '9') {
        out[0] = static_cast<char>(vk);
        out[1] = '\0';
        return out;
    }

    switch (vk) {
        case VK_ESCAPE: strcpy_s(out, 32, "Escape"); return out;
        case VK_SPACE: strcpy_s(out, 32, "Space"); return out;
        case VK_TAB: strcpy_s(out, 32, "Tab"); return out;
        case VK_RETURN: strcpy_s(out, 32, "Enter"); return out;
        case VK_BACK: strcpy_s(out, 32, "Backspace"); return out;
        case VK_DELETE: strcpy_s(out, 32, "Delete"); return out;
        case VK_INSERT: strcpy_s(out, 32, "Insert"); return out;
        case VK_HOME: strcpy_s(out, 32, "Home"); return out;
        case VK_END: strcpy_s(out, 32, "End"); return out;
        case VK_PRIOR: strcpy_s(out, 32, "PageUp"); return out;
        case VK_NEXT: strcpy_s(out, 32, "PageDown"); return out;
        case VK_UP: strcpy_s(out, 32, "Up"); return out;
        case VK_DOWN: strcpy_s(out, 32, "Down"); return out;
        case VK_LEFT: strcpy_s(out, 32, "Left"); return out;
        case VK_RIGHT: strcpy_s(out, 32, "Right"); return out;
        case VK_SHIFT: strcpy_s(out, 32, "Shift"); return out;
        case VK_CONTROL: strcpy_s(out, 32, "Ctrl"); return out;
        case VK_MENU: strcpy_s(out, 32, "Alt"); return out;
        case VK_F1: case VK_F2: case VK_F3: case VK_F4:
        case VK_F5: case VK_F6: case VK_F7: case VK_F8:
        case VK_F9: case VK_F10: case VK_F11: case VK_F12:
            sprintf_s(out, 32, "F%d", vk - VK_F1 + 1);
            return out;
        default:
            UINT scan = MapVirtualKeyA(static_cast<UINT>(vk), MAPVK_VK_TO_VSC);
            LONG lparam = static_cast<LONG>(scan << 16);
            if (GetKeyNameTextA(lparam, out, 32) > 0) {
                return out;
            }
            sprintf_s(out, 32, "VK_%02X", vk);
            return out;
    }
}

bool IsBindableHotkey(int vk) {
    if (vk <= 0 || vk >= 256) {
        return false;
    }
    if (vk >= VK_LBUTTON && vk <= VK_XBUTTON2) {
        return false;
    }
    return true;
}

void SaveConfig() {
    char path[MAX_PATH]{};
    ConfigPath(path, MAX_PATH);
    char value[32]{};
    WritePrivateProfileStringA("Logging", "EnableConsole", g_consoleLoggingEnabled ? "1" : "0", path);
    WritePrivateProfileStringA("Logging", "EnableFile", g_fileLoggingEnabled ? "1" : "0", path);
    WritePrivateProfileStringA("System", "LockMouseToWindow", g_lockMouseToWindow ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableMouseInputWhenUiOpen", g_disableMouseInputWhenUiOpen ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableKeyboardInputWhenUiOpen", g_disableKeyboardInputWhenUiOpen ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableHotkeysWhileUiOpen", g_disableHotkeysWhileUiOpen ? "1" : "0", path);
    sprintf_s(value, "%.0f", g_menuPos.x);
    WritePrivateProfileStringA("UI", "WindowPosX", value, path);
    sprintf_s(value, "%.0f", g_menuPos.y);
    WritePrivateProfileStringA("UI", "WindowPosY", value, path);
    sprintf_s(value, "%.0f", g_menuSize.x);
    WritePrivateProfileStringA("UI", "WindowSizeX", value, path);
    sprintf_s(value, "%.0f", g_menuSize.y);
    WritePrivateProfileStringA("UI", "WindowSizeY", value, path);
    sprintf_s(value, "%.6f", g_timeScale);
    WritePrivateProfileStringA("Game", "TimeScale", value, path);
    sprintf_s(value, "%.6f", g_noclipSpeed);
    WritePrivateProfileStringA("Noclip", "Speed", value, path);
    sprintf_s(value, "%.6f", g_noclipBoostSpeed);
    WritePrivateProfileStringA("Noclip", "BoostSpeed", value, path);
    sprintf_s(value, "%d", g_menuHotkey);
    WritePrivateProfileStringA("Hotkeys", "MenuOpen", value, path);
    for (int i = 0; i < kActionCount; ++i) {
        sprintf_s(value, "%d", g_actions[i].hotkey);
        WritePrivateProfileStringA("Hotkeys", g_actions[i].id, value, path);
    }
}

bool WriteMemory(void* address, const void* bytes, std::size_t size) {
    DWORD oldProtect = 0;
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    memcpy(address, bytes, size);
    FlushInstructionCache(GetCurrentProcess(), address, size);
    DWORD ignored = 0;
    VirtualProtect(address, size, oldProtect, &ignored);
    return true;
}

bool WriteJump(std::uint8_t* address, const void* target, std::size_t patchSize) {
    std::uint8_t patch[16]{};
    patch[0] = 0xE9;
    *reinterpret_cast<std::int32_t*>(&patch[1]) =
        static_cast<std::int32_t>(reinterpret_cast<const std::uint8_t*>(target) - address - 5);
    for (std::size_t i = 5; i < patchSize; ++i) {
        patch[i] = 0x90;
    }
    return WriteMemory(address, patch, patchSize);
}

bool BytesMatch(const void* address, const std::uint8_t* expected, std::size_t size) {
    return memcmp(address, expected, size) == 0;
}

std::uint8_t* FindMainModulePattern(const std::uint8_t* pattern, std::size_t size) {
    auto* base = reinterpret_cast<std::uint8_t*>(GetModuleHandleA(nullptr));
    if (!base || !pattern || size == 0) {
        return nullptr;
    }

    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.SizeOfImage < size) {
        return nullptr;
    }

    const std::size_t imageSize = nt->OptionalHeader.SizeOfImage;
    for (std::size_t offset = 0; offset + size <= imageSize; ++offset) {
        if (memcmp(base + offset, pattern, size) == 0) {
            return base + offset;
        }
    }
    return nullptr;
}

std::uint8_t* FindMainModulePatternMasked(const std::uint8_t* pattern, const char* mask, std::size_t size) {
    auto* base = reinterpret_cast<std::uint8_t*>(GetModuleHandleA(nullptr));
    if (!base || !pattern || !mask || size == 0) {
        return nullptr;
    }

    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.SizeOfImage < size) {
        return nullptr;
    }

    const std::size_t imageSize = nt->OptionalHeader.SizeOfImage;
    for (std::size_t offset = 0; offset + size <= imageSize; ++offset) {
        bool matched = true;
        for (std::size_t i = 0; i < size; ++i) {
            if (mask[i] == 'x' && base[offset + i] != pattern[i]) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return base + offset;
        }
    }
    return nullptr;
}

void FormatBytes(const void* bytes, std::size_t size, char* output, std::size_t outputSize) {
    output[0] = '\0';
    const auto* data = static_cast<const std::uint8_t*>(bytes);
    for (std::size_t i = 0; i < size; ++i) {
        char part[8]{};
        sprintf_s(part, "%s%02X", i == 0 ? "" : " ", data[i]);
        strcat_s(output, outputSize, part);
    }
}

void LogPatchMismatch(const char* label,
                      const void* address,
                      const std::uint8_t* expected,
                      std::size_t size) {
    char expectedText[96]{};
    char foundText[96]{};
    FormatBytes(expected, size, expectedText, sizeof(expectedText));
    FormatBytes(address, size, foundText, sizeof(foundText));
    Logf("Refusing %s: expected [%s], found [%s].", label, expectedText, foundText);
}

void ApplyNoCannonCooldownPatch() {
    if (!g_noCannonCooldownPatchReady) {
        g_noCannonCooldown = false;
        return;
    }

    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoCannonCooldownAddress);
    const std::uint8_t desired = g_noCannonCooldown ? kEnabledNoCannonCooldownByte : kOriginalNoCannonCooldownByte;
    if (*patchAddress == desired) {
        return;
    }
    if (*patchAddress != kOriginalNoCannonCooldownByte &&
        *patchAddress != kEnabledNoCannonCooldownByte) {
        g_noCannonCooldown = false;
        g_noCannonCooldownPatchReady = false;
        Logf("No Cannon Cooldown disabled: byte at 0x%08X changed unexpectedly to 0x%02X.",
             kPatchNoCannonCooldownAddress,
             *patchAddress);
        return;
    }
    if (!WriteMemory(patchAddress, &desired, sizeof(desired))) {
        g_noCannonCooldown = false;
        g_noCannonCooldownPatchReady = false;
        Log("No Cannon Cooldown disabled: failed to write patch byte.");
    }
}

void ApplyStealthModePatch() {
    if (!g_stealthModePatchReady) {
        g_stealthMode = false;
        return;
    }

    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchStealthModeAddress);
    const std::uint8_t desired = g_stealthMode ? kEnabledStealthModeByte : kOriginalStealthModeByte;
    if (*patchAddress == desired) {
        return;
    }
    if (*patchAddress != kOriginalStealthModeByte &&
        *patchAddress != kEnabledStealthModeByte) {
        g_stealthMode = false;
        g_stealthModePatchReady = false;
        Logf("Stealth Mode disabled: byte at 0x%08X changed unexpectedly to 0x%02X.",
             kPatchStealthModeAddress,
             *patchAddress);
        return;
    }
    if (!WriteMemory(patchAddress, &desired, sizeof(desired))) {
        g_stealthMode = false;
        g_stealthModePatchReady = false;
        Log("Stealth Mode disabled: failed to write patch byte.");
    }
}

void EmitJump(std::uint8_t*& p, std::uintptr_t target) {
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(target) - p - 4);
    p += 4;
}

void EmitCall(std::uint8_t*& p, const void* target) {
    *p++ = 0xE8;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<const std::uint8_t*>(target) - p - 4);
    p += 4;
}

void InitModuleDir() {
    GetModuleFileNameA(reinterpret_cast<HMODULE>(&__ImageBase), g_moduleDir, MAX_PATH);
    char* slash = strrchr(g_moduleDir, '\\');
    if (slash) {
        slash[1] = '\0';
    }
}

void InitGameInfo() {
    if (!GetModuleFileNameA(nullptr, g_gameExePath, MAX_PATH)) {
        return;
    }

    const char* name = strrchr(g_gameExePath, '\\');
    strcpy_s(g_gameExeName, name ? name + 1 : g_gameExePath);

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExA(g_gameExePath, GetFileExInfoStandard, &data)) {
        return;
    }

    ULARGE_INTEGER size{};
    size.HighPart = data.nFileSizeHigh;
    size.LowPart = data.nFileSizeLow;
    sprintf_s(g_gameExeSizeText, "%llu bytes", size.QuadPart);

    FILETIME localFileTime{};
    SYSTEMTIME systemTime{};
    if (FileTimeToLocalFileTime(&data.ftLastWriteTime, &localFileTime) &&
        FileTimeToSystemTime(&localFileTime, &systemTime)) {
        sprintf_s(g_gameExeTimestampText,
                  "%04u-%02u-%02u %02u:%02u:%02u",
                  systemTime.wYear,
                  systemTime.wMonth,
                  systemTime.wDay,
                  systemTime.wHour,
                  systemTime.wMinute,
                  systemTime.wSecond);
    }
}

void __stdcall ApplyInventoryValue() {
    const int itemId = g_inventoryItemId;
    g_inventoryLastItemId = itemId;
    g_inventoryLastOriginalValue = g_inventoryValue;
    if (g_infiniteMoney && itemId == 1) {
        g_inventoryValue = 999999;
    } else if (g_infiniteSugar && itemId == 0x10) {
        g_inventoryValue = 2500;
    } else if (g_infiniteRum && itemId == 0x11) {
        g_inventoryValue = 2500;
    } else if (g_infiniteCloth && itemId == 0x12) {
        g_inventoryValue = 2500;
    } else if (g_infiniteWood && itemId == 0x13) {
        g_inventoryValue = 2500;
    } else if (g_infiniteMetal && itemId == 0x14) {
        g_inventoryValue = 2500;
    } else if (g_infiniteSmokeBombs && itemId == 5) {
        g_inventoryValue = 15;
    } else if (g_infiniteBullets && itemId == 0x0B) {
        g_inventoryValue = 30;
    } else if (g_infiniteSleepDarts && itemId == 0x23) {
        g_inventoryValue = 15;
    } else if (g_infiniteBerserkDarts && itemId == 0x24) {
        g_inventoryValue = 15;
    } else if (g_infiniteRopeDarts && itemId == 0x20) {
        g_inventoryValue = 15;
    } else if (g_infiniteHarpoons && itemId == 0x41) {
        g_inventoryValue = 40;
    } else if (g_infiniteThrowingKnives && itemId == 8) {
        g_inventoryValue = 1;
    }
    g_inventoryLastAppliedValue = g_inventoryValue;
}

bool WriteInventoryValueByOffset(std::uintptr_t itemOffset, int value) {
    if (!g_inventoryBase) {
        return false;
    }

    __try {
        auto* inventory = reinterpret_cast<std::uint8_t*>(g_inventoryBase);
        auto* itemTable = *reinterpret_cast<std::uint8_t**>(inventory + 0x0C);
        if (!itemTable) {
            return false;
        }

        auto* item = *reinterpret_cast<std::uint8_t**>(itemTable + itemOffset);
        if (!item) {
            return false;
        }

        auto* amount = reinterpret_cast<int*>(item + 0x0C);
        if (*amount < value) {
            *amount = value;
            ++g_inventoryPointerLastWrites;
        }
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void MaintainInventoryPointerValues() {
    bool touched = false;
    if (g_infiniteShipCrew) {
        touched |= WriteInventoryValueByOffset(0x80, 40);
    }
    if (g_infiniteMortarShotAmmo) {
        touched |= WriteInventoryValueByOffset(0x84, 15);
    }
    if (g_infiniteHeavyShotAmmo) {
        touched |= WriteInventoryValueByOffset(0x88, 25);
    }
    if (g_infiniteFireBarrels) {
        touched |= WriteInventoryValueByOffset(0x8C, 25);
    }
    if (g_infiniteMoney) {
        touched |= WriteInventoryValueByOffset(0x00, 999999);
    }
    if (g_infiniteSmokeBombs) {
        touched |= WriteInventoryValueByOffset(0x04, 15);
    }
    if (g_infiniteBullets) {
        touched |= WriteInventoryValueByOffset(0x40, 30);
    }
    if (g_infiniteRopeDarts) {
        touched |= WriteInventoryValueByOffset(0x54, 15);
    }
    if (g_infiniteSleepDarts) {
        touched |= WriteInventoryValueByOffset(0x78, 15);
    }
    if (g_infiniteBerserkDarts) {
        touched |= WriteInventoryValueByOffset(0x7C, 15);
    }
    if (g_infiniteHarpoons) {
        touched |= WriteInventoryValueByOffset(0xA4, 40);
    }
    if (g_infiniteThrowingKnives) {
        touched |= WriteInventoryValueByOffset(0xAC, 1);
    }
    if (g_infiniteCloth) {
        touched |= WriteInventoryValueByOffset(0x90, 2500);
    }
    if (g_infiniteMetal) {
        touched |= WriteInventoryValueByOffset(0x94, 2500);
    }
    if (g_infiniteRum) {
        touched |= WriteInventoryValueByOffset(0x98, 2500);
    }
    if (g_infiniteSugar) {
        touched |= WriteInventoryValueByOffset(0x9C, 2500);
    }
    if (g_infiniteWood) {
        touched |= WriteInventoryValueByOffset(0xA0, 2500);
    }

    if (touched) {
        g_inventoryLastItemId = 0xFF;
        g_inventoryLastOriginalValue = g_inventoryPointerLastWrites;
        g_inventoryLastAppliedValue = 2500;
    }
}

enum class UnlockCategory {
    Pistols,
    Swords,
    Outfits,
    ShipCosmetics,
    EliteUnlocks,
};

enum class UnlockMode {
    ItemFlag,
    NormalBundle,
};

struct UnlockEntry {
    const char* name;
    UnlockCategory category;
    UnlockMode mode;
    std::uint8_t group;
    std::uint8_t id[8];
    std::uintptr_t flagAddress;
    bool enabled;
    bool found;
    bool patched;
};

UnlockEntry g_unlockEntries[] = {
    {"Golden Flintlock Pistols", UnlockCategory::Pistols, UnlockMode::ItemFlag, 0x01, {0xC0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Captain's Wheellock Pistols (CC)", UnlockCategory::Pistols, UnlockMode::ItemFlag, 0x01, {0xF0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Precision Shooter", UnlockCategory::Pistols, UnlockMode::NormalBundle, 0x02, {0x1A, 0x9A, 0x37, 0x66, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Pistol Swords", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x01, {0xCA, 0x7A, 0x0A, 0x1D, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Scottish Broadsword", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x01, {0xA8, 0xC9, 0x45, 0xD2, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Persian Scimitars (CC)", UnlockCategory::Swords, UnlockMode::ItemFlag, 0x01, {0xE2, 0x30, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Crude Iron Machete", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x02, {0xFD, 0xCB, 0x57, 0x7B, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Mayan Machete", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x02, {0x22, 0x2F, 0x67, 0x81, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Governor Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0x27, 0x70, 0x48, 0xD5, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Templar Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0x12, 0x43, 0x85, 0x16, 0x0D, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Stealth Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0xCB, 0x7A, 0x0A, 0x1D, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Explorer Outfit (CC)", UnlockCategory::Outfits, UnlockMode::ItemFlag, 0x01, {0x33, 0x6D, 0x1F, 0x1C, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Gilded Sails", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xF7, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"The Ranger Figurehead & Queen Anne's Revenge Wheel", UnlockCategory::ShipCosmetics, UnlockMode::NormalBundle, 0x01, {0x83, 0xD9, 0x85, 0x3E, 0x0D, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Aquila Figurehead", UnlockCategory::ShipCosmetics, UnlockMode::NormalBundle, 0x01, {0x2A, 0x70, 0x48, 0xD5, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"El Impoluto Figurehead (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xC1, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"El Impoluto Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x78, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Black and Red Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xE8, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Queen Anne Figurehead (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x9E, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Blackwood Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xF7, 0x2D, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Aquila Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x8C, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Flower Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x85, 0x2B, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Grey Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xED, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Elite Hull (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xE2, 0x45, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Set of Cannons (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x65, 0xD4, 0x41, 0x72, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Ram (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x1E, 0x4D, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Round Shot (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xA5, 0x4A, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Mortars (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x94, 0x34, 0x9F, 0x5F, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Swivel Guns (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x4C, 0x4E, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Heavy Shot (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xC9, 0x4B, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Fire Barrel (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x2A, 0x50, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Heavy Shot Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xDB, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Mortar Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xC7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Fire Barrel Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xE7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Harpoon (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x67, 0x51, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
};

constexpr int kUnlockEntryCount = sizeof(g_unlockEntries) / sizeof(g_unlockEntries[0]);
constexpr std::uintptr_t kUnlockRecordPointerBackOffset = 8;

int CountUnlockEntries(UnlockCategory category) {
    int count = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].category == category) {
            ++count;
        }
    }
    return count;
}

int CountEnabledUnlocks() {
    int count = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled) {
            ++count;
        }
    }
    return count;
}

void UpdateUnlockCounters() {
    g_unlockPistolsFound = 0;
    g_unlockPistolsPatched = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].found) {
            ++g_unlockPistolsFound;
        }
        if (g_unlockEntries[i].patched) {
            ++g_unlockPistolsPatched;
        }
    }
}

bool IsReadableDataPage(DWORD protect) {
    if ((protect & PAGE_GUARD) != 0 || (protect & PAGE_NOACCESS) != 0) {
        return false;
    }

    const DWORD basicProtect = protect & 0xFF;
    return basicProtect == PAGE_READONLY ||
           basicProtect == PAGE_READWRITE ||
           basicProtect == PAGE_WRITECOPY;
}

bool TryReadPointer(std::uintptr_t address, std::uintptr_t* value) {
    if (!value) {
        return false;
    }

    __try {
        *value = *reinterpret_cast<std::uintptr_t*>(address);
        return *value != 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadByte(std::uintptr_t address, std::uint8_t* value) {
    if (!value) {
        return false;
    }

    __try {
        *value = *reinterpret_cast<std::uint8_t*>(address);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadWord(std::uintptr_t address, std::uint16_t* value) {
    if (!value) {
        return false;
    }

    __try {
        *value = *reinterpret_cast<std::uint16_t*>(address);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadDword(std::uintptr_t address, std::uint32_t* value) {
    if (!value) {
        return false;
    }

    __try {
        *value = *reinterpret_cast<std::uint32_t*>(address);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteByte(std::uintptr_t address, std::uint8_t value) {
    __try {
        *reinterpret_cast<std::uint8_t*>(address) = value;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteWord(std::uintptr_t address, std::uint16_t value) {
    __try {
        *reinterpret_cast<std::uint16_t*>(address) = value;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteDword(std::uintptr_t address, std::uint32_t value) {
    __try {
        *reinterpret_cast<std::uint32_t*>(address) = value;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool UnlockPatternMatches(const std::uint8_t* bytes, const UnlockEntry& entry) {
    return bytes[0] == entry.group &&
           bytes[1] == 0x00 &&
           bytes[2] == 0x00 &&
           bytes[3] == 0x80 &&
           memcmp(bytes + 4, entry.id, 8) == 0;
}

UnlockEntry* FindUnlockEntry(const std::uint8_t* bytes) {
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled && UnlockPatternMatches(bytes, g_unlockEntries[i])) {
            return &g_unlockEntries[i];
        }
    }
    return nullptr;
}

struct CommunityChallengeMapping {
    std::uint8_t itemId[8];
    std::uint8_t challengeId[8];
};

CommunityChallengeMapping g_communityChallengeMappings[] = {
    {{0xE2, 0x30, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, {0x1A, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xF0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, {0x0A, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x33, 0x6D, 0x1F, 0x1C, 0x09, 0x00, 0x00, 0x00}, {0x16, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xC1, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x12, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x78, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x06, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xE8, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x0E, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x9E, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x1F, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xF7, 0x2D, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, {0xC2, 0xB0, 0x24, 0x2F, 0x08, 0x00, 0x00, 0x00}},
    {{0x8C, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x23, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x85, 0x2B, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, {0xC6, 0xB0, 0x24, 0x2F, 0x08, 0x00, 0x00, 0x00}},
    {{0xED, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x17, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xE2, 0x45, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xE3, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x65, 0xD4, 0x41, 0x72, 0x07, 0x00, 0x00, 0x00}, {0xEB, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x1E, 0x4D, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xE7, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xA5, 0x4A, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xFB, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x94, 0x34, 0x9F, 0x5F, 0x07, 0x00, 0x00, 0x00}, {0xF3, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x4C, 0x4E, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xF7, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xC9, 0x4B, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x07, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x2A, 0x50, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x0B, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xDB, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xEA, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0xC7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xE2, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0xE7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xE6, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0x67, 0x51, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x13, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
};

const std::uint8_t* FindCommunityChallengeId(const UnlockEntry& entry) {
    for (int i = 0; i < static_cast<int>(sizeof(g_communityChallengeMappings) / sizeof(g_communityChallengeMappings[0])); ++i) {
        if (memcmp(entry.id, g_communityChallengeMappings[i].itemId, sizeof(entry.id)) == 0) {
            return g_communityChallengeMappings[i].challengeId;
        }
    }
    return nullptr;
}

std::uintptr_t FindUnlockRecordSlot(const std::uint8_t* id) {
    if (!id) {
        return 0;
    }

    const std::uint8_t patternPrefix[] = {0x01, 0x00, 0x00, 0x80};
    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= 12 &&
            mbi.RegionSize <= 0x400000) {
            auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
            __try {
                for (std::size_t offset = 0; offset + 12 <= mbi.RegionSize; ++offset) {
                    if (memcmp(bytes + offset, patternPrefix, sizeof(patternPrefix)) == 0 &&
                        memcmp(bytes + offset + 4, id, 8) == 0 &&
                        base + offset >= kUnlockRecordPointerBackOffset) {
                        return base + offset - kUnlockRecordPointerBackOffset;
                    }
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
            }
        }

        if (next <= address) {
            break;
        }
        address = next;
    }
    return 0;
}

std::uintptr_t FindCommunityHandlerValue() {
    const std::uint8_t pattern[] = {
        0x8D, 0x4E, 0x5C, 0xC7, 0x06, 0x00, 0x00, 0x00, 0x00,
        0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x46, 0x64, 0x00,
    };
    const char mask[] = "xxxxx????x????xxxx";
    auto* hit = FindMainModulePatternMasked(pattern, mask, sizeof(pattern));
    if (!hit) {
        return 0;
    }

    std::uint32_t value = 0;
    return TryReadDword(reinterpret_cast<std::uintptr_t>(hit + 5), &value) ? value : 0;
}

std::uintptr_t FindCommunityMemMap2() {
    const std::uint8_t pattern[] = {
        0x3B, 0xF3, 0x74, 0x35, 0x57, 0x8B, 0x3E, 0x85, 0xFF, 0x74, 0x1D, 0x8B, 0x0D,
    };
    auto* hit = FindMainModulePattern(pattern, sizeof(pattern));
    if (!hit) {
        return 0;
    }

    std::uint32_t memMapAddress = 0;
    std::uint32_t memMapRoot = 0;
    std::uint32_t memMap2 = 0;
    if (!TryReadDword(reinterpret_cast<std::uintptr_t>(hit + 13), &memMapAddress) ||
        !TryReadDword(memMapAddress, &memMapRoot) ||
        !TryReadDword(memMapRoot + 444, &memMap2)) {
        return 0;
    }
    return memMap2;
}

void MarkCommunityAllocation(std::uintptr_t allocation) {
    const std::uintptr_t memMap2 = FindCommunityMemMap2();
    if (memMap2 != 0) {
        TryWriteByte(memMap2 + (allocation >> 16), 129);
    }
}

std::uint8_t* AllocateCommunityMemory(std::size_t size) {
    auto* memory = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (memory) {
        MarkCommunityAllocation(reinterpret_cast<std::uintptr_t>(memory));
    }
    return memory;
}

bool FindCommunityChallengeList(std::uintptr_t* listFound) {
    if (!listFound) {
        return false;
    }

    const std::uintptr_t handlerValue = FindCommunityHandlerValue();
    if (handlerValue == 0) {
        return false;
    }

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= sizeof(std::uint32_t) &&
            mbi.RegionSize <= 0x400000) {
            auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
            __try {
                for (std::size_t offset = 0; offset + sizeof(std::uint32_t) <= mbi.RegionSize; offset += 4) {
                    if (*reinterpret_cast<const std::uint32_t*>(bytes + offset) != handlerValue) {
                        continue;
                    }
                    const std::uintptr_t candidate = base + offset;
                    std::uintptr_t point1 = 0;
                    std::uintptr_t point1Level1 = 0;
                    std::uintptr_t point1Level2 = 0;
                    std::uintptr_t point2 = 0;
                    std::uintptr_t list = 0;
                    std::uint8_t marker = 0;
                    if (TryReadPointer(candidate + 0x20, &point1) &&
                        point1 != candidate &&
                        TryReadPointer(point1 + 0x5C, &point1Level1) &&
                        TryReadPointer(point1Level1, &point1Level2) &&
                        TryReadPointer(point1Level2 + 0x0C, &point2) &&
                        TryReadPointer(point2 + 0x08, &list) &&
                        TryReadByte(list + 0x04, &marker) &&
                        marker == 9) {
                        *listFound = list;
                        return true;
                    }
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
            }
        }

        if (next <= address) {
            break;
        }
        address = next;
    }
    return false;
}

bool FinishCommunityChallengeForUnlock(const UnlockEntry& entry) {
    const std::uint8_t* challengeId = FindCommunityChallengeId(entry);
    if (!challengeId) {
        return false;
    }

    const std::uintptr_t challengeRecordSlot = FindUnlockRecordSlot(challengeId);
    if (challengeRecordSlot == 0) {
        Logf("%s community challenge not finished: challenge record was not found.", entry.name);
        return false;
    }

    std::uintptr_t listFound = 0;
    if (!FindCommunityChallengeList(&listFound)) {
        Logf("%s community challenge not finished: challenge list was not found.", entry.name);
        return false;
    }

    std::uint16_t length = 0;
    std::uint16_t capacity = 0;
    std::uintptr_t listArray = 0;
    if (!TryReadWord(listFound + 0x16, &length) ||
        !TryReadWord(listFound + 0x14, &capacity) ||
        !TryReadPointer(listFound + 0x10, &listArray) ||
        length > 1760) {
        Logf("%s community challenge not finished: challenge list metadata was invalid.", entry.name);
        return false;
    }

    if (!g_communityChallengeStorage) {
        g_communityChallengeStorage = AllocateCommunityMemory(8192);
        if (!g_communityChallengeStorage) {
            Log("Community challenge storage allocation failed.");
            return false;
        }
        g_communityChallengeNext = g_communityChallengeStorage + 0x400;
    }

    if (g_communityChallengeNext + 12 > g_communityChallengeStorage + 8192) {
        Log("Community challenge storage is full.");
        return false;
    }

    std::uint32_t const1 = 0;
    std::uint32_t const2 = 0;
    std::uintptr_t firstEntry = 0;
    if (!TryReadPointer(listArray, &firstEntry) ||
        !TryReadDword(firstEntry, &const1) ||
        !TryReadDword(firstEntry + 0x08, &const2)) {
        Logf("%s community challenge not finished: challenge list template was invalid.", entry.name);
        return false;
    }

    if (static_cast<unsigned int>(length) + 1 >= capacity) {
        const std::size_t expandedSize = static_cast<std::size_t>(capacity) * sizeof(std::uint32_t) + 100;
        std::uint8_t* expanded = AllocateCommunityMemory(expandedSize);
        if (!expanded) {
            Log("Community challenge list expansion allocation failed.");
            return false;
        }
        memcpy(expanded, reinterpret_cast<void*>(listArray), static_cast<std::size_t>(length) * sizeof(std::uint32_t));
        if (!TryWriteDword(reinterpret_cast<std::uintptr_t>(expanded) + 4 * length,
                           reinterpret_cast<std::uint32_t>(g_communityChallengeNext)) ||
            !TryWriteDword(listFound + 0x10, reinterpret_cast<std::uint32_t>(expanded)) ||
            !TryWriteWord(listFound + 0x14, capacity + 10)) {
            return false;
        }
    } else if (!TryWriteDword(listArray + 4 * length, reinterpret_cast<std::uint32_t>(g_communityChallengeNext))) {
        return false;
    }

    const std::uintptr_t newEntry = reinterpret_cast<std::uintptr_t>(g_communityChallengeNext);
    if (!TryWriteDword(newEntry, const1) ||
        !TryWriteDword(newEntry + 0x04, static_cast<std::uint32_t>(challengeRecordSlot)) ||
        !TryWriteDword(newEntry + 0x08, const2) ||
        !TryWriteWord(listFound + 0x16, length + 1)) {
        return false;
    }

    g_communityChallengeNext += 12;
    Logf("%s community challenge completion added.", entry.name);
    return true;
}

void ApplyItemFlagUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    std::uintptr_t item = 0;
    if (matchAddress < kUnlockRecordPointerBackOffset ||
        !TryReadPointer(matchAddress - kUnlockRecordPointerBackOffset, &item)) {
        return;
    }

    std::uint8_t currentValue = 0;
    const std::uintptr_t flagAddress = item + 0x38;
    if (!TryReadByte(flagAddress, &currentValue)) {
        return;
    }

    entry.found = true;
    entry.flagAddress = flagAddress;

    const bool unlockedNow = currentValue != 0 && TryWriteByte(flagAddress, 0);
    if (unlockedNow) {
        entry.patched = true;
        if (g_finishCommunityChallengesForUnlocks) {
            FinishCommunityChallengeForUnlock(entry);
        }
    } else if (currentValue == 0) {
        entry.patched = true;
    }
}

void ApplyNormalBundleUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    std::uintptr_t bundle = 0;
    if (matchAddress < kUnlockRecordPointerBackOffset ||
        !TryReadPointer(matchAddress - kUnlockRecordPointerBackOffset, &bundle)) {
        return;
    }

    std::uint8_t bundleState = 0;
    if (!TryReadByte(bundle + 0x0C, &bundleState)) {
        return;
    }

    entry.found = true;
    entry.flagAddress = bundle + 0x0C;
    bool patched = false;
    if (bundleState == 1 && TryWriteByte(bundle + 0x0C, 3)) {
        patched = true;
    } else if (bundleState == 3) {
        patched = true;
    }

    std::uintptr_t first = 0;
    std::uintptr_t second = 0;
    std::uintptr_t list = 0;
    if (TryReadPointer(bundle + 0x3C, &first) &&
        TryReadPointer(first, &second) &&
        TryReadPointer(second + 0x08, &list)) {
        std::uintptr_t itemPointerTable = 0;
        std::uint8_t count = 0;
        if (TryReadPointer(list + 0x24, &itemPointerTable) &&
            TryReadByte(list + 0x2A, &count)) {
            for (int i = 0; i < count && i < 64; ++i) {
                std::uintptr_t holder = 0;
                std::uintptr_t child = 0;
                if (TryReadPointer(itemPointerTable + i * sizeof(std::uintptr_t), &holder) &&
                    TryReadPointer(holder, &child)) {
                    std::uint8_t childState = 0;
                    if (TryReadByte(child + 0x38, &childState)) {
                        if (childState != 0 && TryWriteByte(child + 0x38, 0)) {
                            patched = true;
                        } else if (childState == 0) {
                            patched = true;
                        }
                    }
                }
            }
        }
    }

    entry.patched = patched;
}

void ApplyUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    if (entry.mode == UnlockMode::ItemFlag) {
        ApplyItemFlagUnlockEntry(entry, matchAddress);
    } else {
        ApplyNormalBundleUnlockEntry(entry, matchAddress);
    }
}

void ScanUnlockRegion(std::uintptr_t base, std::size_t size) {
    auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
    __try {
        for (std::size_t offset = 0; offset + 12 <= size; ++offset) {
            if (bytes[offset + 1] != 0x00 ||
                bytes[offset + 2] != 0x00 ||
                bytes[offset + 3] != 0x80) {
                continue;
            }
            UnlockEntry* entry = FindUnlockEntry(bytes + offset);
            if (entry) {
                ApplyUnlockEntry(*entry, base + offset);
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void ScanAndApplyUnlocks() {
    if (InterlockedCompareExchange(&g_unlockScanRunning, 1, 0) != 0) {
        return;
    }

    for (int i = 0; i < kUnlockEntryCount; ++i) {
        g_unlockEntries[i].found = false;
        g_unlockEntries[i].patched = false;
    }

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= 12 &&
            mbi.RegionSize <= 0x400000) {
            ScanUnlockRegion(base, mbi.RegionSize);
        }

        if (next <= address) {
            break;
        }
        address = next;
    }

    UpdateUnlockCounters();
    Logf("Unlock scan finished: enabled=%d found=%d patched=%d.",
         CountEnabledUnlocks(),
         g_unlockPistolsFound,
         g_unlockPistolsPatched);
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled && g_unlockEntries[i].patched) {
            g_unlockEntries[i].enabled = false;
        }
    }
    InterlockedExchange(&g_unlockScanRunning, 0);
}

void MaintainUnlocks() {
    if (CountEnabledUnlocks() == 0) {
        g_unlockPistolsLastScan = 0;
        UpdateUnlockCounters();
        InterlockedExchange(&g_unlockScanRequested, 0);
        return;
    }

    const DWORD now = GetTickCount();
    const bool requested = InterlockedExchange(&g_unlockScanRequested, 0) != 0;
    if (!requested && g_unlockPistolsLastScan != 0 && now - g_unlockPistolsLastScan < 15000) {
        return;
    }
    g_unlockPistolsLastScan = now;
    ScanAndApplyUnlocks();
}

void LoadConfig() {
    char ownPath[MAX_PATH]{};
    ConfigPath(ownPath, MAX_PATH);

    g_consoleLoggingEnabled = GetPrivateProfileIntA("Logging", "EnableConsole", 0, ownPath) != 0;
    g_fileLoggingEnabled = GetPrivateProfileIntA("Logging", "EnableFile", 0, ownPath) != 0;
    g_lockMouseToWindow = GetPrivateProfileIntA("System", "LockMouseToWindow", 0, ownPath) != 0;
    g_disableMouseInputWhenUiOpen = GetPrivateProfileIntA("System", "DisableMouseInputWhenUiOpen", 0, ownPath) != 0;
    g_disableKeyboardInputWhenUiOpen = GetPrivateProfileIntA("System", "DisableKeyboardInputWhenUiOpen", 0, ownPath) != 0;
    g_disableHotkeysWhileUiOpen = GetPrivateProfileIntA("System", "DisableHotkeysWhileUiOpen", 0, ownPath) != 0;
    char windowPosValue[32]{};
    GetPrivateProfileStringA("UI", "WindowPosX", "60", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuPos.x = static_cast<float>(atof(windowPosValue));
    GetPrivateProfileStringA("UI", "WindowPosY", "40", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuPos.y = static_cast<float>(atof(windowPosValue));
    GetPrivateProfileStringA("UI", "WindowSizeX", "680", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuSize.x = static_cast<float>(atof(windowPosValue));
    GetPrivateProfileStringA("UI", "WindowSizeY", "420", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuSize.y = static_cast<float>(atof(windowPosValue));
    if (g_menuSize.x < 320.0f) {
        g_menuSize.x = 320.0f;
    }
    if (g_menuSize.y < 240.0f) {
        g_menuSize.y = 240.0f;
    }

    char timeScaleValue[32]{};
    GetPrivateProfileStringA("Game", "TimeScale", "0.010000", timeScaleValue, sizeof(timeScaleValue), ownPath);
    g_timeScale = static_cast<float>(atof(timeScaleValue));
    if (g_timeScale < 0.0001f) {
        g_timeScale = 0.0001f;
    }
    if (g_timeScale > 100.0f) {
        g_timeScale = 100.0f;
    }

    char noclipSpeedValue[32]{};
    GetPrivateProfileStringA("Noclip", "Speed", "1.000000", noclipSpeedValue, sizeof(noclipSpeedValue), ownPath);
    g_noclipSpeed = static_cast<float>(atof(noclipSpeedValue));
    if (g_noclipSpeed < 0.0001f) {
        g_noclipSpeed = 0.0001f;
    }
    if (g_noclipSpeed > 1000.0f) {
        g_noclipSpeed = 1000.0f;
    }

    char noclipBoostValue[32]{};
    GetPrivateProfileStringA("Noclip", "BoostSpeed", "5.000000", noclipBoostValue, sizeof(noclipBoostValue), ownPath);
    g_noclipBoostSpeed = static_cast<float>(atof(noclipBoostValue));
    if (g_noclipBoostSpeed < 0.0001f) {
        g_noclipBoostSpeed = 0.0001f;
    }
    if (g_noclipBoostSpeed > 1000.0f) {
        g_noclipBoostSpeed = 1000.0f;
    }

    char keyValue[32]{};
    GetPrivateProfileStringA("Hotkeys", "MenuOpen", "66", keyValue, sizeof(keyValue), ownPath);
    int vk = atoi(keyValue);
    g_menuHotkey = IsBindableHotkey(vk) ? vk : 'B';

    for (int i = 0; i < kActionCount; ++i) {
        GetPrivateProfileStringA("Hotkeys", g_actions[i].id, "0", keyValue, sizeof(keyValue), ownPath);
        vk = atoi(keyValue);
        g_actions[i].hotkey = IsBindableHotkey(vk) && vk != g_menuHotkey ? vk : 0;
    }
}

void UpdateTimeScaleInterval() {
    if (!g_timeScaleEnabled) {
        g_timeScaleInterval = 10000;
    } else {
        const float clamped = g_timeScale < 0.0001f ? 0.0001f : g_timeScale;
        int interval = static_cast<int>((1000.0f / clamped) + 0.5f);
        if (interval < 1) {
            interval = 1;
        }
        g_timeScaleInterval = interval;
    }
}

void* GetPlayerActor() {
    using GetPlayerRootFn = void*(__cdecl*)();
    using GetActorFn = void*(__thiscall*)(void*);

    void* root = reinterpret_cast<GetPlayerRootFn>(0x0054D4D0)();
    if (!root) {
        return nullptr;
    }
    return reinterpret_cast<GetActorFn>(0x0070E810)(root);
}

void* GetPlayerEntity() {
    using GetEntityFn = void*(__cdecl*)(void*);
    return reinterpret_cast<GetEntityFn>(0x013905A0)(GetPlayerActor());
}

bool IsGameNoclipActive() {
    void* entity = GetPlayerEntity();
    if (!entity) {
        return false;
    }
    using GetStateFn = int(__thiscall*)(void*, int);
    return reinterpret_cast<GetStateFn>(0x0052C2E0)(entity, 1) == 3;
}

void* GetNoclipComponent() {
    void* actor = GetPlayerActor();
    if (!actor) {
        return nullptr;
    }
    using GetComponentFn = void*(__thiscall*)(void*);
    return reinterpret_cast<GetComponentFn>(0x005220D0)(actor);
}

void __stdcall UpdateNoclipState() {
    if (!g_noclipPatchReady) {
        g_noclipEnabled = false;
        return;
    }

    const bool active = IsGameNoclipActive();
    g_noclipActive = active;
    if (active == g_noclipEnabled) {
        return;
    }

    void* component = GetNoclipComponent();
    if (!component) {
        return;
    }

    if (g_noclipEnabled) {
        using EnableFn = void(__thiscall*)(void*, int);
        reinterpret_cast<EnableFn>(0x017FFDD0)(component, 0);
        Log("Noclip enable requested through game component.");
    } else {
        using DisableFn = void(__thiscall*)(void*);
        reinterpret_cast<DisableFn>(0x017FFE40)(component);
        Log("Noclip disable requested through game component.");
    }
}

void AssignMenuHotkey(int vk) {
    if (vk != 0 && !IsBindableHotkey(vk)) {
        return;
    }
    if (vk != 0) {
        for (int i = 0; i < kActionCount; ++i) {
            if (g_actions[i].hotkey == vk) {
                g_actions[i].hotkey = 0;
            }
        }
    }
    g_menuHotkey = vk;
    g_suppressedHotkeyVk = vk;
    SaveConfig();
    Logf("Hotkey for Open/Close Menu set to %s.", HotkeyName(vk));
}

void AssignHotkey(int actionIndex, int vk) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    if (vk != 0 && !IsBindableHotkey(vk)) {
        return;
    }
    if (vk != 0) {
        if (g_menuHotkey == vk) {
            g_menuHotkey = 0;
        }
        for (int i = 0; i < kActionCount; ++i) {
            if (i != actionIndex && g_actions[i].hotkey == vk) {
                g_actions[i].hotkey = 0;
            }
        }
    }
    g_actions[actionIndex].hotkey = vk;
    g_suppressedHotkeyVk = vk;
    SaveConfig();
    Logf("Hotkey for %s set to %s.", g_actions[actionIndex].label, HotkeyName(vk));
}

void ToggleActionFromHotkey(int actionIndex) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    ToggleAction& action = g_actions[actionIndex];
    if (!action.ready || !*action.ready || !action.value) {
        return;
    }
    *action.value = !*action.value;
    if (action.afterToggle) {
        action.afterToggle();
    }
    Logf("%s toggled %s from hotkey %s.",
         action.label,
         *action.value ? "ON" : "OFF",
         HotkeyName(action.hotkey));
}

void ProcessHotkeys() {
    static bool wasDown[256]{};
    if ((g_menuOpen && g_disableHotkeysWhileUiOpen) || GetForegroundWindow() != g_gameWindow) {
        for (int vk = 0; vk < 256; ++vk) {
            wasDown[vk] = (QueryPhysicalKeyState(vk) & 0x8000) != 0;
        }
        return;
    }

    for (int vk = 1; vk < 256; ++vk) {
        const bool down = (QueryPhysicalKeyState(vk) & 0x8000) != 0;
        if (g_suppressedHotkeyVk == vk) {
            wasDown[vk] = down;
            if (!down) {
                g_suppressedHotkeyVk = 0;
            }
            continue;
        }
        if (down && !wasDown[vk]) {
            for (int i = 0; i < kActionCount; ++i) {
                if (g_actions[i].hotkey == vk) {
                    ToggleActionFromHotkey(i);
                    break;
                }
            }
        }
        wasDown[vk] = down;
    }
}

bool InstallHealthPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchHealthAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing ship godmode patch: 0x01225C3F is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalHealthBytes, sizeof(kOriginalHealthBytes))) {
        LogPatchMismatch("ship godmode patch at 0x01225C3F",
                         patchAddress,
                         kOriginalHealthBytes,
                         sizeof(kOriginalHealthBytes));
        return false;
    }

    g_cave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_cave) {
        Log("VirtualAlloc failed.");
        return false;
    }

    std::uint8_t* p = g_cave;

    // cmp byte ptr [&g_enabled], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_enabled);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // cmp edx, 0
    *p++ = 0x83;
    *p++ = 0xFA;
    *p++ = 0x00;

    // jne filtered
    *p++ = 0x75;
    const auto jneOriginalOffset = p++;

    // mov dword ptr [esi+0xD0], 0x47C35000 ; 100000.0f
    *p++ = 0xC7;
    *p++ = 0x86;
    *reinterpret_cast<std::uint32_t*>(p) = 0x000000D0;
    p += 4;
    *reinterpret_cast<std::uint32_t*>(p) = 0x47C35000;
    p += 4;

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchHealthReturnAddress) - p - 4);
    p += 4;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    *jneOriginalOffset = static_cast<std::uint8_t>(originalLabel - jneOriginalOffset - 1);

    // movss [esi+0xD0], xmm0
    memcpy(p, kOriginalHealthBytes, sizeof(kOriginalHealthBytes));
    p += sizeof(kOriginalHealthBytes);

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchHealthReturnAddress) - p - 4);
    p += 4;

    if (!WriteJump(patchAddress, g_cave, sizeof(kOriginalHealthBytes))) {
        Log("Refusing ship godmode patch: failed to write hook at 0x01225C3F.");
        return false;
    }
    return true;
}

bool InstallNoCannonCooldownPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoCannonCooldownAddress);
    if (*patchAddress == kEnabledNoCannonCooldownByte) {
        Log("Refusing No Cannon Cooldown patch: 0x01C97031 is already patched by another tool.");
        return false;
    }
    if (*patchAddress != kOriginalNoCannonCooldownByte) {
        const std::uint8_t expected[] = {kOriginalNoCannonCooldownByte};
        LogPatchMismatch("No Cannon Cooldown patch at 0x01C97031",
                         patchAddress,
                         expected,
                         sizeof(expected));
        return false;
    }
    Log("No Cannon Cooldown byte patch ready at 0x01C97031.");
    return true;
}

bool InstallStealthModePatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchStealthModeAddress);
    if (*patchAddress == kEnabledStealthModeByte) {
        Log("Refusing Stealth Mode patch: 0x0101CB66 is already patched by another tool.");
        return false;
    }
    if (*patchAddress != kOriginalStealthModeByte) {
        const std::uint8_t expected[] = {kOriginalStealthModeByte};
        LogPatchMismatch("Stealth Mode patch at 0x0101CB66",
                         patchAddress,
                         expected,
                         sizeof(expected));
        return false;
    }
    Log("Stealth Mode byte patch ready at 0x0101CB66.");
    return true;
}

bool InstallAllyGodmodePatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchAllyGodmodeAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing Ally Godmode patch: 0x01BFF520 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalAllyGodmodeBytes, sizeof(kOriginalAllyGodmodeBytes))) {
        LogPatchMismatch("Ally Godmode patch at 0x01BFF520",
                         patchAddress,
                         kOriginalAllyGodmodeBytes,
                         sizeof(kOriginalAllyGodmodeBytes));
        return false;
    }

    g_allyGodmodeCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_allyGodmodeCave) {
        Log("VirtualAlloc failed for Ally Godmode patch.");
        return false;
    }

    std::uint8_t* p = g_allyGodmodeCave;

    // cmp byte ptr [&g_allyGodmode], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_allyGodmode);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // cmp dword ptr [ecx+0xF4], 1
    *p++ = 0x83;
    *p++ = 0xB9;
    *reinterpret_cast<std::uint32_t*>(p) = 0x000000F4;
    p += 4;
    *p++ = 0x01;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOwnerOffset = p++;

    // mov ax, word ptr [ecx+0x5E]
    *p++ = 0x66;
    *p++ = 0x8B;
    *p++ = 0x41;
    *p++ = 0x5E;

    // mov word ptr [ecx+0x5C], ax
    *p++ = 0x66;
    *p++ = 0x89;
    *p++ = 0x41;
    *p++ = 0x5C;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    *jeOriginalOwnerOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOwnerOffset - 1);

    memcpy(p, kOriginalAllyGodmodeBytes, sizeof(kOriginalAllyGodmodeBytes));
    p += sizeof(kOriginalAllyGodmodeBytes);

    if (!WriteJump(patchAddress, g_allyGodmodeCave, sizeof(kOriginalAllyGodmodeBytes))) {
        Log("Refusing Ally Godmode patch: failed to write hook at 0x01BFF520.");
        return false;
    }
    Log("Installed Ally Godmode hook at 0x01BFF520.");
    return true;
}

bool InstallTimeIntervalPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchTimeIntervalAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing TimeScale interval patch: 0x00A047EA is already hooked by another ASI.");
        return false;
    }

    if (!BytesMatch(patchAddress, kOriginalTimeIntervalBytes, sizeof(kOriginalTimeIntervalBytes))) {
        LogPatchMismatch("TimeScale interval patch at 0x00A047EA",
                         patchAddress,
                         kOriginalTimeIntervalBytes,
                         sizeof(kOriginalTimeIntervalBytes));
        return false;
    }

    g_timeIntervalCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_timeIntervalCave) {
        Log("VirtualAlloc failed for TimeScale interval patch.");
        return false;
    }

    std::uint8_t* p = g_timeIntervalCave;

    // inc dword ptr [&g_timeIntervalHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_timeIntervalHits);
    p += 4;

    // mov edx, dword ptr [&g_timeScaleInterval]
    *p++ = 0x8B;
    *p++ = 0x15;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_timeScaleInterval);
    p += 4;

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchTimeIntervalReturnAddress) - p - 4);
    p += 4;

    if (!WriteJump(patchAddress, g_timeIntervalCave, sizeof(kOriginalTimeIntervalBytes))) {
        Log("Refusing TimeScale interval patch: failed to write hook at 0x00A047EA.");
        return false;
    }
    Log("Installed standalone TimeScale interval hook at 0x00A047EA.");
    return true;
}

bool InstallPlayerHealthPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchPlayerHealthAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing player godmode patch: 0x0115AD07 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalPlayerHealthBytes, sizeof(kOriginalPlayerHealthBytes))) {
        LogPatchMismatch("player godmode patch at 0x0115AD07",
                         patchAddress,
                         kOriginalPlayerHealthBytes,
                         sizeof(kOriginalPlayerHealthBytes));
        return false;
    }

    g_playerHealthCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_playerHealthCave) {
        Log("VirtualAlloc failed for player godmode patch.");
        return false;
    }

    std::uint8_t* p = g_playerHealthCave;

    // inc dword ptr [&g_playerHealthHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerHealthHits);
    p += 4;

    // cmp byte ptr [&g_playerGodmode], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerGodmode);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    auto* jeOriginalOffset = p++;

    // mov ax, word ptr [esi+0x5E]
    *p++ = 0x66;
    *p++ = 0x8B;
    *p++ = 0x46;
    *p++ = 0x5E;

    // mov word ptr [esi+0x5C], ax
    *p++ = 0x66;
    *p++ = 0x89;
    *p++ = 0x46;
    *p++ = 0x5C;

    // test edi, edi
    *p++ = 0x85;
    *p++ = 0xFF;
    EmitJump(p, kPatchPlayerHealthReturnAddress);

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    memcpy(p, kOriginalPlayerHealthBytes, sizeof(kOriginalPlayerHealthBytes));
    p += sizeof(kOriginalPlayerHealthBytes);
    EmitJump(p, kPatchPlayerHealthReturnAddress);

    if (!WriteJump(patchAddress, g_playerHealthCave, sizeof(kOriginalPlayerHealthBytes))) {
        Log("Refusing player godmode patch: failed to write hook at 0x0115AD07.");
        return false;
    }
    Log("Installed player godmode hook at 0x0115AD07.");
    return true;
}

bool InstallInfiniteBreathPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInfiniteBreathAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing Infinite Breath patch: 0x01148936 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalInfiniteBreathBytes, sizeof(kOriginalInfiniteBreathBytes))) {
        LogPatchMismatch("Infinite Breath patch at 0x01148936",
                         patchAddress,
                         kOriginalInfiniteBreathBytes,
                         sizeof(kOriginalInfiniteBreathBytes));
        return false;
    }

    g_infiniteBreathCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_infiniteBreathCave) {
        Log("VirtualAlloc failed for Infinite Breath patch.");
        return false;
    }

    std::uint8_t* p = g_infiniteBreathCave;

    // inc dword ptr [&g_infiniteBreathHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_infiniteBreathHits);
    p += 4;

    // cmp byte ptr [&g_infiniteBreath], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_infiniteBreath);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // mov dword ptr [eax+0x80], 0x3F800000 ; 1.0f
    *p++ = 0xC7;
    *p++ = 0x80;
    *reinterpret_cast<std::uint32_t*>(p) = 0x00000080;
    p += 4;
    *reinterpret_cast<std::uint32_t*>(p) = 0x3F800000;
    p += 4;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);

    memcpy(p, kOriginalInfiniteBreathBytes, sizeof(kOriginalInfiniteBreathBytes));
    p += sizeof(kOriginalInfiniteBreathBytes);
    EmitJump(p, kPatchInfiniteBreathReturnAddress);

    if (!WriteJump(patchAddress, g_infiniteBreathCave, sizeof(kOriginalInfiniteBreathBytes))) {
        Log("Refusing Infinite Breath patch: failed to write hook at 0x01148936.");
        return false;
    }
    Log("Installed Infinite Breath hook at 0x01148936.");
    return true;
}

bool InstallInventoryPointerPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInventoryPointerAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing inventory pointer patch: 0x01CFD381 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalInventoryPointerBytes, sizeof(kOriginalInventoryPointerBytes))) {
        LogPatchMismatch("inventory pointer patch at 0x01CFD381",
                         patchAddress,
                         kOriginalInventoryPointerBytes,
                         sizeof(kOriginalInventoryPointerBytes));
        return false;
    }

    g_inventoryPointerCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 192, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_inventoryPointerCave) {
        Log("VirtualAlloc failed for inventory pointer patch.");
        return false;
    }

    std::uint8_t* p = g_inventoryPointerCave;

    memcpy(p, kOriginalInventoryPointerBytes, sizeof(kOriginalInventoryPointerBytes));
    p += sizeof(kOriginalInventoryPointerBytes);

    *p++ = 0x9C; // pushfd
    *p++ = 0x53; // push ebx

    *p++ = 0x85;
    *p++ = 0xC0; // test eax,eax
    *p++ = 0x74;
    auto* jeNoBhv = p++;

    *p++ = 0xA3; // mov [&g_bhvAssassin],eax
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_bhvAssassin);
    p += 4;

    *p++ = 0x8B;
    *p++ = 0x58;
    *p++ = 0x08; // mov ebx,[eax+08]
    *p++ = 0x89;
    *p++ = 0x1D; // mov [&g_playerEntity],ebx
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerEntity);
    p += 4;

    *p++ = 0x8B;
    *p++ = 0x58;
    *p++ = 0x1C; // mov ebx,[eax+1C]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoCharacter = p++;

    *p++ = 0x8B;
    *p++ = 0x5B;
    *p++ = 0x5C; // mov ebx,[ebx+5C]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoInventoryHolder = p++;

    *p++ = 0x8B;
    *p++ = 0x1B; // mov ebx,[ebx]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoInventory = p++;

    *p++ = 0x89;
    *p++ = 0x1D; // mov [&g_inventoryBase],ebx
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryBase);
    p += 4;

    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryPointerHits);
    p += 4;

    auto* done = p;
    *jeNoBhv = static_cast<std::uint8_t>(done - jeNoBhv - 1);
    *jeNoCharacter = static_cast<std::uint8_t>(done - jeNoCharacter - 1);
    *jeNoInventoryHolder = static_cast<std::uint8_t>(done - jeNoInventoryHolder - 1);
    *jeNoInventory = static_cast<std::uint8_t>(done - jeNoInventory - 1);

    *p++ = 0x5B; // pop ebx
    *p++ = 0x9D; // popfd
    EmitJump(p, kPatchInventoryPointerReturnAddress);

    if (!WriteJump(patchAddress, g_inventoryPointerCave, sizeof(kOriginalInventoryPointerBytes))) {
        Log("Refusing inventory pointer patch: failed to write hook at 0x01CFD381.");
        return false;
    }
    Log("Installed inventory pointer hook at 0x01CFD381.");
    return true;
}

bool InstallInventorySetPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInventorySetAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing inventory set patch: 0x011A1F3D is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalInventorySetBytes, sizeof(kOriginalInventorySetBytes))) {
        LogPatchMismatch("inventory set patch at 0x011A1F3D",
                         patchAddress,
                         kOriginalInventorySetBytes,
                         sizeof(kOriginalInventorySetBytes));
        return false;
    }

    g_inventorySetCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 96, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_inventorySetCave) {
        Log("VirtualAlloc failed for inventory set patch.");
        return false;
    }

    std::uint8_t* p = g_inventorySetCave;

    // inc dword ptr [&g_inventorySetHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventorySetHits);
    p += 4;

    // mov dword ptr [&g_inventoryValue], ecx
    *p++ = 0x89;
    *p++ = 0x0D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov dword ptr [&g_inventoryItemId], edi
    *p++ = 0x89;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryItemId);
    p += 4;

    // pushad; call ApplyInventoryValue; popad
    *p++ = 0x60;
    EmitCall(p, &ApplyInventoryValue);
    *p++ = 0x61;

    // mov ecx, dword ptr [&g_inventoryValue]
    *p++ = 0x8B;
    *p++ = 0x0D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov dword ptr [esi+0x0C], ecx; pop esi; pop ebp
    memcpy(p, kOriginalInventorySetBytes, sizeof(kOriginalInventorySetBytes));
    p += sizeof(kOriginalInventorySetBytes);
    EmitJump(p, kPatchInventorySetReturnAddress);

    if (!WriteJump(patchAddress, g_inventorySetCave, sizeof(kOriginalInventorySetBytes))) {
        Log("Refusing inventory set patch: failed to write hook at 0x011A1F3D.");
        return false;
    }
    Log("Installed inventory set hook at 0x011A1F3D.");
    return true;
}

bool InstallInventorySetAltPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInventorySetAltAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing inventory set-alt patch: 0x011A1F6F is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalInventorySetAltBytes, sizeof(kOriginalInventorySetAltBytes))) {
        LogPatchMismatch("inventory set-alt patch at 0x011A1F6F",
                         patchAddress,
                         kOriginalInventorySetAltBytes,
                         sizeof(kOriginalInventorySetAltBytes));
        return false;
    }

    g_inventorySetAltCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 96, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_inventorySetAltCave) {
        Log("VirtualAlloc failed for inventory set-alt patch.");
        return false;
    }

    std::uint8_t* p = g_inventorySetAltCave;

    // inc dword ptr [&g_inventorySetAltHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventorySetAltHits);
    p += 4;

    // mov dword ptr [&g_inventoryValue], eax
    *p++ = 0xA3;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov dword ptr [&g_inventoryItemId], edi
    *p++ = 0x89;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryItemId);
    p += 4;

    // pushad; call ApplyInventoryValue; popad
    *p++ = 0x60;
    EmitCall(p, &ApplyInventoryValue);
    *p++ = 0x61;

    // mov eax, dword ptr [&g_inventoryValue]
    *p++ = 0xA1;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov dword ptr [ecx+0x0C], eax; mov al, 1
    memcpy(p, kOriginalInventorySetAltBytes, sizeof(kOriginalInventorySetAltBytes));
    p += sizeof(kOriginalInventorySetAltBytes);
    EmitJump(p, kPatchInventorySetAltReturnAddress);

    if (!WriteJump(patchAddress, g_inventorySetAltCave, sizeof(kOriginalInventorySetAltBytes))) {
        Log("Refusing inventory set-alt patch: failed to write hook at 0x011A1F6F.");
        return false;
    }
    Log("Installed inventory set-alt hook at 0x011A1F6F.");
    return true;
}

bool InstallInventoryEntrySubtractPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInventoryEntrySubtractAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing inventory entry-subtract patch: 0x011A1FA3 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalInventoryEntrySubtractBytes, sizeof(kOriginalInventoryEntrySubtractBytes))) {
        LogPatchMismatch("inventory entry-subtract patch at 0x011A1FA3",
                         patchAddress,
                         kOriginalInventoryEntrySubtractBytes,
                         sizeof(kOriginalInventoryEntrySubtractBytes));
        return false;
    }

    g_inventoryEntrySubtractCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 112, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_inventoryEntrySubtractCave) {
        Log("VirtualAlloc failed for inventory entry-subtract patch.");
        return false;
    }

    std::uint8_t* p = g_inventoryEntrySubtractCave;

    // inc dword ptr [&g_inventoryEntrySubtractHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryEntrySubtractHits);
    p += 4;

    // mov dword ptr [&g_inventoryValue], eax
    *p++ = 0xA3;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov eax, dword ptr [esi+0x04]
    *p++ = 0x8B;
    *p++ = 0x46;
    *p++ = 0x04;

    // mov dword ptr [&g_inventoryItemId], eax
    *p++ = 0xA3;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryItemId);
    p += 4;

    // pushad; call ApplyInventoryValue; popad
    *p++ = 0x60;
    EmitCall(p, &ApplyInventoryValue);
    *p++ = 0x61;

    // mov eax, dword ptr [&g_inventoryValue]
    *p++ = 0xA1;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryValue);
    p += 4;

    // mov dword ptr [esi+0x0C], eax; mov al, 1
    memcpy(p, kOriginalInventoryEntrySubtractBytes, sizeof(kOriginalInventoryEntrySubtractBytes));
    p += sizeof(kOriginalInventoryEntrySubtractBytes);
    EmitJump(p, kPatchInventoryEntrySubtractReturnAddress);

    if (!WriteJump(patchAddress, g_inventoryEntrySubtractCave, sizeof(kOriginalInventoryEntrySubtractBytes))) {
        Log("Refusing inventory entry-subtract patch: failed to write hook at 0x011A1FA3.");
        return false;
    }
    Log("Installed inventory entry-subtract hook at 0x011A1FA3.");
    return true;
}

bool InstallNoclipUpdatePatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoclipUpdateAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing noclip update patch: 0x016FAACD is already hooked by another ASI.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalNoclipUpdateBytes, sizeof(kOriginalNoclipUpdateBytes))) {
        LogPatchMismatch("noclip update patch at 0x016FAACD",
                         patchAddress,
                         kOriginalNoclipUpdateBytes,
                         sizeof(kOriginalNoclipUpdateBytes));
        return false;
    }

    g_noclipUpdateCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_noclipUpdateCave) {
        Log("VirtualAlloc failed for noclip update patch.");
        return false;
    }

    std::uint8_t* p = g_noclipUpdateCave;

    // inc dword ptr [&g_noclipUpdateHits]
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noclipUpdateHits);
    p += 4;

    // pushad; call UpdateNoclipState; popad
    *p++ = 0x60;
    EmitCall(p, &UpdateNoclipState);
    *p++ = 0x61;

    memcpy(p, kOriginalNoclipUpdateBytes, sizeof(kOriginalNoclipUpdateBytes));
    p += sizeof(kOriginalNoclipUpdateBytes);
    EmitJump(p, kPatchNoclipUpdateReturnAddress);

    if (!WriteJump(patchAddress, g_noclipUpdateCave, sizeof(kOriginalNoclipUpdateBytes))) {
        Log("Refusing noclip update patch: failed to write hook at 0x016FAACD.");
        return false;
    }
    Log("Installed noclip update hook at 0x016FAACD.");
    return true;
}

bool InstallGlobalHiddenUnlockPatch() {
    if (g_globalHiddenUnlockInstalled) {
        return true;
    }

    auto* write1Pattern = FindMainModulePattern(kGlobalHiddenUnlockWrite1Pattern, sizeof(kGlobalHiddenUnlockWrite1Pattern));
    auto* write2Pattern = FindMainModulePattern(kGlobalHiddenUnlockWrite2Pattern, sizeof(kGlobalHiddenUnlockWrite2Pattern));
    auto* visibilityPattern = FindMainModulePattern(kGlobalHiddenUnlockVisibilityPattern, sizeof(kGlobalHiddenUnlockVisibilityPattern));
    if (!write1Pattern || !write2Pattern || !visibilityPattern) {
        Log("Global Hidden Unlocks unavailable: signatures were not found.");
        return false;
    }

    g_globalHiddenUnlockWrite1Address = write1Pattern + 7;
    g_globalHiddenUnlockWrite2Address = write2Pattern + 2;
    g_globalHiddenUnlockVisibilityAddress = visibilityPattern + 6;

    if (!BytesMatch(g_globalHiddenUnlockWrite1Address,
                    kOriginalGlobalHiddenUnlockWrite1Bytes,
                    sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)) ||
        !BytesMatch(g_globalHiddenUnlockWrite2Address,
                    kOriginalGlobalHiddenUnlockWrite2Bytes,
                    sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)) ||
        !BytesMatch(g_globalHiddenUnlockVisibilityAddress,
                    kOriginalGlobalHiddenUnlockVisibilityBytes,
                    sizeof(kOriginalGlobalHiddenUnlockVisibilityBytes))) {
        Log("Global Hidden Unlocks unavailable: patch bytes are not in their original state.");
        return false;
    }

    g_globalHiddenUnlockCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_globalHiddenUnlockCave) {
        Log("VirtualAlloc failed for Global Hidden Unlocks patch.");
        return false;
    }

    std::uint8_t* p = g_globalHiddenUnlockCave;
    *p++ = 0xC6; *p++ = 0x41; *p++ = 0x38; *p++ = 0x00; // mov byte ptr [ecx+38],0
    *p++ = 0x89; *p++ = 0x51; *p++ = 0x0C; // mov [ecx+0C],edx
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite1Address + sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)));

    p = g_globalHiddenUnlockCave + 0x20;
    *p++ = 0x8B; *p++ = 0xF1; // mov esi,ecx
    *p++ = 0x57; // push edi
    *p++ = 0xC6; *p++ = 0x46; *p++ = 0x38; *p++ = 0x00; // mov byte ptr [esi+38],0
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite2Address + sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)));

    if (!WriteJump(g_globalHiddenUnlockWrite1Address,
                   g_globalHiddenUnlockCave,
                   sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)) ||
        !WriteJump(g_globalHiddenUnlockWrite2Address,
                   g_globalHiddenUnlockCave + 0x20,
                   sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)) ||
        !WriteMemory(g_globalHiddenUnlockVisibilityAddress,
                     kEnabledGlobalHiddenUnlockVisibilityBytes,
                     sizeof(kEnabledGlobalHiddenUnlockVisibilityBytes))) {
        Log("Global Hidden Unlocks failed: could not write patches.");
        return false;
    }

    g_globalHiddenUnlockInstalled = true;
    Logf("Installed Global Hidden Unlocks patches at 0x%08X, 0x%08X, visibility 0x%08X.",
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite1Address)),
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite2Address)),
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockVisibilityAddress)));
    return true;
}

bool InstallNoReloadPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoReloadAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing No Reload patch: 0x016B8A96 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalNoReloadBytes, sizeof(kOriginalNoReloadBytes))) {
        LogPatchMismatch("No Reload patch at 0x016B8A96",
                         patchAddress,
                         kOriginalNoReloadBytes,
                         sizeof(kOriginalNoReloadBytes));
        return false;
    }

    g_noReloadCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 96, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_noReloadCave) {
        Log("VirtualAlloc failed for No Reload patch.");
        return false;
    }

    std::uint8_t* p = g_noReloadCave;

    memcpy(p, kOriginalNoReloadBytes, sizeof(kOriginalNoReloadBytes));
    p += sizeof(kOriginalNoReloadBytes);

    // Keep flags exactly as the original two MOV instructions would.
    *p++ = 0x9C; // pushfd

    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noReloadHits);
    p += 4;

    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noReload);
    p += 4;
    *p++ = 0x00;

    *p++ = 0x74;
    auto* jeDone = p++;

    *p++ = 0x31;
    *p++ = 0xD2; // xor edx,edx

    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);

    *p++ = 0x9D; // popfd
    EmitJump(p, kPatchNoReloadReturnAddress);

    if (!WriteJump(patchAddress, g_noReloadCave, sizeof(kOriginalNoReloadBytes))) {
        Log("Refusing No Reload patch: failed to write hook at 0x016B8A96.");
        return false;
    }
    Log("Installed No Reload hook at 0x016B8A96.");
    return true;
}

bool InstallMissionTimerPatch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchMissionTimerAddress);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing Mission Timer patch: 0x019446C3 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalMissionTimerBytes, sizeof(kOriginalMissionTimerBytes))) {
        LogPatchMismatch("Mission Timer patch at 0x019446C3",
                         patchAddress,
                         kOriginalMissionTimerBytes,
                         sizeof(kOriginalMissionTimerBytes));
        return false;
    }

    g_missionTimerCave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 192, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_missionTimerCave) {
        Log("VirtualAlloc failed for Mission Timer patch.");
        return false;
    }

    std::uint8_t* p = g_missionTimerCave;
    *p++ = 0x9C; // pushfd
    *p++ = 0xFF; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerHits); p += 4;
    *p++ = 0x53; *p++ = 0x57; // push ebx; push edi
    *p++ = 0x85; *p++ = 0xC9; // test ecx,ecx
    *p++ = 0x74; auto* jeDone = p++;
    *p++ = 0x80; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freezeMissionTimer); p += 4; *p++ = 0;
    *p++ = 0x74; auto* jeReset = p++;
    *p++ = 0x8B; *p++ = 0x59; *p++ = 0x48; // mov ebx,[ecx+48]
    *p++ = 0x8B; *p++ = 0x79; *p++ = 0x5C; // mov edi,[ecx+5C]
    *p++ = 0x85; *p++ = 0xFF;
    *p++ = 0x74; auto* jeDone2 = p++;
    *p++ = 0x8B; *p++ = 0x3F; // mov edi,[edi]
    *p++ = 0x83; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4; *p++ = 0;
    *p++ = 0x75; auto* jneHaveDiff = p++;
    *p++ = 0x2B; *p++ = 0xDF; // sub ebx,edi
    *p++ = 0x89; *p++ = 0x1D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4;
    auto* haveDiff = p;
    *jneHaveDiff = static_cast<std::uint8_t>(haveDiff - jneHaveDiff - 1);
    *p++ = 0x03; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4;
    *p++ = 0x89; *p++ = 0x79; *p++ = 0x48; // mov [ecx+48],edi
    *p++ = 0xEB; auto* jmpDone = p++;
    auto* reset = p;
    *jeReset = static_cast<std::uint8_t>(reset - jeReset - 1);
    *p++ = 0xC7; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4; *reinterpret_cast<std::uint32_t*>(p) = 0; p += 4;
    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);
    *jeDone2 = static_cast<std::uint8_t>(done - jeDone2 - 1);
    *jmpDone = static_cast<std::uint8_t>(done - jmpDone - 1);
    *p++ = 0x5F; *p++ = 0x5B; // pop edi; pop ebx
    *p++ = 0x9D; // popfd
    memcpy(p, kOriginalMissionTimerBytes, sizeof(kOriginalMissionTimerBytes));
    p += sizeof(kOriginalMissionTimerBytes);
    EmitJump(p, kPatchMissionTimerReturnAddress);

    if (!WriteJump(patchAddress, g_missionTimerCave, sizeof(kOriginalMissionTimerBytes))) {
        Log("Refusing Mission Timer patch: failed to write hook at 0x019446C3.");
        return false;
    }
    Log("Installed Mission Timer hook at 0x019446C3.");
    return true;
}

bool InstallMissionTimer2Patch() {
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchMissionTimer2Address);
    if (patchAddress[0] == 0xE9) {
        Log("Refusing Mission Timer II patch: 0x016869B7 is already hooked by another tool.");
        return false;
    }
    if (!BytesMatch(patchAddress, kOriginalMissionTimer2Bytes, sizeof(kOriginalMissionTimer2Bytes))) {
        LogPatchMismatch("Mission Timer II patch at 0x016869B7",
                         patchAddress,
                         kOriginalMissionTimer2Bytes,
                         sizeof(kOriginalMissionTimer2Bytes));
        return false;
    }

    g_missionTimer2Cave = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, 192, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_missionTimer2Cave) {
        Log("VirtualAlloc failed for Mission Timer II patch.");
        return false;
    }

    std::uint8_t* p = g_missionTimer2Cave;
    *p++ = 0x9C; // pushfd
    *p++ = 0xFF; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Hits); p += 4;
    *p++ = 0x53; *p++ = 0x57;
    *p++ = 0x85; *p++ = 0xF6; // test esi,esi
    *p++ = 0x74; auto* jeDone = p++;
    *p++ = 0x80; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freezeMissionTimer); p += 4; *p++ = 0;
    *p++ = 0x74; auto* jeReset = p++;
    *p++ = 0x8B; *p++ = 0x9E; *reinterpret_cast<std::uint32_t*>(p) = 0x00000110; p += 4;
    *p++ = 0x8B; *p++ = 0xBE; *reinterpret_cast<std::uint32_t*>(p) = 0x00000124; p += 4;
    *p++ = 0x85; *p++ = 0xFF;
    *p++ = 0x74; auto* jeDone2 = p++;
    *p++ = 0x8B; *p++ = 0x3F;
    *p++ = 0x83; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4; *p++ = 0;
    *p++ = 0x75; auto* jneHaveDiff = p++;
    *p++ = 0x2B; *p++ = 0xDF;
    *p++ = 0x89; *p++ = 0x1D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4;
    auto* haveDiff = p;
    *jneHaveDiff = static_cast<std::uint8_t>(haveDiff - jneHaveDiff - 1);
    *p++ = 0x03; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4;
    *p++ = 0x89; *p++ = 0xBE; *reinterpret_cast<std::uint32_t*>(p) = 0x00000110; p += 4;
    *p++ = 0xEB; auto* jmpDone = p++;
    auto* reset = p;
    *jeReset = static_cast<std::uint8_t>(reset - jeReset - 1);
    *p++ = 0xC7; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4; *reinterpret_cast<std::uint32_t*>(p) = 0; p += 4;
    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);
    *jeDone2 = static_cast<std::uint8_t>(done - jeDone2 - 1);
    *jmpDone = static_cast<std::uint8_t>(done - jmpDone - 1);
    *p++ = 0x5F; *p++ = 0x5B;
    *p++ = 0x9D; // popfd
    memcpy(p, kOriginalMissionTimer2Bytes, sizeof(kOriginalMissionTimer2Bytes));
    p += sizeof(kOriginalMissionTimer2Bytes);
    EmitJump(p, kPatchMissionTimer2ReturnAddress);

    if (!WriteJump(patchAddress, g_missionTimer2Cave, sizeof(kOriginalMissionTimer2Bytes))) {
        Log("Refusing Mission Timer II patch: failed to write hook at 0x016869B7.");
        return false;
    }
    Log("Installed Mission Timer II hook at 0x016869B7.");
    return true;
}

bool InstallPatches() {
    g_shipPatchReady = InstallHealthPatch();
    g_noCannonCooldownPatchReady = InstallNoCannonCooldownPatch();
    g_allyGodmodePatchReady = InstallAllyGodmodePatch();
    g_timeScalePatchReady = InstallTimeIntervalPatch();
    g_playerHealthPatchReady = InstallPlayerHealthPatch();
    g_infiniteBreathPatchReady = InstallInfiniteBreathPatch();
    g_stealthModePatchReady = InstallStealthModePatch();
    g_noReloadPatchReady = InstallNoReloadPatch();
    g_missionTimerPatchReady = InstallMissionTimerPatch();
    g_missionTimer2PatchReady = InstallMissionTimer2Patch();
    g_missionTimersPatchReady = g_missionTimerPatchReady || g_missionTimer2PatchReady;
    g_inventoryPointerPatchReady = InstallInventoryPointerPatch();
    g_inventorySetPatchReady = InstallInventorySetPatch();
    g_inventorySetAltPatchReady = InstallInventorySetAltPatch();
    g_inventoryEntrySubtractPatchReady = InstallInventoryEntrySubtractPatch();
    g_inventoryPatchReady = g_inventoryPointerPatchReady ||
                            g_inventorySetPatchReady ||
                            g_inventorySetAltPatchReady ||
                            g_inventoryEntrySubtractPatchReady;
    g_noclipPatchReady = InstallNoclipUpdatePatch();

    if (!g_shipPatchReady) {
        g_enabled = false;
    }
    if (!g_noCannonCooldownPatchReady) {
        g_noCannonCooldown = false;
    }
    if (!g_allyGodmodePatchReady) {
        g_allyGodmode = false;
    }
    if (!g_timeScalePatchReady) {
        g_timeScaleEnabled = false;
    }
    if (!g_playerHealthPatchReady) {
        g_playerGodmode = false;
    }
    if (!g_infiniteBreathPatchReady) {
        g_infiniteBreath = false;
    }
    if (!g_stealthModePatchReady) {
        g_stealthMode = false;
    }
    if (!g_noReloadPatchReady) {
        g_noReload = false;
    }
    if (!g_missionTimersPatchReady) {
        g_freezeMissionTimer = false;
    }
    if (!g_inventoryPatchReady) {
        g_infiniteMoney = false;
        g_infiniteSugar = false;
        g_infiniteRum = false;
        g_infiniteWood = false;
        g_infiniteMetal = false;
        g_infiniteCloth = false;
        g_infiniteSmokeBombs = false;
        g_infiniteBullets = false;
        g_infiniteSleepDarts = false;
        g_infiniteBerserkDarts = false;
        g_infiniteRopeDarts = false;
        g_infiniteHarpoons = false;
        g_infiniteThrowingKnives = false;
    }
    if (!g_noclipPatchReady) {
        g_noclipEnabled = false;
    }

    if (!g_shipPatchReady &&
        !g_noCannonCooldownPatchReady &&
        !g_allyGodmodePatchReady &&
        !g_timeScalePatchReady &&
        !g_playerHealthPatchReady &&
        !g_infiniteBreathPatchReady &&
        !g_stealthModePatchReady &&
        !g_noReloadPatchReady &&
        !g_missionTimersPatchReady &&
        !g_inventoryPatchReady &&
        !g_noclipPatchReady) {
        Log("No gameplay patches installed; UI will load with features unavailable.");
        return false;
    }
    g_installed = true;
    Logf("AC4Tools standalone patches installed: ship=%d cannonCooldown=%d allyGodmode=%d timescale=%d player=%d breath=%d stealth=%d noReload=%d timers=%d inventory=%d noclip=%d.",
         g_shipPatchReady ? 1 : 0,
         g_noCannonCooldownPatchReady ? 1 : 0,
         g_allyGodmodePatchReady ? 1 : 0,
         g_timeScalePatchReady ? 1 : 0,
         g_playerHealthPatchReady ? 1 : 0,
         g_infiniteBreathPatchReady ? 1 : 0,
         g_stealthModePatchReady ? 1 : 0,
         g_noReloadPatchReady ? 1 : 0,
         g_missionTimersPatchReady ? 1 : 0,
         g_inventoryPatchReady ? 1 : 0,
         g_noclipPatchReady ? 1 : 0);
    return true;
}

void ApplyRedStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(8.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.52f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.05f, 0.05f, 0.88f);
    colors[ImGuiCol_Border] = ImVec4(0.78f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.10f, 0.13f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.65f, 0.12f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.34f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.55f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.10f, 0.16f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.09f, 0.11f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.13f, 0.18f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.32f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.48f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.72f, 0.13f, 0.18f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.32f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.62f, 0.13f, 0.18f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.48f, 0.12f, 0.16f, 1.00f);
}

void CreateRenderTarget(IDXGISwapChain* swapChain) {
    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
        g_device->CreateRenderTargetView(backBuffer, nullptr, &g_renderTarget);
        backBuffer->Release();
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_ACTIVATEAPP || msg == WM_KILLFOCUS || msg == WM_CANCELMODE) {
        if (wparam == FALSE || msg != WM_ACTIVATEAPP) {
            g_menuOpen = false;
            ReleaseMenuInputState();
        }
    }

    if (g_menuOpen) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
        if (g_hotkeyCaptureAction != -1 &&
            (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)) {
            const int vk = static_cast<int>(wparam);
            if (vk == VK_ESCAPE) {
                g_hotkeyCaptureAction = -1;
                Log("Hotkey capture cancelled.");
            } else if (vk == VK_BACK || vk == VK_DELETE) {
                if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                    AssignMenuHotkey(0);
                } else {
                    AssignHotkey(g_hotkeyCaptureAction, 0);
                }
                g_hotkeyCaptureAction = -1;
            } else if (IsBindableHotkey(vk)) {
                if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                    AssignMenuHotkey(vk);
                } else {
                    AssignHotkey(g_hotkeyCaptureAction, vk);
                }
                g_hotkeyCaptureAction = -1;
            }
            return true;
        }
        ImGuiIO& io = ImGui::GetIO();
        if (IsMouseInputCaptureActive() && io.WantCaptureMouse &&
            (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP ||
             msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP ||
             msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP ||
             msg == WM_MOUSEMOVE || msg == WM_MOUSEWHEEL ||
             msg == WM_MOUSEHWHEEL)) {
            return true;
        }
        if (IsKeyboardInputCaptureActive() &&
            (msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR ||
             msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP ||
             msg == WM_SYSCHAR || msg == WM_DEADCHAR || msg == WM_SYSDEADCHAR)) {
            return true;
        }
        if (IsMouseInputCaptureActive() && msg == WM_SETCURSOR) {
            SetCursor(nullptr);
            return true;
        }
        if ((IsMouseInputCaptureActive() || IsKeyboardInputCaptureActive()) &&
            (msg == WM_INPUT || msg == WM_CAPTURECHANGED)) {
            return true;
        }
    }
    return CallWindowProcA(g_originalWndProc, hwnd, msg, wparam, lparam);
}

bool InitImGui(IDXGISwapChain* swapChain) {
    if (FAILED(swapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&g_device)))) {
        return false;
    }
    InitConsole();
    g_device->GetImmediateContext(&g_context);

    DXGI_SWAP_CHAIN_DESC desc{};
    swapChain->GetDesc(&desc);
    g_gameWindow = desc.OutputWindow;

    CreateRenderTarget(swapChain);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigWindowsMoveFromTitleBarOnly = false;
    ApplyRedStyle();
    ImGui_ImplWin32_Init(g_gameWindow);
    ImGui_ImplDX11_Init(g_device, g_context);

    g_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(g_gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
    g_imguiReady = true;
    Log("Dear ImGui DX11 overlay initialized.");
    Log(g_installed ? "AC4Tools patches installed." : "AC4Tools patches not installed.");
    return true;
}

void DrawHotkeysTab() {
    if (ImGui::BeginTable("HotkeyTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Hotkey", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Set", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("Open/Close Menu");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted("Menu");
        ImGui::TableSetColumnIndex(2);
        if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
            ImGui::TextUnformatted("Press key...");
        } else {
            ImGui::TextUnformatted(HotkeyName(g_menuHotkey));
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::PushID("MenuOpen");
        if (ImGui::Button(g_hotkeyCaptureAction == kMenuHotkeyCapture ? "Cancel" : "Set", ImVec2(64.0f, 0.0f))) {
            if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                g_hotkeyCaptureAction = -1;
            } else {
                g_hotkeyCaptureAction = kMenuHotkeyCapture;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("None", ImVec2(64.0f, 0.0f))) {
            AssignMenuHotkey(0);
            if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                g_hotkeyCaptureAction = -1;
            }
        }
        ImGui::PopID();

        for (int i = 0; i < kActionCount; ++i) {
            ToggleAction& action = g_actions[i];
            const bool available = action.ready && *action.ready;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if (available) {
                ImGui::TextUnformatted(action.label);
            } else {
                ImGui::TextDisabled("%s", action.label);
            }

            ImGui::TableSetColumnIndex(1);
            if (!available) {
                ImGui::TextDisabled("Unavailable");
            } else {
                ImGui::TextUnformatted(*action.value ? "On" : "Off");
            }

            ImGui::TableSetColumnIndex(2);
            if (g_hotkeyCaptureAction == i) {
                ImGui::TextUnformatted("Press key...");
            } else {
                ImGui::TextUnformatted(HotkeyName(action.hotkey));
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::PushID(i);
            if (ImGui::Button(g_hotkeyCaptureAction == i ? "Cancel" : "Set", ImVec2(64.0f, 0.0f))) {
                if (g_hotkeyCaptureAction == i) {
                    g_hotkeyCaptureAction = -1;
                } else {
                    g_hotkeyCaptureAction = i;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("None", ImVec2(64.0f, 0.0f))) {
                AssignHotkey(i, 0);
                if (g_hotkeyCaptureAction == i) {
                    g_hotkeyCaptureAction = -1;
                }
            }
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void DrawInfoRow(const char* label, const char* value) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(label);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(value);
}

void DrawInfoRowf(const char* label, const char* format, ...) {
    char value[256]{};
    va_list args;
    va_start(args, format);
    vsprintf_s(value, format, args);
    va_end(args);
    DrawInfoRow(label, value);
}

void DrawPatchRow(const char* label, bool ready, const char* address, long hits = -1) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(label);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(address);
    ImGui::TableSetColumnIndex(2);
    ImGui::TextUnformatted(ready ? "ready" : "unavailable");
    ImGui::TableSetColumnIndex(3);
    if (hits >= 0) {
        ImGui::Text("%ld", hits);
    } else {
        ImGui::TextUnformatted("-");
    }
}

float ClampFloat(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

ImVec2 ClampMenuPosToGameWindow(ImVec2 pos, ImVec2 windowSize) {
    if (!g_gameWindow) {
        return pos;
    }

    RECT clientRect{};
    if (!GetClientRect(g_gameWindow, &clientRect)) {
        return pos;
    }

    const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);
    const float maxX = clientWidth > windowSize.x ? clientWidth - windowSize.x : 0.0f;
    const float maxY = clientHeight > windowSize.y ? clientHeight - windowSize.y : 0.0f;

    pos.x = ClampFloat(pos.x, 0.0f, maxX);
    pos.y = ClampFloat(pos.y, 0.0f, maxY);
    return pos;
}

ImVec2 GameWindowClientSize() {
    if (!g_gameWindow) {
        return ImVec2(0.0f, 0.0f);
    }

    RECT clientRect{};
    if (!GetClientRect(g_gameWindow, &clientRect)) {
        return ImVec2(0.0f, 0.0f);
    }

    return ImVec2(static_cast<float>(clientRect.right - clientRect.left),
                  static_cast<float>(clientRect.bottom - clientRect.top));
}

void DrawMenu() {
    const ImVec2 clientSize = GameWindowClientSize();
    if (clientSize.x > 0.0f && clientSize.y > 0.0f) {
        ImVec2 minSize(320.0f, 240.0f);
        minSize.x = clientSize.x < minSize.x ? clientSize.x : minSize.x;
        minSize.y = clientSize.y < minSize.y ? clientSize.y : minSize.y;
        g_menuSize.x = ClampFloat(g_menuSize.x, minSize.x, clientSize.x);
        g_menuSize.y = ClampFloat(g_menuSize.y, minSize.y, clientSize.y);
        ImGui::SetNextWindowSizeConstraints(minSize, clientSize);
    }

    ImGui::SetNextWindowPos(g_menuPos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(g_menuSize, ImGuiCond_Appearing);
    if (!ImGui::Begin(kToolTitle,
                      &g_menuOpen,
                      ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImVec2 currentMenuPos = ImGui::GetWindowPos();
    const ImVec2 currentMenuSize = ImGui::GetWindowSize();
    const ImVec2 clampedMenuPos = ClampMenuPosToGameWindow(currentMenuPos, currentMenuSize);
    if (clampedMenuPos.x != currentMenuPos.x || clampedMenuPos.y != currentMenuPos.y) {
        ImGui::SetWindowPos(clampedMenuPos);
        currentMenuPos = clampedMenuPos;
    }
    if (currentMenuPos.x != g_menuPos.x || currentMenuPos.y != g_menuPos.y) {
        g_menuPos = currentMenuPos;
        g_menuPosDirty = true;
    }
    if (currentMenuSize.x != g_menuSize.x || currentMenuSize.y != g_menuSize.y) {
        g_menuSize = currentMenuSize;
        g_menuSizeDirty = true;
    }
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && (g_menuPosDirty || g_menuSizeDirty)) {
        SaveConfig();
        g_menuPosDirty = false;
        g_menuSizeDirty = false;
    }

    if (ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Ship")) {
            if (!g_shipPatchReady) {
                ImGui::TextDisabled("Ship Godmode unavailable: hook was not installed.");
            } else {
                bool value = g_enabled;
                if (ImGui::Checkbox("Ship Godmode", &value)) {
                    g_enabled = value;
                    Log(g_enabled ? "Ship Godmode toggled ON from ImGui." :
                                    "Ship Godmode toggled OFF from ImGui.");
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "If it does not apply right away, leave the wheel and take control again.\n"
                        "Turning it off leaves the current ship state as-is.");
                }
            }
            if (!g_inventoryPatchReady) {
                ImGui::TextDisabled("Infinite Ship Crew unavailable: inventory hook was not installed.");
            } else {
                bool value = g_infiniteShipCrew;
                if (ImGui::Checkbox("Infinite Ship Crew", &value)) {
                    g_infiniteShipCrew = value;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Keeps ship crew at 40 while enabled.\n"
                        "If it does not apply right away, leave the wheel and take control again.\n"
                        "Turning it off leaves the current crew value as-is.");
                }
                value = g_infiniteMortarShotAmmo;
                if (ImGui::Checkbox("Infinite Mortar Shot Ammo", &value)) {
                    g_infiniteMortarShotAmmo = value;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Keeps mortar shot ammo at 15 while enabled.\n"
                        "If it does not apply right away, leave the wheel and take control again.\n"
                        "Turning it off leaves the current value as-is.");
                }
                value = g_infiniteHeavyShotAmmo;
                if (ImGui::Checkbox("Infinite Heavy Shot Ammo", &value)) {
                    g_infiniteHeavyShotAmmo = value;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Keeps heavy shot ammo at 25 while enabled.\n"
                        "If it does not apply right away, leave the wheel and take control again.\n"
                        "Turning it off leaves the current value as-is.");
                }
                value = g_infiniteFireBarrels;
                if (ImGui::Checkbox("Infinite Fire Barrels", &value)) {
                    g_infiniteFireBarrels = value;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Keeps fire barrels at 25 while enabled.\n"
                        "If it does not apply right away, leave the wheel and take control again.\n"
                        "Turning it off leaves the current value as-is.");
                }
            }
            if (!g_noCannonCooldownPatchReady) {
                ImGui::TextDisabled("No Cannon Cooldown unavailable: byte patch was not installed.");
            } else {
                bool value = g_noCannonCooldown;
                if (ImGui::Checkbox("No Cannon Cooldown", &value)) {
                    g_noCannonCooldown = value;
                    ApplyNoCannonCooldownPatch();
                    Log(g_noCannonCooldown ? "No Cannon Cooldown toggled ON from ImGui." :
                                              "No Cannon Cooldown toggled OFF from ImGui.");
                }
            }
            if (!g_allyGodmodePatchReady) {
                ImGui::TextDisabled("Ally Godmode unavailable: hook was not installed.");
            } else {
                bool value = g_allyGodmode;
                if (ImGui::Checkbox("Ally Godmode", &value)) {
                    g_allyGodmode = value;
                    Log(g_allyGodmode ? "Ally Godmode toggled ON from ImGui." :
                                        "Ally Godmode toggled OFF from ImGui.");
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Prevents all allies from taking damage, not just allied ships.\n"
                        "Use this when you want friendly units to stay protected.");
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Player")) {
            if (!g_playerHealthPatchReady && !g_infiniteBreathPatchReady && !g_noReloadPatchReady && !g_inventoryPatchReady) {
                ImGui::TextDisabled("Player options unavailable: hooks were not installed.");
            } else {
                ImGui::Columns(3, "PlayerColumns", false);

                if (g_playerHealthPatchReady) {
                    ImGui::Checkbox("God Mode", &g_playerGodmode);
                } else {
                    ImGui::TextDisabled("God Mode unavailable");
                }
                if (g_inventoryPatchReady) {
                    ImGui::Checkbox("Infinite Money", &g_infiniteMoney);
                } else {
                    ImGui::TextDisabled("Infinite Money unavailable");
                }
                if (g_infiniteBreathPatchReady) {
                    ImGui::Checkbox("Infinite Breath", &g_infiniteBreath);
                } else {
                    ImGui::TextDisabled("Infinite Breath unavailable");
                }
                if (g_stealthModePatchReady) {
                    bool value = g_stealthMode;
                    if (ImGui::Checkbox("Stealth Mode", &value)) {
                        g_stealthMode = value;
                        ApplyStealthModePatch();
                        Log(g_stealthMode ? "Stealth Mode toggled ON from ImGui." :
                                             "Stealth Mode toggled OFF from ImGui.");
                    }
                } else {
                    ImGui::TextDisabled("Stealth Mode unavailable");
                }
                if (g_noReloadPatchReady) {
                    ImGui::Checkbox("No Reload", &g_noReload);
                } else {
                    ImGui::TextDisabled("No Reload unavailable");
                }

                ImGui::NextColumn();
                if (g_inventoryPatchReady) {
                    ImGui::Checkbox("Infinite Bullets", &g_infiniteBullets);
                    ImGui::Checkbox("Infinite Smokebombs", &g_infiniteSmokeBombs);
                    ImGui::Checkbox("Infinite Sleep Darts", &g_infiniteSleepDarts);
                    ImGui::Checkbox("Infinite Berserk Darts", &g_infiniteBerserkDarts);
                    ImGui::Checkbox("Infinite Throwing Knives", &g_infiniteThrowingKnives);
                    ImGui::Checkbox("Infinite Harpoons", &g_infiniteHarpoons);
                    ImGui::Checkbox("Infinite Rope Darts", &g_infiniteRopeDarts);
                }

                ImGui::NextColumn();
                if (g_inventoryPatchReady) {
                    ImGui::Checkbox("Infinite Sugar", &g_infiniteSugar);
                    ImGui::Checkbox("Infinite Rum", &g_infiniteRum);
                    ImGui::Checkbox("Infinite Wood", &g_infiniteWood);
                    ImGui::Checkbox("Infinite Cloth", &g_infiniteCloth);
                    ImGui::Checkbox("Infinite Metal", &g_infiniteMetal);
                }

                ImGui::Columns(1);
                if (!g_inventoryPatchReady) {
                    ImGui::TextDisabled("Infinite item options unavailable: hooks were not installed.");
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Noclip")) {
            if (!g_noclipPatchReady) {
                ImGui::TextDisabled("Noclip unavailable: hook was not installed.");
            } else {
                bool noclip = g_noclipEnabled;
                if (ImGui::Checkbox("Enable Noclip", &noclip)) {
                    g_noclipEnabled = noclip;
                    UpdateNoclipState();
                    Log(g_noclipEnabled ? "Noclip requested ON from ImGui." :
                                          "Noclip requested OFF from ImGui.");
                }
            }

            float speed = g_noclipSpeed;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Speed", &speed, 0.001f, 0.0001f, 1000.0f, "%.6f")) {
                if (speed < 0.0001f) {
                    speed = 0.0001f;
                }
                if (speed > 1000.0f) {
                    speed = 1000.0f;
                }
                g_noclipSpeed = speed;
                SaveConfig();
            }

            float boost = g_noclipBoostSpeed;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Boost Speed", &boost, 0.001f, 0.0001f, 1000.0f, "%.6f")) {
                if (boost < 0.0001f) {
                    boost = 0.0001f;
                }
                if (boost > 1000.0f) {
                    boost = 1000.0f;
                }
                g_noclipBoostSpeed = boost;
                SaveConfig();
            }

            ImGui::Separator();
            ImGui::Text("Game noclip state: %s", g_noclipActive ? "active" : "inactive");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Game")) {
            if (!g_timeScalePatchReady) {
                ImGui::TextDisabled("Time Scale unavailable: hook was not installed.");
            } else {
                bool enabled = g_timeScaleEnabled;
                ImGui::Checkbox("##TimeScaleEnabled", &enabled);
                if (ImGui::IsItemDeactivatedAfterEdit() || enabled != g_timeScaleEnabled) {
                    g_timeScaleEnabled = enabled;
                    UpdateTimeScaleInterval();
                    SaveConfig();
                    Logf("TimeScale %s value=%.6f interval=%d hits=%ld",
                         g_timeScaleEnabled ? "ON" : "OFF",
                         g_timeScale,
                         g_timeScaleInterval,
                         g_timeIntervalHits);
                }
                ImGui::SameLine();
            }

            float scale = g_timeScale;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Time Scale", &scale, 0.001f, 0.0001f, 100.0f, "%.6f")) {
                if (scale < 0.0001f) {
                    scale = 0.0001f;
                }
                if (scale > 100.0f) {
                    scale = 100.0f;
                }
                g_timeScale = scale;
                UpdateTimeScaleInterval();
                SaveConfig();
                Logf("TimeScale value changed to %.6f interval=%d hits=%ld",
                     g_timeScale,
                     g_timeScaleInterval,
                     g_timeIntervalHits);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Click and drag to edit value.\n"
                    "Hold SHIFT/ALT for faster/slower edit.\n"
                    "Double-click or CTRL+click to input value.");
            }
            if (g_missionTimersPatchReady) {
                ImGui::Checkbox("Freeze Mission Timer", &g_freezeMissionTimer);
            } else {
                ImGui::TextDisabled("Freeze Mission Timer unavailable: hooks were not installed.");
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Unlocks")) {
            ImGui::TextDisabled("Best used from the ship cabin, where weapon/outfit records are loaded.");
            ImGui::TextDisabled("Back up your save first: unlocks can become permanent/irreversible once the game saves.");
            ImGui::TextDisabled("Unlocks can softlock saves if something was meant to unlock later through progression.");
            ImGui::TextDisabled("Per-item unlocks auto-uncheck after success and stay unlocked for this game session.");
            ImGui::Separator();

            if (g_globalHiddenUnlockInstalled) {
                ImGui::TextDisabled("Global Hidden Unlocks installed for this session.");
            } else if (ImGui::Button("Install Global Hidden Unlocks")) {
                InstallGlobalHiddenUnlockPatch();
            }
            ImGui::TextDisabled("Use at the main menu before loading a save; can be saved permanently by the game.");
            ImGui::TextDisabled("Recommended only with a save backup.");
            ImGui::Checkbox("Finish Community Challenges if needed for certain unlocks", &g_finishCommunityChallengesForUnlocks);
            ImGui::TextDisabled("Only affects entries that require community completion (CC); normal unlocks ignore this.");
            ImGui::Separator();

            auto drawUnlockCategory = [](const char* label, UnlockCategory category) {
                ImGui::TextUnformatted(label);
                if (ImGui::BeginTable(label, 2, ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("Item", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    for (int i = 0; i < kUnlockEntryCount; ++i) {
                        UnlockEntry& entry = g_unlockEntries[i];
                        if (entry.category != category) {
                            continue;
                        }
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        bool enabled = entry.enabled;
                        ImGui::PushID(i);
                        if (ImGui::Checkbox(entry.name, &enabled)) {
                            entry.enabled = enabled;
                            entry.found = false;
                            entry.patched = false;
                            g_unlockPistolsLastScan = 0;
                            if (entry.enabled) {
                                InterlockedExchange(&g_unlockScanRequested, 1);
                            }
                            UpdateUnlockCounters();
                            Logf("%s unlock %s.", entry.name, entry.enabled ? "queued" : "cancelled");
                        }
                        ImGui::PopID();
                        ImGui::TableSetColumnIndex(1);
                        if (entry.patched) {
                            ImGui::TextDisabled(entry.enabled ? "unlocked" : "done");
                        } else if (entry.found) {
                            ImGui::TextDisabled("found");
                        } else if (entry.enabled) {
                            ImGui::TextDisabled("waiting");
                        } else {
                            ImGui::TextDisabled("ready");
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::Spacing();
            };

            drawUnlockCategory("Pistols", UnlockCategory::Pistols);
            drawUnlockCategory("Swords", UnlockCategory::Swords);
            drawUnlockCategory("Outfits", UnlockCategory::Outfits);
            drawUnlockCategory("Ship Cosmetics", UnlockCategory::ShipCosmetics);
            drawUnlockCategory("Elite Unlocks", UnlockCategory::EliteUnlocks);

            ImGui::Spacing();
            const int enabledCount = CountEnabledUnlocks();
            ImGui::Text("Pending: %d", enabledCount);
            ImGui::Text("Done this session: %d", g_unlockPistolsPatched);
            if (g_unlockScanRunning != 0) {
                ImGui::TextDisabled("Scan running in background...");
            }
            if (enabledCount > 0) {
                ImGui::TextDisabled("If an item stays waiting, go to the ship cabin and keep it checked.");
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Hotkeys")) {
            DrawHotkeysTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("System")) {
            if (ImGui::Checkbox("Lock Mouse to Window", &g_lockMouseToWindow)) {
                SaveConfig();
                UpdateMouseWindowLock();
                Log(g_lockMouseToWindow ? "Lock Mouse to Window enabled." :
                                           "Lock Mouse to Window disabled.");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Keeps the Windows cursor inside the game window while AC4 is focused.\n"
                    "Releases automatically when AC4 loses focus, such as when you alt-tab.");
            }
            if (ImGui::Checkbox("Disable Mouse Input while UI is open", &g_disableMouseInputWhenUiOpen)) {
                SaveConfig();
                RefreshInputCaptureState();
                Log(g_disableMouseInputWhenUiOpen ? "Disable Mouse Input while UI is open enabled." :
                                                    "Disable Mouse Input while UI is open disabled.");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Blocks mouse input from reaching the game while the AC4Tools UI is open.\n"
                    "This affects clicks and mouse look only while the menu is visible.");
            }
            if (ImGui::Checkbox("Disable Keyboard Input while UI is open", &g_disableKeyboardInputWhenUiOpen)) {
                SaveConfig();
                RefreshInputCaptureState();
                Log(g_disableKeyboardInputWhenUiOpen ? "Disable Keyboard Input while UI is open enabled." :
                                                       "Disable Keyboard Input while UI is open disabled.");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Blocks keyboard input from reaching the game while the AC4Tools UI is open.\n"
                    "Use this if you do not want movement or actions to trigger behind the menu.");
            }
            if (ImGui::Checkbox("Disable Hotkeys while UI is open", &g_disableHotkeysWhileUiOpen)) {
                SaveConfig();
                Log(g_disableHotkeysWhileUiOpen ? "Disable Hotkeys while UI is open enabled." :
                                                  "Disable Hotkeys while UI is open disabled.");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Stops AC4Tools feature hotkeys from firing while the AC4Tools UI is open.\n"
                    "The menu open/close hotkey still works so you can always close the UI.");
            }
            ImGui::Separator();

            if (ImGui::BeginTable("SystemInfoTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 160.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                DrawInfoRow("Tool", kToolTitle);
                DrawInfoRow("Menu hotkey", HotkeyName(g_menuHotkey));
                DrawInfoRow("Config folder", g_moduleDir);
                DrawInfoRow("Game executable", g_gameExeName[0] ? g_gameExeName : "unknown");
                DrawInfoRow("Game path", g_gameExePath[0] ? g_gameExePath : "unknown");
                DrawInfoRow("Game size", g_gameExeSizeText);
                DrawInfoRow("Game timestamp", g_gameExeTimestampText);
                DrawInfoRow("Supported exe", kSupportedGameExe);
                DrawInfoRow("Supported size", kSupportedGameSize);
                DrawInfoRow("Supported timestamp", kSupportedGameTimestamp);
                DrawInfoRow("Supported SHA256", kSupportedGameSha256);
                DrawInfoRow("Console logging", g_consoleLoggingEnabled ? "enabled" : "disabled");
                DrawInfoRow("File logging", g_fileLoggingEnabled ? "enabled" : "disabled");
                DrawInfoRow("Mouse window lock", g_lockMouseToWindow ? "enabled" : "disabled");
                DrawInfoRow("Disable mouse input on UI open", g_disableMouseInputWhenUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Disable keyboard input on UI open", g_disableKeyboardInputWhenUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Disable hotkeys on UI open", g_disableHotkeysWhileUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Input captured", g_inputCaptured ? "yes" : "no");
                DrawInfoRowf("Inventory base", "0x%08X", static_cast<unsigned int>(g_inventoryBase));
                DrawInfoRowf("Inventory pointer writes", "%d", g_inventoryPointerLastWrites);
                DrawInfoRowf("Unlock entries", "%d total", kUnlockEntryCount);
                DrawInfoRowf("Unlock queue", "%d pending", CountEnabledUnlocks());
                DrawInfoRowf("Unlocks done this session", "%d", g_unlockPistolsPatched);
                DrawInfoRowf("Unlock records found", "%d last pass", g_unlockPistolsFound);
                DrawInfoRow("Unlock scan", g_unlockScanRunning != 0 ? "running" : "idle");
                DrawInfoRow("Community completion", g_finishCommunityChallengesForUnlocks ? "enabled" : "disabled");
                DrawInfoRow("Global hidden unlocks", g_globalHiddenUnlockInstalled ? "installed" : "not installed");
                DrawInfoRowf("TimeScale interval", "%d", g_timeScaleInterval);
                DrawInfoRowf("Last inventory item", "id=0x%02X original=%d applied=%d",
                             g_inventoryLastItemId,
                             g_inventoryLastOriginalValue,
                             g_inventoryLastAppliedValue);
                ImGui::EndTable();
            }

            ImGui::Spacing();
            if (ImGui::BeginTable("PatchStatusTable",
                                  4,
                                  ImGuiTableFlags_BordersInnerV |
                                      ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableSetupColumn("Patch");
                ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 92.0f);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 88.0f);
                ImGui::TableSetupColumn("Hits", ImGuiTableColumnFlags_WidthFixed, 56.0f);
                ImGui::TableHeadersRow();
                DrawPatchRow("Ship Godmode", g_shipPatchReady, "0x01225C3F");
                DrawPatchRow("No Cannon Cooldown", g_noCannonCooldownPatchReady, "0x01C97031");
                DrawPatchRow("Ally Godmode", g_allyGodmodePatchReady, "0x01BFF520");
                DrawPatchRow("Time Scale", g_timeScalePatchReady, "0x00A047EA", g_timeIntervalHits);
                DrawPatchRow("Player Godmode", g_playerHealthPatchReady, "0x0115AD07", g_playerHealthHits);
                DrawPatchRow("Infinite Breath", g_infiniteBreathPatchReady, "0x01148936", g_infiniteBreathHits);
                DrawPatchRow("Stealth Mode", g_stealthModePatchReady, "0x0101CB66");
                DrawPatchRow("No Reload", g_noReloadPatchReady, "0x016B8A96", g_noReloadHits);
                DrawPatchRow("Mission Timer", g_missionTimerPatchReady, "0x019446C3", g_missionTimerHits);
                DrawPatchRow("Mission Timer II", g_missionTimer2PatchReady, "0x016869B7", g_missionTimer2Hits);
                DrawPatchRow("Inventory Pointer", g_inventoryPointerPatchReady, "0x01CFD381", g_inventoryPointerHits);
                DrawPatchRow("Inventory Set", g_inventorySetPatchReady, "0x011A1F3D", g_inventorySetHits);
                DrawPatchRow("Inventory Set Alt", g_inventorySetAltPatchReady, "0x011A1F6F", g_inventorySetAltHits);
                DrawPatchRow("Inventory Subtract", g_inventoryEntrySubtractPatchReady, "0x011A1FA3", g_inventoryEntrySubtractHits);
                DrawPatchRow("Noclip Update", g_noclipPatchReady, "0x016FAACD", g_noclipUpdateHits);
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

HRESULT __stdcall HookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
    if (!g_imguiReady) {
        InitImGui(swapChain);
    }

    static bool menuWasDown = false;
    const bool menuIsDown =
        g_menuHotkey != 0 &&
        g_suppressedHotkeyVk != g_menuHotkey &&
        (QueryPhysicalKeyState(g_menuHotkey) & 0x8000) != 0;
    if (menuIsDown && !menuWasDown) {
        g_menuOpen = !g_menuOpen;
        RefreshInputCaptureState();
        if (!g_menuOpen) {
            ReleaseMenuInputState();
        }
    }
    menuWasDown = menuIsDown;
    if (g_suppressedHotkeyVk != 0 && (QueryPhysicalKeyState(g_suppressedHotkeyVk) & 0x8000) == 0) {
        g_suppressedHotkeyVk = 0;
    }

    if (g_imguiReady) {
        if (g_menuOpen && GetForegroundWindow() != g_gameWindow) {
            g_menuOpen = false;
            ReleaseMenuInputState();
        }
        UpdateMouseWindowLock();
        MaintainInventoryPointerValues();
        ProcessHotkeys();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = g_menuOpen;
        io.MouseDown[0] = g_menuOpen && ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
        io.MouseDown[1] = g_menuOpen && ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
        io.MouseDown[2] = g_menuOpen && ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
        ImGui::NewFrame();

        if (g_menuOpen) {
            DrawMenu();
        }

        ImGui::Render();
        g_context->OMSetRenderTargets(1, &g_renderTarget, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return g_originalPresent(swapChain, syncInterval, flags);
}

LRESULT CALLBACK DummyWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

bool InstallDx11Hook() {
    WNDCLASSA wc{};
    wc.lpfnWndProc = DummyWndProc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "AC4ToolsDummyWindow";
    RegisterClassA(&wc);
    HWND hwnd = CreateWindowA(wc.lpszClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    D3D_FEATURE_LEVEL featureLevel{};
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               0,
                                               levels,
                                               2,
                                               D3D11_SDK_VERSION,
                                               &sd,
                                               &swapChain,
                                               &device,
                                               &featureLevel,
                                               &context);
    if (FAILED(hr)) {
        DestroyWindow(hwnd);
        Log("D3D11CreateDeviceAndSwapChain dummy creation failed.");
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(swapChain);
    void* present = vtable[8];

    if (MH_Initialize() != MH_OK && MH_Initialize() != MH_ERROR_ALREADY_INITIALIZED) {
        Log("MinHook initialize failed.");
    }
    if (MH_CreateHook(present, &HookPresent, reinterpret_cast<void**>(&g_originalPresent)) != MH_OK) {
        Log("MinHook CreateHook Present failed.");
    } else if (MH_EnableHook(present) != MH_OK) {
        Log("MinHook EnableHook Present failed.");
    } else {
        Log("DX11 Present hook installed.");
    }

    swapChain->Release();
    context->Release();
    device->Release();
    DestroyWindow(hwnd);
    return true;
}

HRESULT __stdcall HookGetDeviceState(IDirectInputDevice8A* device, DWORD dataSize, LPVOID data) {
    GetDeviceStateFn original = nullptr;
    if (IsMouseDevice(device)) {
        original = g_originalGetDeviceStateMouse ? g_originalGetDeviceStateMouse : g_originalGetDeviceStateKeyboard;
    } else if (IsKeyboardDevice(device)) {
        original = g_originalGetDeviceStateKeyboard ? g_originalGetDeviceStateKeyboard : g_originalGetDeviceStateMouse;
    } else {
        original = g_originalGetDeviceStateKeyboard ? g_originalGetDeviceStateKeyboard : g_originalGetDeviceStateMouse;
    }
    if (!original) {
        return DIERR_NOTINITIALIZED;
    }
    const HRESULT result = original(device, dataSize, data);
    if (SUCCEEDED(result) && g_menuOpen && data && dataSize > 0) {
        const bool isMouse = IsMouseDevice(device);
        if (IsMouseInputCaptureActive() && isMouse) {
            memset(data, 0, dataSize);
            return DI_OK;
        }
        if (IsKeyboardInputCaptureActive() && !isMouse) {
            memset(data, 0, dataSize);
            return DI_OK;
        }
    }
    return result;
}

HRESULT __stdcall HookGetDeviceData(IDirectInputDevice8A* device,
                                    DWORD objectDataSize,
                                    LPDIDEVICEOBJECTDATA objectData,
                                    LPDWORD inOut,
                                    DWORD flags) {
    GetDeviceDataFn original = nullptr;
    if (IsMouseDevice(device)) {
        original = g_originalGetDeviceDataMouse ? g_originalGetDeviceDataMouse : g_originalGetDeviceDataKeyboard;
    } else if (IsKeyboardDevice(device)) {
        original = g_originalGetDeviceDataKeyboard ? g_originalGetDeviceDataKeyboard : g_originalGetDeviceDataMouse;
    } else {
        original = g_originalGetDeviceDataKeyboard ? g_originalGetDeviceDataKeyboard : g_originalGetDeviceDataMouse;
    }
    if (!original) {
        return DIERR_NOTINITIALIZED;
    }
    const HRESULT result = original(device, objectDataSize, objectData, inOut, flags);
    if (SUCCEEDED(result) && g_menuOpen && inOut) {
        const bool isMouse = IsMouseDevice(device);
        if (IsMouseInputCaptureActive() && isMouse) {
            *inOut = 0;
            return DI_OK;
        }
        if (IsKeyboardInputCaptureActive() && !isMouse) {
            *inOut = 0;
            return DI_OK;
        }
    }
    return result;
}

SHORT WINAPI HookGetAsyncKeyState(int vk) {
    if (ShouldBlockPolledKeyboardKey(vk)) {
        return 0;
    }
    return g_originalGetAsyncKeyState(vk);
}

SHORT WINAPI HookGetKeyState(int vk) {
    if (ShouldBlockPolledKeyboardKey(vk)) {
        return 0;
    }
    return g_originalGetKeyState(vk);
}

BOOL WINAPI HookGetKeyboardState(PBYTE keyState) {
    const BOOL result = g_originalGetKeyboardState(keyState);
    if (result && keyState && IsKeyboardInputCaptureActive()) {
        for (int vk = 0; vk < 256; ++vk) {
            if (!ShouldBlockPolledKeyboardKey(vk)) {
                continue;
            }
            keyState[vk] = 0;
        }
    }
    return result;
}

bool InstallDirectInputHook() {
    IDirectInput8A* directInput = nullptr;
    HRESULT hr = DirectInput8Create(GetModuleHandleA(nullptr),
                                    DIRECTINPUT_VERSION,
                                    IID_IDirectInput8A,
                                    reinterpret_cast<void**>(&directInput),
                                    nullptr);
    if (FAILED(hr) || !directInput) {
        Log("DirectInput8Create failed for hook discovery.");
        return false;
    }

    IDirectInputDevice8A* mouse = nullptr;
    hr = directInput->CreateDevice(GUID_SysMouse, &mouse, nullptr);
    if (FAILED(hr) || !mouse) {
        directInput->Release();
        Log("DirectInput CreateDevice(GUID_SysMouse) failed for hook discovery.");
        return false;
    }

    IDirectInputDevice8A* keyboard = nullptr;
    hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, nullptr);
    if (FAILED(hr) || !keyboard) {
        mouse->Release();
        directInput->Release();
        Log("DirectInput CreateDevice(GUID_SysKeyboard) failed for hook discovery.");
        return false;
    }

    void** mouseVtable = *reinterpret_cast<void***>(mouse);
    void** keyboardVtable = *reinterpret_cast<void***>(keyboard);
    void* mouseGetDeviceState = mouseVtable[9];
    void* mouseGetDeviceData = mouseVtable[10];
    void* keyboardGetDeviceState = keyboardVtable[9];
    void* keyboardGetDeviceData = keyboardVtable[10];

    if (MH_Initialize() != MH_OK && MH_Initialize() != MH_ERROR_ALREADY_INITIALIZED) {
        Log("MinHook initialize failed for DirectInput.");
    }

    if (MH_CreateHook(mouseGetDeviceState, &HookGetDeviceState, reinterpret_cast<void**>(&g_originalGetDeviceStateMouse)) == MH_OK &&
        MH_EnableHook(mouseGetDeviceState) == MH_OK) {
        Log("DirectInput mouse GetDeviceState hook installed.");
    } else {
        Log("DirectInput mouse GetDeviceState hook failed.");
    }

    if (MH_CreateHook(mouseGetDeviceData, &HookGetDeviceData, reinterpret_cast<void**>(&g_originalGetDeviceDataMouse)) == MH_OK &&
        MH_EnableHook(mouseGetDeviceData) == MH_OK) {
        Log("DirectInput mouse GetDeviceData hook installed.");
    } else {
        Log("DirectInput mouse GetDeviceData hook failed.");
    }

    if (keyboardGetDeviceState == mouseGetDeviceState) {
        g_originalGetDeviceStateKeyboard = g_originalGetDeviceStateMouse;
        Log("DirectInput keyboard GetDeviceState shares mouse hook target.");
    } else if (MH_CreateHook(keyboardGetDeviceState, &HookGetDeviceState, reinterpret_cast<void**>(&g_originalGetDeviceStateKeyboard)) == MH_OK &&
               MH_EnableHook(keyboardGetDeviceState) == MH_OK) {
        Log("DirectInput keyboard GetDeviceState hook installed.");
    } else {
        Log("DirectInput keyboard GetDeviceState hook failed.");
    }

    if (keyboardGetDeviceData == mouseGetDeviceData) {
        g_originalGetDeviceDataKeyboard = g_originalGetDeviceDataMouse;
        Log("DirectInput keyboard GetDeviceData shares mouse hook target.");
    } else if (MH_CreateHook(keyboardGetDeviceData, &HookGetDeviceData, reinterpret_cast<void**>(&g_originalGetDeviceDataKeyboard)) == MH_OK &&
               MH_EnableHook(keyboardGetDeviceData) == MH_OK) {
        Log("DirectInput keyboard GetDeviceData hook installed.");
    } else {
        Log("DirectInput keyboard GetDeviceData hook failed.");
    }

    keyboard->Release();
    mouse->Release();
    directInput->Release();
    return true;
}

bool InstallKeyboardStateHooks() {
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) {
        Log("user32.dll not available for keyboard state hooks.");
        return false;
    }

    void* getAsyncKeyState = reinterpret_cast<void*>(GetProcAddress(user32, "GetAsyncKeyState"));
    void* getKeyState = reinterpret_cast<void*>(GetProcAddress(user32, "GetKeyState"));
    void* getKeyboardState = reinterpret_cast<void*>(GetProcAddress(user32, "GetKeyboardState"));
    if (!getAsyncKeyState || !getKeyState || !getKeyboardState) {
        Log("Keyboard state hook discovery failed.");
        return false;
    }

    if (MH_Initialize() != MH_OK && MH_Initialize() != MH_ERROR_ALREADY_INITIALIZED) {
        Log("MinHook initialize failed for keyboard state hooks.");
    }

    if (MH_CreateHook(getAsyncKeyState, &HookGetAsyncKeyState, reinterpret_cast<void**>(&g_originalGetAsyncKeyState)) == MH_OK &&
        MH_EnableHook(getAsyncKeyState) == MH_OK) {
        Log("GetAsyncKeyState hook installed.");
    } else {
        Log("GetAsyncKeyState hook failed.");
    }

    if (MH_CreateHook(getKeyState, &HookGetKeyState, reinterpret_cast<void**>(&g_originalGetKeyState)) == MH_OK &&
        MH_EnableHook(getKeyState) == MH_OK) {
        Log("GetKeyState hook installed.");
    } else {
        Log("GetKeyState hook failed.");
    }

    if (MH_CreateHook(getKeyboardState, &HookGetKeyboardState, reinterpret_cast<void**>(&g_originalGetKeyboardState)) == MH_OK &&
        MH_EnableHook(getKeyboardState) == MH_OK) {
        Log("GetKeyboardState hook installed.");
    } else {
        Log("GetKeyboardState hook failed.");
    }

    return true;
}

void InitConsole() {
    if (!g_consoleLoggingEnabled) {
        return;
    }
    if (g_consoleReady) {
        return;
    }
    g_consoleMutex = CreateMutexA(nullptr, TRUE, "Global\\AC4ToolsConsole");
    if (!g_consoleMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        return;
    }
    if (!AllocConsole()) {
        return;
    }
    SetConsoleTitleA("AC4Tools v1.01 Log");
    freopen_s(&g_consoleOut, "CONOUT$", "w", stdout);
    FILE* consoleErr = nullptr;
    freopen_s(&consoleErr, "CONOUT$", "w", stderr);
    g_consoleReady = true;
    for (int i = 0; i < g_pendingConsoleLineCount; ++i) {
        const int index = (g_pendingConsoleLineStart + i) % 96;
        fprintf(g_consoleOut, "%s\n", g_pendingConsoleLines[index]);
    }
    g_pendingConsoleLineCount = 0;
    g_pendingConsoleLineStart = 0;
    Log("AC4Tools console attached.");
}

DWORD WINAPI MainThread(void*) {
    if (!IsTargetGameProcess()) {
        return 0;
    }
    InitModuleDir();
    InitGameInfo();
    LoadConfig();
    InstallPatches();
    InstallDx11Hook();
    InstallDirectInputHook();
    InstallKeyboardStateHooks();
    for (;;) {
        Sleep(250);
        UpdateTimeScaleInterval();
        ApplyNoCannonCooldownPatch();
        ApplyStealthModePatch();
        MaintainUnlocks();
    }
    return 0;
}

}  // namespace

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, void*) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        if (InterlockedCompareExchange(&g_mainStarted, 1, 0) == 0) {
            CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        }
    }
    return TRUE;
}
