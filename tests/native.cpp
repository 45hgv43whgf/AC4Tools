// Build the same private unity source into an isolated Win32 test executable.
// It is deliberately named AC4BFSP.exe, but is not the supported game build.
#include "../AC4Tools.cpp"
#include <limits>
#include <initializer_list>

#define CHECK(condition) do { if (!(condition)) { \
    printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); return false; \
} } while (0)

bool TestBuildGate() {
    CHECK(IsTargetGameProcess());
    InitGameInfo();
    CHECK(!g_supportedBuild);
    CHECK(!g_toggleHoodReady && !g_toggleCrouchReady && !g_freeCamHotkeyReady && !g_lockConsumablesHotkeyReady);
    CHECK(IsSupportedBuild("AC4BFSP.exe", kSupportedGameSha256, 0x400000, kSupportedGameSha256));
    CHECK(!IsSupportedBuild("renamed.exe", kSupportedGameSha256, 0x400000, kSupportedGameSha256));
    CHECK(!IsSupportedBuild("AC4BFSP.exe", "unknown", 0x400000, kSupportedGameSha256));
    CHECK(!IsSupportedBuild("AC4BFSP.exe", kSupportedGameSha256, 0x500000, kSupportedGameSha256));
    bool (*installers[])() = {
        InstallPatches, InstallHealthPatch, InstallNoCannonCooldownPatch, InstallStealthModePatch,
        InstallAllyGodmodePatch, InstallTimeIntervalPatch, InstallPlayerHealthPatch,
        InstallInfiniteBreathPatch, InstallNoFallDamagePatch, InstallEagleVisionSprintPatch,
        InstallKillCiviliansNoDesyncPatch, InstallShowCollectiblesPatch, InstallLockConsumablesPatch,
        InstallInventoryPointerPatch, InstallNoclipUpdatePatch, InstallFreeCamPatch,
        InstallGlobalHiddenUnlockPatch, InstallNoReloadPatch, InstallMissionTimerPatch, InstallMissionTimer2Patch,
    };
    for (auto install : installers) CHECK(!install());
    CHECK(!GetPlayerActor() && !GetPlayerEntity() && !GetNoclipComponent());
    CHECK(!ToggleHood() && !ToggleCrouch() && !IsGameNoclipActive());
    CHECK(GetPlayerHoodState() == -1 && GetPlayerCrouchState() == -1);
    g_inventoryBase = 1; g_playerHealthComponent = 1;
    CHECK(!WriteInventoryValueByOffset(0, 100, true));
    CHECK(!AddInventoryValueByOffset(0, 100, "test"));
    DesynchronizeYourself(); UpdateNoclipState(); UpdateFreeCam(); MaintainUnlocks(); ScanAndApplyUnlocks();
    ApplyNoCannonCooldownPatch(); ApplyStealthModePatch(); ApplyEagleVisionSprintPatch(); ApplyKillCiviliansNoDesyncPatch();
    CHECK(g_patches.checkpoint() == 0);
    unsigned char byte = 0x90, patch = 0xCC;
    CHECK(!WriteMemory(&byte, &patch, 1) && byte == 0x90);
    CHECK(!FindMainModulePattern(&byte, 1));
    CHECK(!FindMainModulePatternMasked(&byte, "x", 1));
    g_inventoryBase = 0; g_playerHealthComponent = 0;
    return true;
}

bool TestNumbers() {
    for (const char* text : {"nan", "NaN", "inf", "-inf", "1e999", "1e-999", "", " ", "2oops", "--1"}) {
        CHECK(ParseFiniteFloat(text, 3.0f) == 3.0f);
    }
    CHECK(ParseFiniteFloat(" 2.5 \t", 3.0f) == 2.5f);
    CHECK(SaturatingAdd(INT_MAX - 10, 100) == INT_MAX);
    CHECK(SaturatingAdd(INT_MIN + 10, -100) == INT_MIN);
    CHECK(SaturatingAdd(0, INT_MIN) == INT_MIN);
    CHECK(SaturatingAdd(-1, INT_MAX) == INT_MAX - 1);
    CHECK(SaturatingAdd(40, 100) == 140);
    g_timeScaleEnabled = true;
    for (float number : {std::numeric_limits<float>::quiet_NaN(),
                         std::numeric_limits<float>::infinity(), 0.0f, -1.0f}) {
        g_timeScale = number; UpdateTimeScaleInterval();
        CHECK(g_timeScaleInterval >= 1 && g_timeScaleInterval <= 10000000);
    }
    g_timeScaleEnabled = false; UpdateTimeScaleInterval();
    CHECK(g_timeScaleInterval == 10000);
    return true;
}

bool TestConfigFile() {
    char directory[MAX_PATH]{};
    CHECK(GetModuleFileNameA(nullptr, directory, MAX_PATH));
    char* slash = strrchr(directory, '\\'); CHECK(slash);
    slash[1] = '\0';
    strcat_s(directory, "config-fixture\\");
    CHECK(CreateDirectoryA(directory, nullptr) || GetLastError() == ERROR_ALREADY_EXISTS);
    strcpy_s(g_moduleDir, directory);
    char path[MAX_PATH]{}; ConfigPath(path, sizeof(path));
    CHECK(WritePrivateProfileStringA("UI", "WindowSizeX", "nan", path));
    CHECK(WritePrivateProfileStringA("UI", "WindowPosY", "inf", path));
    CHECK(WritePrivateProfileStringA("Game", "TimeScale", "nan", path));
    CHECK(WritePrivateProfileStringA("Game", "FreeCamSpeed", "1e999", path));
    CHECK(WritePrivateProfileStringA("Noclip", "BoostSpeed", "5junk", path));
    LoadConfig();
    CHECK(g_menuSize.x == 680 && g_menuPos.y == 40);
    CHECK(g_timeScale == 0.01f && g_freeCamSpeed == 1 && g_noclipBoostSpeed == 5);
    CHECK(DeleteFileA(path));
    CHECK(RemoveDirectoryA(directory));
    g_moduleDir[0] = '\0';
    return true;
}

bool TestPatternScanner() {
    auto* image = static_cast<unsigned char*>(VirtualAlloc(nullptr, 16384, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    CHECK(image);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(image);
    dos->e_magic = IMAGE_DOS_SIGNATURE; dos->e_lfanew = 128;
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(image + 128);
    nt->Signature = IMAGE_NT_SIGNATURE; nt->OptionalHeader.SizeOfImage = 16384;
    const unsigned char pattern[] = { 0xAB, 0xBA, 0xDE, 0xCA, 0xFE, 0xED };
    memcpy(image + 4128, pattern, sizeof(pattern));
    CHECK(FindUniqueImagePattern(reinterpret_cast<HMODULE>(image), pattern, nullptr, sizeof(pattern)) == image + 4128);
    memcpy(image + 8224, pattern, sizeof(pattern));
    CHECK(!FindUniqueImagePattern(reinterpret_cast<HMODULE>(image), pattern, nullptr, sizeof(pattern)));
    DWORD old = 0;
    CHECK(VirtualProtect(image + 8192, 4096, PAGE_NOACCESS, &old));
    CHECK(FindUniqueImagePattern(reinterpret_cast<HMODULE>(image), pattern, nullptr, sizeof(pattern)) == image + 4128);
    CHECK(VirtualProtect(image + 4096, 4096, PAGE_READONLY, &old));
    CHECK(!FindUniqueImagePattern(reinterpret_cast<HMODULE>(image), pattern, nullptr, sizeof(pattern)));
    CHECK(VirtualFree(image, 0, MEM_RELEASE));
    CHECK(!FindUniqueImagePattern(reinterpret_cast<HMODULE>(1), pattern, nullptr, sizeof(pattern)));
    return true;
}

DWORD WINAPI TestThread(void*) { return 0; }

bool TestPatchWriter() {
    auto* code = static_cast<unsigned char*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    CHECK(code);
    memset(code, 0x90, 4096);
    unsigned char replacement[5] = {0xE9, 0, 0, 0, 0};
    PatchRegistry registry;
    unsigned char expected[5] = {0x90, 0x90, 0x90, 0x90, 0x90};
    CHECK(registry.Expect(code, expected, sizeof(expected)));
    code[0] = 0xCC; // changed between validation and the actual write
    CHECK(!registry.Write(code, replacement, sizeof(replacement)) && code[0] == 0xCC);
    code[0] = 0x90;
    HANDLE thread = CreateThread(nullptr, 0, TestThread, nullptr, CREATE_SUSPENDED, nullptr);
    CHECK(thread);
    CONTEXT saved{}; saved.ContextFlags = CONTEXT_CONTROL;
    CHECK(GetThreadContext(thread, &saved));
    CONTEXT inside = saved;
    inside.Eip = reinterpret_cast<DWORD>(code + 1);
    CHECK(SetThreadContext(thread, &inside));
    CHECK(!registry.Write(code, replacement, sizeof(replacement))); // in-flight instruction
    CHECK(code[0] == 0x90);
    CHECK(SetThreadContext(thread, &saved));
    CHECK(ResumeThread(thread) != DWORD(-1));
    CHECK(WaitForSingleObject(thread, 5000) == WAIT_OBJECT_0);
    CloseHandle(thread);

    DWORD previous = 0;
    CHECK(VirtualProtect(code, 4096, PAGE_EXECUTE_READ, &previous));
    CHECK(registry.Write(code, replacement, sizeof(replacement)));
    MEMORY_BASIC_INFORMATION info{};
    CHECK(VirtualQuery(code, &info, sizeof(info)));
    CHECK(info.Protect == PAGE_EXECUTE_READ);
    CHECK(!registry.Write(code + 1, replacement, sizeof(replacement))); // overlaps
    CHECK(VirtualProtect(code, 4096, PAGE_EXECUTE_READWRITE, &previous));
    code[0] = 0xCC; // simulate another patch owner
    CHECK(!registry.RestoreFrom() && code[0] == 0xCC);
    memcpy(code, replacement, sizeof(replacement));
    CHECK(registry.RestoreFrom());
    for (int i = 0; i < 5; ++i) CHECK(code[i] == 0x90);
    CHECK(!registry.Write(reinterpret_cast<void*>(1), replacement, sizeof(replacement)));
    CHECK(VirtualFree(code, 0, MEM_RELEASE));
    return true;
}

using TestFn = int(WINAPI*)(int);
TestFn g_testOriginal = nullptr;
__declspec(noinline) int WINAPI TestTarget(int value) { return value * 3 + 7; }
int WINAPI TestDetour(int value) { return g_testOriginal(value) + 100; }

bool TestHookLifecycle() {
    CHECK(g_hooks.Initialize() && g_hooks.Initialize());
    CHECK(g_hooks.Create(nullptr, reinterpret_cast<void*>(&TestDetour), reinterpret_cast<void**>(&g_testOriginal)) != MH_OK);
    CHECK(g_hooks.Create(reinterpret_cast<void*>(&TestTarget), reinterpret_cast<void*>(&TestDetour),
                         reinterpret_cast<void**>(&g_testOriginal)) == MH_OK);
    CHECK(g_hooks.Enable(reinterpret_cast<void*>(&TestTarget)) == MH_OK);
    TestFn volatile target = TestTarget;
    CHECK(target(2) == 13); // queued, not yet published
    CHECK(g_hooks.EnableAll());
    CHECK(target(2) == 113);
    CHECK(g_hooks.DisableAll());
    CHECK(target(2) == 13);
    CHECK(g_testOriginal(2) == 13); // in-flight return paths remain valid
    return true;
}

bool TestCaveStateAndShutdown() {
    g_enabled = true; g_freeCamX = 1.25f; PublishCaveState();
    CHECK(g_enabledCell.load() == 1 && g_freeCamXCell.load() == 1.25f);
    void* unused = g_caves.Allocate(64);
    void* published = g_caves.Allocate(64);
    CHECK(unused && published);
    g_caves.Publish(published);
    CHECK(ShutdownRuntime());
    CHECK(g_stopping.load() && g_stopped.load() && !g_enabledCell.load());
    MEMORY_BASIC_INFORMATION info{};
    CHECK(VirtualQuery(unused, &info, sizeof(info)) && info.State == MEM_FREE);
    CHECK(VirtualQuery(published, &info, sizeof(info)) && info.State == MEM_COMMIT);
    CHECK(ShutdownRuntime()); // idempotent
    // This synthetic cave has never actually executed and can be freed in the test.
    CHECK(VirtualFree(published, 0, MEM_RELEASE));
    return true;
}

int main() {
    if (!TestBuildGate() || !TestNumbers() || !TestConfigFile() || !TestPatternScanner() || !TestPatchWriter() ||
        !TestHookLifecycle() || !TestCaveStateAndShutdown()) return 1;
    puts("PASS: unsupported-build gates, numeric parsing, patch ownership/quiescence, hooks, shutdown.");
    return 0;
}
