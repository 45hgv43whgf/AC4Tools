// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void LoadConfig() {
    char ownPath[MAX_PATH]{};
    ConfigPath(ownPath, MAX_PATH);

    g_consoleLoggingEnabled = GetPrivateProfileIntA("Logging", "EnableConsole", 0, ownPath) != 0;
    g_fileLoggingEnabled = GetPrivateProfileIntA("Logging", "EnableFile", 0, ownPath) != 0;
    g_lockMouseToWindow = GetPrivateProfileIntA("System", "LockMouseToWindow", 0, ownPath) != 0;
    g_disableMouseInputWhenUiOpen = GetPrivateProfileIntA("System", "DisableMouseInputWhenUiOpen", 0, ownPath) != 0;
    g_disableKeyboardInputWhenUiOpen = GetPrivateProfileIntA("System", "DisableKeyboardInputWhenUiOpen", 0, ownPath) != 0;
    g_disableControllerInputWhenUiOpen = GetPrivateProfileIntA("System", "DisableControllerInputWhenUiOpen", 0, ownPath) != 0;
    g_disableHotkeysWhileUiOpen = GetPrivateProfileIntA("System", "DisableHotkeysWhileUiOpen", 0, ownPath) != 0;
    char windowPosValue[32]{};
    GetPrivateProfileStringA("UI", "WindowPosX", "60", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuPos.x = ParseFiniteFloat(windowPosValue, 60.0f);
    GetPrivateProfileStringA("UI", "WindowPosY", "40", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuPos.y = ParseFiniteFloat(windowPosValue, 40.0f);
    GetPrivateProfileStringA("UI", "WindowSizeX", "680", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuSize.x = ParseFiniteFloat(windowPosValue, 680.0f);
    GetPrivateProfileStringA("UI", "WindowSizeY", "420", windowPosValue, sizeof(windowPosValue), ownPath);
    g_menuSize.y = ParseFiniteFloat(windowPosValue, 420.0f);
    if (g_menuSize.x < 320.0f) {
        g_menuSize.x = 320.0f;
    }
    if (g_menuSize.y < 240.0f) {
        g_menuSize.y = 240.0f;
    }

    char timeScaleValue[32]{};
    GetPrivateProfileStringA("Game", "TimeScale", "0.010000", timeScaleValue, sizeof(timeScaleValue), ownPath);
    g_timeScale = ParseFiniteFloat(timeScaleValue, 0.01f);
    if (g_timeScale < 0.0001f) {
        g_timeScale = 0.0001f;
    }
    if (g_timeScale > 100.0f) {
        g_timeScale = 100.0f;
    }

    char freeCamValue[32]{};
    GetPrivateProfileStringA("Game", "FreeCamSpeed", "1.000000", freeCamValue, sizeof(freeCamValue), ownPath);
    g_freeCamSpeed = ParseFiniteFloat(freeCamValue, 1.0f);
    if (g_freeCamSpeed < 0.001f) {
        g_freeCamSpeed = 0.001f;
    }
    if (g_freeCamSpeed > 100.0f) {
        g_freeCamSpeed = 100.0f;
    }

    GetPrivateProfileStringA("Game", "FreeCamVerticalSpeed", "1.000000", freeCamValue, sizeof(freeCamValue), ownPath);
    g_freeCamVerticalSpeed = ParseFiniteFloat(freeCamValue, 1.0f);
    if (g_freeCamVerticalSpeed < 0.001f) {
        g_freeCamVerticalSpeed = 0.001f;
    }
    if (g_freeCamVerticalSpeed > 100.0f) {
        g_freeCamVerticalSpeed = 100.0f;
    }

    GetPrivateProfileStringA("Game", "FreeCamLookSpeed", "1.000000", freeCamValue, sizeof(freeCamValue), ownPath);
    g_freeCamLookSpeed = ParseFiniteFloat(freeCamValue, 1.0f);
    if (g_freeCamLookSpeed < 0.05f) {
        g_freeCamLookSpeed = 0.05f;
    }
    if (g_freeCamLookSpeed > 20.0f) {
        g_freeCamLookSpeed = 20.0f;
    }

    char noclipSpeedValue[32]{};
    GetPrivateProfileStringA("Noclip", "Speed", "1.000000", noclipSpeedValue, sizeof(noclipSpeedValue), ownPath);
    g_noclipSpeed = ParseFiniteFloat(noclipSpeedValue, 1.0f);
    if (g_noclipSpeed < 0.0001f) {
        g_noclipSpeed = 0.0001f;
    }
    if (g_noclipSpeed > 1000.0f) {
        g_noclipSpeed = 1000.0f;
    }

    char noclipBoostValue[32]{};
    GetPrivateProfileStringA("Noclip", "BoostSpeed", "5.000000", noclipBoostValue, sizeof(noclipBoostValue), ownPath);
    g_noclipBoostSpeed = ParseFiniteFloat(noclipBoostValue, 5.0f);
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
    GetPrivateProfileStringA("Hotkeys", "MenuOpenGamepad", "0", keyValue, sizeof(keyValue), ownPath);
    int button = atoi(keyValue);
    g_menuGamepadHotkey = IsBindableGamepadButton(button) ? button : 0;

    for (int i = 0; i < kActionCount; ++i) {
        GetPrivateProfileStringA("Hotkeys", g_actions[i].id, "0", keyValue, sizeof(keyValue), ownPath);
        vk = atoi(keyValue);
        g_actions[i].hotkey = IsBindableHotkey(vk) && vk != g_menuHotkey ? vk : 0;
        char gamepadKey[64]{};
        sprintf_s(gamepadKey, "%sGamepad", g_actions[i].id);
        GetPrivateProfileStringA("Hotkeys", gamepadKey, "0", keyValue, sizeof(keyValue), ownPath);
        button = atoi(keyValue);
        g_actions[i].gamepadHotkey =
            IsBindableGamepadButton(button) && button != g_menuGamepadHotkey ? button : 0;
    }
    for (int i = 0; i < kFreeCamControlCount; ++i) {
        char defaultValue[16]{};
        sprintf_s(defaultValue, "%d", g_freeCamControls[i].defaultKey);
        GetPrivateProfileStringA("FreeCamControls", g_freeCamControls[i].id, defaultValue, keyValue, sizeof(keyValue), ownPath);
        vk = atoi(keyValue);
        g_freeCamControls[i].key = IsBindableHotkey(vk) ? vk : g_freeCamControls[i].defaultKey;
        sprintf_s(defaultValue, "%d", g_freeCamControls[i].defaultGamepadButton);
        char gamepadKey[64]{};
        sprintf_s(gamepadKey, "%sGamepad", g_freeCamControls[i].id);
        GetPrivateProfileStringA("FreeCamControls", gamepadKey, defaultValue, keyValue, sizeof(keyValue), ownPath);
        button = atoi(keyValue);
        g_freeCamControls[i].gamepadButton =
            IsBindableGamepadButton(button) ? button : g_freeCamControls[i].defaultGamepadButton;
    }
}
