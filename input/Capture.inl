// Private unity fragment; included by AC4Tools.cpp inside its namespace.
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

bool IsControllerInputCaptureActive() {
    return g_menuOpen && g_disableControllerInputWhenUiOpen;
}

void RefreshInputCaptureState() {
    SetGameInputCaptured(IsMouseInputCaptureActive() ||
                         IsKeyboardInputCaptureActive() ||
                         IsControllerInputCaptureActive());
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

bool IsControllerDevice(IDirectInputDevice8A* device) {
    if (!device) {
        return false;
    }
    DIDEVCAPS caps{};
    caps.dwSize = sizeof(caps);
    if (FAILED(device->GetCapabilities(&caps))) {
        return false;
    }
    const DWORD deviceType = GET_DIDEVICE_TYPE(caps.dwDevType);
    return deviceType == DI8DEVTYPE_GAMEPAD || deviceType == DI8DEVTYPE_JOYSTICK;
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
