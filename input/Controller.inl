// Private unity fragment; included by AC4Tools.cpp inside its namespace.
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
        case VK_NUMPAD0: case VK_NUMPAD1: case VK_NUMPAD2: case VK_NUMPAD3:
        case VK_NUMPAD4: case VK_NUMPAD5: case VK_NUMPAD6: case VK_NUMPAD7:
        case VK_NUMPAD8: case VK_NUMPAD9:
            sprintf_s(out, 32, "Num%d", vk - VK_NUMPAD0);
            return out;
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

const int kBindableGamepadButtons[] = {
    kGamepadButtonA,
    kGamepadButtonB,
    kGamepadButtonX,
    kGamepadButtonY,
    kGamepadButtonDpadUp,
    kGamepadButtonDpadDown,
    kGamepadButtonDpadLeft,
    kGamepadButtonDpadRight,
    kGamepadButtonLeftShoulder,
    kGamepadButtonRightShoulder,
    kGamepadButtonLeftThumb,
    kGamepadButtonRightThumb,
    kGamepadButtonBack,
    kGamepadButtonStart,
    kGamepadButtonLeftTrigger,
    kGamepadButtonRightTrigger,
    kGamepadButtonLeftStickUp,
    kGamepadButtonLeftStickDown,
    kGamepadButtonLeftStickLeft,
    kGamepadButtonLeftStickRight,
    kGamepadButtonRightStickUp,
    kGamepadButtonRightStickDown,
    kGamepadButtonRightStickLeft,
    kGamepadButtonRightStickRight,
};

const char* GamepadButtonName(int button) {
    switch (button) {
        case 0: return "None";
        case kGamepadButtonA: return "Pad A";
        case kGamepadButtonB: return "Pad B";
        case kGamepadButtonX: return "Pad X";
        case kGamepadButtonY: return "Pad Y";
        case kGamepadButtonDpadUp: return "Pad D-Up";
        case kGamepadButtonDpadDown: return "Pad D-Down";
        case kGamepadButtonDpadLeft: return "Pad D-Left";
        case kGamepadButtonDpadRight: return "Pad D-Right";
        case kGamepadButtonLeftShoulder: return "Pad LB";
        case kGamepadButtonRightShoulder: return "Pad RB";
        case kGamepadButtonLeftThumb: return "Pad L3";
        case kGamepadButtonRightThumb: return "Pad R3";
        case kGamepadButtonBack: return "Pad Back";
        case kGamepadButtonStart: return "Pad Start";
        case kGamepadButtonLeftTrigger: return "Pad LT";
        case kGamepadButtonRightTrigger: return "Pad RT";
        case kGamepadButtonLeftStickUp: return "Pad LS Up";
        case kGamepadButtonLeftStickDown: return "Pad LS Down";
        case kGamepadButtonLeftStickLeft: return "Pad LS Left";
        case kGamepadButtonLeftStickRight: return "Pad LS Right";
        case kGamepadButtonRightStickUp: return "Pad RS Up";
        case kGamepadButtonRightStickDown: return "Pad RS Down";
        case kGamepadButtonRightStickLeft: return "Pad RS Left";
        case kGamepadButtonRightStickRight: return "Pad RS Right";
        default: return "Pad ?";
    }
}

int GamepadButtonIndex(int button) {
    for (int i = 0; i < static_cast<int>(sizeof(kBindableGamepadButtons) / sizeof(kBindableGamepadButtons[0])); ++i) {
        if (kBindableGamepadButtons[i] == button) {
            return i;
        }
    }
    return -1;
}

bool IsDirectInputGamepadButtonDown(int button) {
    const int index = GamepadButtonIndex(button);
    return index >= 0 && index < static_cast<int>(sizeof(g_directInputGamepadButtons) / sizeof(g_directInputGamepadButtons[0])) &&
           g_directInputGamepadButtons[index];
}

bool WasDirectInputGamepadButtonJustPressed(int button) {
    const int index = GamepadButtonIndex(button);
    return index >= 0 &&
           index < static_cast<int>(sizeof(g_directInputGamepadButtons) / sizeof(g_directInputGamepadButtons[0])) &&
           g_directInputGamepadButtons[index] &&
           !g_previousDirectInputGamepadButtons[index];
}

void SetDirectInputGamepadButtonState(int button, bool down) {
    const int index = GamepadButtonIndex(button);
    if (index < 0 || index >= static_cast<int>(sizeof(g_directInputGamepadButtons) / sizeof(g_directInputGamepadButtons[0]))) {
        return;
    }
    g_directInputGamepadButtons[index] = down;
    if (down) {
        g_directInputGamepadActive = true;
    }
}

bool IsDirectInputPovDirectionActive(DWORD povValue, int direction) {
    if (LOWORD(povValue) == 0xFFFF) {
        return false;
    }
    switch (direction) {
        case 0: return povValue <= 4500 || povValue >= 31500;
        case 1: return povValue >= 13500 && povValue <= 22500;
        case 2: return povValue >= 22500 && povValue <= 31500;
        case 3: return povValue >= 4500 && povValue <= 13500;
        default: return false;
    }
}

bool IsDirectInputAxisHigh(LONG value) {
    return value >= 45000;
}

bool IsDirectInputAxisLow(LONG value) {
    return value <= 20535;
}

void ReleasePolledControllerDevices() {
    for (int i = 0; i < g_controllerDeviceCount; ++i) {
        if (g_controllerDevices[i]) {
            g_controllerDevices[i]->Unacquire();
            g_controllerDevices[i]->Release();
            g_controllerDevices[i] = nullptr;
        }
    }
    g_controllerDeviceCount = 0;
    if (g_controllerDirectInput) {
        g_controllerDirectInput->Release();
        g_controllerDirectInput = nullptr;
    }
    g_controllerPollingInitialized = false;
}

BOOL CALLBACK EnumControllerDevicesCallback(const DIDEVICEINSTANCEA* instance, VOID* context) {
    (void)context;
    if (!g_controllerDirectInput || !instance || g_controllerDeviceCount >= kMaxPolledControllers) {
        return DIENUM_STOP;
    }

    IDirectInputDevice8A* device = nullptr;
    if (FAILED(g_controllerDirectInput->CreateDevice(instance->guidInstance, &device, nullptr)) || !device) {
        return DIENUM_CONTINUE;
    }

    if (FAILED(device->SetDataFormat(&c_dfDIJoystick2))) {
        device->Release();
        return DIENUM_CONTINUE;
    }

    HWND coopWindow = g_gameWindow ? g_gameWindow : GetDesktopWindow();
    if (FAILED(device->SetCooperativeLevel(coopWindow, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
        device->Release();
        return DIENUM_CONTINUE;
    }

    device->Acquire();
    g_controllerDevices[g_controllerDeviceCount++] = device;
    return g_controllerDeviceCount >= kMaxPolledControllers ? DIENUM_STOP : DIENUM_CONTINUE;
}

void BeginControllerPollingFrame() {
    for (int i = 0; i < static_cast<int>(sizeof(g_directInputGamepadButtons) / sizeof(g_directInputGamepadButtons[0])); ++i) {
        g_previousDirectInputGamepadButtons[i] = g_directInputGamepadButtons[i];
        g_directInputGamepadButtons[i] = false;
    }
    g_previousDirectInputGamepadActive = g_directInputGamepadActive;
    g_directInputGamepadActive = false;
}

void InitializeControllerPolling() {
    if (g_controllerPollingInitialized) {
        return;
    }
    ReleasePolledControllerDevices();
    HRESULT hr = DirectInput8Create(GetModuleHandleA(nullptr),
                                    DIRECTINPUT_VERSION,
                                    IID_IDirectInput8A,
                                    reinterpret_cast<void**>(&g_controllerDirectInput),
                                    nullptr);
    if (FAILED(hr) || !g_controllerDirectInput) {
        LogWarn("DirectInput8Create failed for controller polling.");
        return;
    }

    hr = g_controllerDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                              EnumControllerDevicesCallback,
                                              nullptr,
                                              DIEDFL_ATTACHEDONLY);
    if (FAILED(hr)) {
        LogWarn("DirectInput controller enumeration failed.");
        ReleasePolledControllerDevices();
        return;
    }

    g_controllerPollingInitialized = true;
    Logf("Controller polling initialized with %d DirectInput device(s).", g_controllerDeviceCount);
}

void PollControllerDevices() {
    if (!g_controllerPollingInitialized) {
        InitializeControllerPolling();
    }
    for (int i = 0; i < g_controllerDeviceCount; ++i) {
        IDirectInputDevice8A* device = g_controllerDevices[i];
        if (!device) {
            continue;
        }

        DIJOYSTATE2 state{};
        HRESULT hr = device->Poll();
        if (FAILED(hr)) {
            device->Acquire();
        }
        hr = device->GetDeviceState(sizeof(state), &state);
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            if (SUCCEEDED(device->Acquire())) {
                hr = device->GetDeviceState(sizeof(state), &state);
            }
        }
        if (SUCCEEDED(hr)) {
            CaptureDirectInputControllerStateFromJoystick(state);
        }
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
