// Private unity fragment; included by AC4Tools.cpp inside its namespace.
bool ShutdownRuntime() {
    g_stopping.store(true);
    StateGuard stateGuard(g_stateLock);
    g_menuOpen = false;
    ReleaseMenuInputState();
    g_noclipEnabled = false;
    g_finishingGameState = true;
    UpdateNoclipStateImpl();
    g_finishingGameState = false;
    // Stop native toggles before restoring their entry points.
    g_enabled = g_allyGodmode = g_playerGodmode = g_infiniteBreath = false;
    g_noFallDamage = g_lockConsumables = g_freeCamEnabled = g_noReload = false;
    g_freezeMissionTimer = false;
    g_timeScaleInterval = 10000;
    PublishCaveState();
    const bool hooksDisabled = g_hooks.DisableAll();
    const bool patchesRestored = g_patches.RestoreFrom();
    if (g_gameWindow && g_originalWndProc &&
        reinterpret_cast<WNDPROC>(GetWindowLongPtrA(g_gameWindow, GWLP_WNDPROC)) == WndProc) {
        SetWindowLongPtrA(g_gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_originalWndProc.load()));
    }
    if (g_imguiReady) {
        ImGui_ImplDX11_Shutdown(); ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(); g_imguiReady = false;
    }
    ReleaseRenderTarget();
    if (g_context) { g_context->Release(); g_context = nullptr; }
    if (g_device) { g_device->Release(); g_device = nullptr; }
    ReleasePolledControllerDevices();
    g_caves.ReleaseUnpublished();
    g_hooks.ReleaseIfUnused();
    // Published caves and MinHook trampolines remain valid for in-flight returns.
    Log(hooksDisabled && patchesRestored ? "AC4Tools stopped; owned hooks and patches disabled." :
        "AC4Tools stopped with retained patch sites; restart the game before using another version.");
    if (g_consoleMutex) { CloseHandle(g_consoleMutex); g_consoleMutex = nullptr; }
    if (g_consoleReady) {
        if (g_consoleOut) { fclose(g_consoleOut); g_consoleOut = nullptr; }
        if (g_consoleErr) { fclose(g_consoleErr); g_consoleErr = nullptr; }
        FreeConsole(); g_consoleReady = false;
    }
    g_stopped.store(true);
    return hooksDisabled && patchesRestored;
}

DWORD WINAPI MainThread(void*) {
    if (!IsTargetGameProcess()) { g_stopped.store(true); return 0; }
    {
        StateGuard stateGuard(g_stateLock);
        InitModuleDir(); InitGameInfo(); LoadConfig(); LogGameInfo();
        if (!g_supportedBuild) LogWarn("Gameplay disabled: supported SHA256 and image base are required.");
        PublishCaveState();
        InstallPatches();
        const bool overlayReady = InstallDx11Hook();
        if (overlayReady) {
            InstallDirectInputHook(); InstallKeyboardStateHooks(); InstallXInputHooks();
        }
        if (!overlayReady || !g_hooks.EnableAll()) {
            LogError("Overlay hook setup failed; stopping AC4Tools.");
            g_stopping.store(true);
        }
    }
    while (!g_stopping.load()) {
        Sleep(250);
        StateGuard stateGuard(g_stateLock);
        if (g_stopping.load()) break;
        UpdateTimeScaleInterval();
        ApplyNoCannonCooldownPatch(); ApplyStealthModePatch();
        ApplyEagleVisionSprintPatch(); ApplyKillCiviliansNoDesyncPatch();
        MaintainUnlocks(); PublishCaveState();
    }
    ShutdownRuntime();
    return 0;
}
