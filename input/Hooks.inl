// Private unity fragment; included by AC4Tools.cpp inside its namespace.
HRESULT __stdcall HookGetDeviceState(IDirectInputDevice8A* device, DWORD dataSize, LPVOID data) {
    StateGuard stateGuard(g_stateLock);
    GetDeviceStateFn original = nullptr;
    const bool isMouse = IsMouseDevice(device);
    const bool isKeyboard = IsKeyboardDevice(device);
    const bool isController = IsControllerDevice(device);
    if (isMouse) {
        original = g_originalGetDeviceStateMouse ? g_originalGetDeviceStateMouse : g_originalGetDeviceStateKeyboard;
    } else if (isKeyboard) {
        original = g_originalGetDeviceStateKeyboard ? g_originalGetDeviceStateKeyboard : g_originalGetDeviceStateMouse;
    } else {
        original = g_originalGetDeviceStateKeyboard ? g_originalGetDeviceStateKeyboard : g_originalGetDeviceStateMouse;
    }
    if (!original) {
        return DIERR_NOTINITIALIZED;
    }
    stateGuard.unlock();
    const HRESULT result = original(device, dataSize, data);
    stateGuard.lock();
    if (g_stopping.load()) return result;
    if (SUCCEEDED(result) && data && dataSize > 0) {
        if (isMouse && g_menuOpen) {
            AccumulateMouseWheelDelta(data, dataSize);
        }
        if (isController) {
            CaptureDirectInputControllerState(data, dataSize);
        }
        if (g_menuOpen) {
            if (IsMouseInputCaptureActive() && isMouse) {
                memset(data, 0, dataSize);
                return DI_OK;
            }
            if (IsControllerInputCaptureActive() && isController) {
                memset(data, 0, dataSize);
                return DI_OK;
            }
            if (IsKeyboardInputCaptureActive() && isKeyboard) {
                memset(data, 0, dataSize);
                return DI_OK;
            }
        }
        if (isMouse) {
            ApplyFreeCamMouseLookScale(data, dataSize);
        }
    }
    return result;
}

HRESULT __stdcall HookGetDeviceData(IDirectInputDevice8A* device,
                                    DWORD objectDataSize,
                                    LPDIDEVICEOBJECTDATA objectData,
                                    LPDWORD inOut,
                                    DWORD flags) {
    StateGuard stateGuard(g_stateLock);
    GetDeviceDataFn original = nullptr;
    const bool isMouse = IsMouseDevice(device);
    const bool isKeyboard = IsKeyboardDevice(device);
    const bool isController = IsControllerDevice(device);
    if (isMouse) {
        original = g_originalGetDeviceDataMouse ? g_originalGetDeviceDataMouse : g_originalGetDeviceDataKeyboard;
    } else if (isKeyboard) {
        original = g_originalGetDeviceDataKeyboard ? g_originalGetDeviceDataKeyboard : g_originalGetDeviceDataMouse;
    } else {
        original = g_originalGetDeviceDataKeyboard ? g_originalGetDeviceDataKeyboard : g_originalGetDeviceDataMouse;
    }
    if (!original) {
        return DIERR_NOTINITIALIZED;
    }
    stateGuard.unlock();
    const HRESULT result = original(device, objectDataSize, objectData, inOut, flags);
    stateGuard.lock();
    if (g_stopping.load()) return result;
    if (SUCCEEDED(result) && inOut) {
        if (isMouse && g_menuOpen && objectData && *inOut > 0) {
            AccumulateMouseWheelDelta(objectData, *inOut);
        }
        if (isController && objectData && *inOut > 0) {
            CaptureDirectInputControllerState(objectData, *inOut);
        }
        if (g_menuOpen) {
            if (IsMouseInputCaptureActive() && isMouse) {
                *inOut = 0;
                return DI_OK;
            }
            if (IsControllerInputCaptureActive() && isController) {
                *inOut = 0;
                return DI_OK;
            }
            if (IsKeyboardInputCaptureActive() && isKeyboard) {
                *inOut = 0;
                return DI_OK;
            }
        }
        if (isMouse && objectData && *inOut > 0) {
            ApplyFreeCamMouseLookScale(objectData, *inOut);
        }
    }
    return result;
}

SHORT WINAPI HookGetAsyncKeyState(int vk) {
    StateGuard stateGuard(g_stateLock);
    if (ShouldBlockPolledKeyboardKey(vk)) {
        return 0;
    }
    return g_originalGetAsyncKeyState(vk);
}

SHORT WINAPI HookGetKeyState(int vk) {
    StateGuard stateGuard(g_stateLock);
    if (ShouldBlockPolledKeyboardKey(vk)) {
        return 0;
    }
    return g_originalGetKeyState(vk);
}

BOOL WINAPI HookGetKeyboardState(PBYTE keyState) {
    StateGuard stateGuard(g_stateLock);
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

DWORD WINAPI HookXInputGetStateCommon(XInputGetStateFn original, DWORD userIndex, XINPUT_STATE* state) {
    StateGuard stateGuard(g_stateLock);
    if (!g_stopping.load() && IsControllerInputCaptureActive()) {
        if (state) {
            ZeroMemory(state, sizeof(*state));
        }
        return ERROR_SUCCESS;
    }
    stateGuard.unlock();
    return original ? original(userIndex, state) : ERROR_DEVICE_NOT_CONNECTED;
}

DWORD WINAPI HookXInputGetState9(DWORD userIndex, XINPUT_STATE* state) {
    return HookXInputGetStateCommon(g_originalXInputGetState9, userIndex, state);
}

DWORD WINAPI HookXInputGetState14(DWORD userIndex, XINPUT_STATE* state) {
    return HookXInputGetStateCommon(g_originalXInputGetState14, userIndex, state);
}

DWORD WINAPI HookXInputGetState13(DWORD userIndex, XINPUT_STATE* state) {
    return HookXInputGetStateCommon(g_originalXInputGetState13, userIndex, state);
}

DWORD WINAPI HookXInputGetState12(DWORD userIndex, XINPUT_STATE* state) {
    return HookXInputGetStateCommon(g_originalXInputGetState12, userIndex, state);
}

DWORD WINAPI HookXInputGetState11(DWORD userIndex, XINPUT_STATE* state) {
    return HookXInputGetStateCommon(g_originalXInputGetState11, userIndex, state);
}

bool HookXInputGetStateFromModule(const char* moduleName, XInputGetStateFn hook, XInputGetStateFn* original) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN, moduleName, &module)) return false;
    void* target = reinterpret_cast<void*>(GetProcAddress(module, "XInputGetState"));
    if (!target) {
        return false;
    }
    const MH_STATUS createStatus = g_hooks.Create(target, hook, reinterpret_cast<void**>(original));
    if (createStatus != MH_OK) {
        LogWarnf("MinHook CreateHook %s!XInputGetState failed: %d.", moduleName, createStatus);
        return false;
    }
    const MH_STATUS enableStatus = g_hooks.Enable(target);
    if (enableStatus != MH_OK && enableStatus != MH_ERROR_ENABLED) {
        LogWarnf("MinHook EnableHook %s!XInputGetState failed: %d.", moduleName, enableStatus);
        return false;
    }
    Logf("%s XInputGetState hook installed.", moduleName);
    return true;
}

void InstallXInputHooks() {
    if (!g_hooks.Initialize()) {
        LogWarn("MinHook initialize failed for XInput hooks.");
        return;
    }

    HookXInputGetStateFromModule("xinput9_1_0.dll", &HookXInputGetState9, &g_originalXInputGetState9);
    HookXInputGetStateFromModule("xinput1_4.dll", &HookXInputGetState14, &g_originalXInputGetState14);
    HookXInputGetStateFromModule("xinput1_3.dll", &HookXInputGetState13, &g_originalXInputGetState13);
    HookXInputGetStateFromModule("xinput1_2.dll", &HookXInputGetState12, &g_originalXInputGetState12);
    HookXInputGetStateFromModule("xinput1_1.dll", &HookXInputGetState11, &g_originalXInputGetState11);

    if (g_originalXInputGetState9) {
        g_originalXInputGetState = g_originalXInputGetState9;
    } else if (g_originalXInputGetState14) {
        g_originalXInputGetState = g_originalXInputGetState14;
    } else if (g_originalXInputGetState13) {
        g_originalXInputGetState = g_originalXInputGetState13;
    } else if (g_originalXInputGetState12) {
        g_originalXInputGetState = g_originalXInputGetState12;
    } else if (g_originalXInputGetState11) {
        g_originalXInputGetState = g_originalXInputGetState11;
    }
}

bool InstallDirectInputHook() {
    IDirectInput8A* directInput = nullptr;
    HRESULT hr = DirectInput8Create(GetModuleHandleA(nullptr),
                                    DIRECTINPUT_VERSION,
                                    IID_IDirectInput8A,
                                    reinterpret_cast<void**>(&directInput),
                                    nullptr);
    if (FAILED(hr) || !directInput) {
        LogWarn("DirectInput8Create failed for hook discovery.");
        return false;
    }

    IDirectInputDevice8A* mouse = nullptr;
    hr = directInput->CreateDevice(GUID_SysMouse, &mouse, nullptr);
    if (FAILED(hr) || !mouse) {
        directInput->Release();
        LogWarn("DirectInput CreateDevice(GUID_SysMouse) failed for hook discovery.");
        return false;
    }

    IDirectInputDevice8A* keyboard = nullptr;
    hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, nullptr);
    if (FAILED(hr) || !keyboard) {
        mouse->Release();
        directInput->Release();
        LogWarn("DirectInput CreateDevice(GUID_SysKeyboard) failed for hook discovery.");
        return false;
    }

    void** mouseVtable = *reinterpret_cast<void***>(mouse);
    void** keyboardVtable = *reinterpret_cast<void***>(keyboard);
    void* mouseGetDeviceState = mouseVtable[9];
    void* mouseGetDeviceData = mouseVtable[10];
    void* keyboardGetDeviceState = keyboardVtable[9];
    void* keyboardGetDeviceData = keyboardVtable[10];

    if (!g_hooks.Initialize()) {
        LogWarn("MinHook initialize failed for DirectInput.");
        keyboard->Release(); mouse->Release(); directInput->Release();
        return false;
    }

    if (g_hooks.Create(mouseGetDeviceState, &HookGetDeviceState, reinterpret_cast<void**>(&g_originalGetDeviceStateMouse)) == MH_OK &&
        g_hooks.Enable(mouseGetDeviceState) == MH_OK) {
        Log("DirectInput mouse GetDeviceState hook installed.");
    } else {
        LogWarn("DirectInput mouse GetDeviceState hook failed.");
    }

    if (g_hooks.Create(mouseGetDeviceData, &HookGetDeviceData, reinterpret_cast<void**>(&g_originalGetDeviceDataMouse)) == MH_OK &&
        g_hooks.Enable(mouseGetDeviceData) == MH_OK) {
        Log("DirectInput mouse GetDeviceData hook installed.");
    } else {
        LogWarn("DirectInput mouse GetDeviceData hook failed.");
    }

    if (keyboardGetDeviceState == mouseGetDeviceState) {
        g_originalGetDeviceStateKeyboard = g_originalGetDeviceStateMouse;
        Log("DirectInput keyboard GetDeviceState shares mouse hook target.");
    } else if (g_hooks.Create(keyboardGetDeviceState, &HookGetDeviceState, reinterpret_cast<void**>(&g_originalGetDeviceStateKeyboard)) == MH_OK &&
               g_hooks.Enable(keyboardGetDeviceState) == MH_OK) {
        Log("DirectInput keyboard GetDeviceState hook installed.");
    } else {
        LogWarn("DirectInput keyboard GetDeviceState hook failed.");
    }

    if (keyboardGetDeviceData == mouseGetDeviceData) {
        g_originalGetDeviceDataKeyboard = g_originalGetDeviceDataMouse;
        Log("DirectInput keyboard GetDeviceData shares mouse hook target.");
    } else if (g_hooks.Create(keyboardGetDeviceData, &HookGetDeviceData, reinterpret_cast<void**>(&g_originalGetDeviceDataKeyboard)) == MH_OK &&
               g_hooks.Enable(keyboardGetDeviceData) == MH_OK) {
        Log("DirectInput keyboard GetDeviceData hook installed.");
    } else {
        LogWarn("DirectInput keyboard GetDeviceData hook failed.");
    }

    keyboard->Release();
    mouse->Release();
    directInput->Release();
    return g_originalGetDeviceStateMouse && g_originalGetDeviceDataMouse &&
        g_originalGetDeviceStateKeyboard && g_originalGetDeviceDataKeyboard;
}

bool InstallKeyboardStateHooks() {
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (!user32) {
        LogWarn("user32.dll not available for keyboard state hooks.");
        return false;
    }

    void* getAsyncKeyState = reinterpret_cast<void*>(GetProcAddress(user32, "GetAsyncKeyState"));
    void* getKeyState = reinterpret_cast<void*>(GetProcAddress(user32, "GetKeyState"));
    void* getKeyboardState = reinterpret_cast<void*>(GetProcAddress(user32, "GetKeyboardState"));
    if (!getAsyncKeyState || !getKeyState || !getKeyboardState) {
        LogWarn("Keyboard state hook discovery failed.");
        return false;
    }

    if (!g_hooks.Initialize()) {
        LogWarn("MinHook initialize failed for keyboard state hooks.");
        return false;
    }

    if (g_hooks.Create(getAsyncKeyState, &HookGetAsyncKeyState, reinterpret_cast<void**>(&g_originalGetAsyncKeyState)) == MH_OK &&
        g_hooks.Enable(getAsyncKeyState) == MH_OK) {
        Log("GetAsyncKeyState hook installed.");
    } else {
        LogWarn("GetAsyncKeyState hook failed.");
    }

    if (g_hooks.Create(getKeyState, &HookGetKeyState, reinterpret_cast<void**>(&g_originalGetKeyState)) == MH_OK &&
        g_hooks.Enable(getKeyState) == MH_OK) {
        Log("GetKeyState hook installed.");
    } else {
        LogWarn("GetKeyState hook failed.");
    }

    if (g_hooks.Create(getKeyboardState, &HookGetKeyboardState, reinterpret_cast<void**>(&g_originalGetKeyboardState)) == MH_OK &&
        g_hooks.Enable(getKeyboardState) == MH_OK) {
        Log("GetKeyboardState hook installed.");
    } else {
        LogWarn("GetKeyboardState hook failed.");
    }

    return g_originalGetAsyncKeyState && g_originalGetKeyState && g_originalGetKeyboardState;
}
