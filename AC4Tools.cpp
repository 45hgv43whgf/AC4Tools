#include <windows.h>
#include <d3d11.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include <dxgi.h>
#include <wincrypt.h>
#include <xinput.h>

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <atomic>
#include "runtime/StateLock.h"
#include "runtime/AtomicCell.h"
#include "runtime/HookManager.h"
#include "patches/PatchRegistry.h"
#include "config/Numeric.h"
#include "game_addresses/BuildPolicy.h"
#include "game_addresses/PatternScan.h"

#include "MinHook.h"
#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "advapi32.lib")


extern "C" IMAGE_DOS_HEADER __ImageBase;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace {

StateLock g_stateLock;
HookManager g_hooks;
PatchRegistry g_patches;
CaveRegistry g_caves;
std::atomic<bool> g_stopping{false};
std::atomic<bool> g_stopped{false};
bool g_finishingGameState = false; // Protected by g_stateLock; shutdown-only game calls.
bool g_supportedBuild = false; // Set once, before installing any callbacks.
void PublishCaveState();


#include "game_addresses/SupportedBuild.h"

#include "runtime/State.inl"

void InitConsole();
void __stdcall UpdateNoclipState();

enum class LogLevel {
    Info,
    Warning,
    Error,
};
void LogWarn(const char* message);
void UpdateTimeScaleInterval();
void UpdateFreeCam();
bool InstallLockConsumablesPatch();
bool InstallFreeCamPatch();
void ApplyNoCannonCooldownPatch();
void ApplyStealthModePatch();
void ApplyEagleVisionSprintPatch();
void ApplyKillCiviliansNoDesyncPatch();
void DesynchronizeYourself();
bool ToggleHood();
bool ToggleCrouch();
void __stdcall ShowCollectiblesVisit(std::uint8_t* mapRoot);
bool IsBindableGamepadButton(int button);
bool IsGamepadButtonDown(int button);
bool WasGamepadButtonJustPressed(int button);
void ClearKeyboardHotkeyConflicts(int vk, int actionToSkip, int freeCamControlToSkip);
void ClearGamepadHotkeyConflicts(int button, int actionToSkip, int freeCamControlToSkip);
bool IsControllerDevice(IDirectInputDevice8A* device);
BOOL CALLBACK EnumControllerDevicesCallback(const DIDEVICEINSTANCEA* instance, VOID* context);
void ReleasePolledControllerDevices();
void BeginControllerPollingFrame();
void InitializeControllerPolling();
void PollControllerDevices();
void CaptureDirectInputControllerStateFromJoystick(const DIJOYSTATE2& state);

#include "input/Bindings.inl"

#include "input/Capture.inl"

#include "runtime/Logging.inl"

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

#include "input/Controller.inl"

#include "config/Save.inl"

#include "patches/Memory.inl"

#include "patches/Actions.inl"

#include "patches/Collectibles.inl"

#include "patches/Emit.inl"

#include "game_addresses/BuildDetection.inl"

#include "unlock/Inventory.inl"

#include "unlock/Unlocks.inl"

#include "config/Load.inl"

#include "patches/FreeCam.inl"

#include "patches/Player.inl"

#include "input/Hotkeys.inl"

#include "patches/Install.inl"

#include "overlay/Overlay.inl"

#include "overlay/Hooks.inl"

#include "input/Hooks.inl"

#include "runtime/Console.inl"

#include "runtime/Lifecycle.inl"

}  // namespace

// Nonblocking request, safe for a loader/tool to call outside DllMain.
extern "C" __declspec(dllexport) void WINAPI AC4ToolsRequestShutdown() {
    g_stopping.store(true);
}
extern "C" __declspec(dllexport) BOOL WINAPI AC4ToolsIsStopped() {
    return g_stopped.load() ? TRUE : FALSE;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, void*) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        if (!IsTargetGameProcess()) { g_stopped.store(true); return TRUE; }
        HMODULE pinnedModule = nullptr;
        if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
                              reinterpret_cast<LPCSTR>(module), &pinnedModule)) {
            g_stopped.store(true); return TRUE;
        }
        if (InterlockedCompareExchange(&g_mainStarted, 1, 0) == 0) {
            HANDLE thread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
            if (thread) CloseHandle(thread);
            else g_stopped.store(true);
        }
    }
    // No waiting or hook APIs under the loader lock, including process exit.
    return TRUE;
}
