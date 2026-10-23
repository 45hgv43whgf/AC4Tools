// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void SaveConfig() {
    char path[MAX_PATH]{};
    ConfigPath(path, MAX_PATH);
    char value[32]{};
    WritePrivateProfileStringA("Logging", "EnableConsole", g_consoleLoggingEnabled ? "1" : "0", path);
    WritePrivateProfileStringA("Logging", "EnableFile", g_fileLoggingEnabled ? "1" : "0", path);
    WritePrivateProfileStringA("System", "LockMouseToWindow", g_lockMouseToWindow ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableMouseInputWhenUiOpen", g_disableMouseInputWhenUiOpen ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableKeyboardInputWhenUiOpen", g_disableKeyboardInputWhenUiOpen ? "1" : "0", path);
    WritePrivateProfileStringA("System", "DisableControllerInputWhenUiOpen", g_disableControllerInputWhenUiOpen ? "1" : "0", path);
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
    sprintf_s(value, "%.6f", g_freeCamSpeed);
    WritePrivateProfileStringA("Game", "FreeCamSpeed", value, path);
    sprintf_s(value, "%.6f", g_freeCamVerticalSpeed);
    WritePrivateProfileStringA("Game", "FreeCamVerticalSpeed", value, path);
    sprintf_s(value, "%.6f", g_freeCamLookSpeed);
    WritePrivateProfileStringA("Game", "FreeCamLookSpeed", value, path);
    sprintf_s(value, "%.6f", g_noclipSpeed);
    WritePrivateProfileStringA("Noclip", "Speed", value, path);
    sprintf_s(value, "%.6f", g_noclipBoostSpeed);
    WritePrivateProfileStringA("Noclip", "BoostSpeed", value, path);
    sprintf_s(value, "%d", g_menuHotkey);
    WritePrivateProfileStringA("Hotkeys", "MenuOpen", value, path);
    sprintf_s(value, "%d", g_menuGamepadHotkey);
    WritePrivateProfileStringA("Hotkeys", "MenuOpenGamepad", value, path);
    for (int i = 0; i < kActionCount; ++i) {
        sprintf_s(value, "%d", g_actions[i].hotkey);
        WritePrivateProfileStringA("Hotkeys", g_actions[i].id, value, path);
        sprintf_s(value, "%d", g_actions[i].gamepadHotkey);
        char gamepadKey[64]{};
        sprintf_s(gamepadKey, "%sGamepad", g_actions[i].id);
        WritePrivateProfileStringA("Hotkeys", gamepadKey, value, path);
    }
    for (int i = 0; i < kFreeCamControlCount; ++i) {
        sprintf_s(value, "%d", g_freeCamControls[i].key);
        WritePrivateProfileStringA("FreeCamControls", g_freeCamControls[i].id, value, path);
        sprintf_s(value, "%d", g_freeCamControls[i].gamepadButton);
        char gamepadKey[64]{};
        sprintf_s(gamepadKey, "%sGamepad", g_freeCamControls[i].id);
        WritePrivateProfileStringA("FreeCamControls", gamepadKey, value, path);
    }
}
