// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void AssignMenuHotkey(int vk) {
    if (vk != 0 && !IsBindableHotkey(vk)) {
        return;
    }
    if (vk != 0) {
        ClearKeyboardHotkeyConflicts(vk, -2, -2);
    }
    g_menuHotkey = vk;
    g_suppressedHotkeyVk = vk;
    SaveConfig();
    Logf("Hotkey for Open/Close Menu set to %s.", HotkeyName(vk));
}

void AssignMenuGamepadHotkey(int button) {
    if (button != 0 && !IsBindableGamepadButton(button)) {
        return;
    }
    if (button != 0) {
        ClearGamepadHotkeyConflicts(button, -2, -2);
    }
    g_menuGamepadHotkey = button;
    g_suppressedHotkeyGamepadButton = button;
    SaveConfig();
    Logf("Gamepad hotkey for Open/Close Menu set to %s.", GamepadButtonName(button));
}

int FreeCamControlCaptureId(int controlIndex) {
    return kFreeCamControlCaptureBase - controlIndex;
}

int FreeCamControlGamepadCaptureId(int controlIndex) {
    return kFreeCamControlGamepadCaptureBase - controlIndex;
}

int ActionGamepadCaptureId(int actionIndex) {
    return kActionGamepadCaptureBase - actionIndex;
}

bool GetFreeCamControlCaptureIndex(int captureId, int& controlIndex) {
    if (captureId > kFreeCamControlCaptureBase ||
        captureId <= kFreeCamControlCaptureBase - kFreeCamControlCount) {
        return false;
    }
    controlIndex = kFreeCamControlCaptureBase - captureId;
    return controlIndex >= 0 && controlIndex < kFreeCamControlCount;
}

bool GetFreeCamControlGamepadCaptureIndex(int captureId, int& controlIndex) {
    if (captureId > kFreeCamControlGamepadCaptureBase ||
        captureId <= kFreeCamControlGamepadCaptureBase - kFreeCamControlCount) {
        return false;
    }
    controlIndex = kFreeCamControlGamepadCaptureBase - captureId;
    return controlIndex >= 0 && controlIndex < kFreeCamControlCount;
}

bool GetActionGamepadCaptureIndex(int captureId, int& actionIndex) {
    if (captureId > kActionGamepadCaptureBase ||
        captureId <= kActionGamepadCaptureBase - kActionCount) {
        return false;
    }
    actionIndex = kActionGamepadCaptureBase - captureId;
    return actionIndex >= 0 && actionIndex < kActionCount;
}

void ClearKeyboardHotkeyConflicts(int vk, int actionToSkip = -1, int freeCamControlToSkip = -1) {
    if (vk == 0) {
        return;
    }
    if (g_menuHotkey == vk && actionToSkip != -2 && freeCamControlToSkip != -2) {
        g_menuHotkey = 0;
    }
    for (int i = 0; i < kActionCount; ++i) {
        if (i != actionToSkip && g_actions[i].hotkey == vk) {
            g_actions[i].hotkey = 0;
        }
    }
    for (int i = 0; i < kFreeCamControlCount; ++i) {
        if (i != freeCamControlToSkip && g_freeCamControls[i].key == vk) {
            g_freeCamControls[i].key = 0;
        }
    }
}

void AccumulateMouseWheelDelta(LONG delta) {
    if (delta == 0) {
        return;
    }
    g_pendingMouseWheel += static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
}

void AccumulateMouseWheelDelta(LPVOID data, DWORD dataSize) {
    if (!data || dataSize <= 0) {
        return;
    }
    if (dataSize >= sizeof(DIMOUSESTATE2)) {
        auto* state = static_cast<DIMOUSESTATE2*>(data);
        AccumulateMouseWheelDelta(state->lZ);
    } else if (dataSize >= sizeof(DIMOUSESTATE)) {
        auto* state = static_cast<DIMOUSESTATE*>(data);
        AccumulateMouseWheelDelta(state->lZ);
    }
}

void AccumulateMouseWheelDelta(LPDIDEVICEOBJECTDATA objectData, DWORD count) {
    if (!objectData) {
        return;
    }
    for (DWORD i = 0; i < count; ++i) {
        if (objectData[i].dwOfs == DIMOFS_Z) {
            AccumulateMouseWheelDelta(static_cast<LONG>(objectData[i].dwData));
        }
    }
}

void CaptureDirectInputControllerStateFromJoystick(const DIJOYSTATE2& state) {
    SetDirectInputGamepadButtonState(kGamepadButtonA, state.rgbButtons[0] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonB, state.rgbButtons[1] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonX, state.rgbButtons[2] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonY, state.rgbButtons[3] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonLeftShoulder, state.rgbButtons[4] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonRightShoulder, state.rgbButtons[5] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonBack, state.rgbButtons[6] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonStart, state.rgbButtons[7] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonLeftThumb, state.rgbButtons[8] != 0);
    SetDirectInputGamepadButtonState(kGamepadButtonRightThumb, state.rgbButtons[9] != 0);

    const DWORD pov = state.rgdwPOV[0];
    SetDirectInputGamepadButtonState(kGamepadButtonDpadUp, IsDirectInputPovDirectionActive(pov, 0));
    SetDirectInputGamepadButtonState(kGamepadButtonDpadDown, IsDirectInputPovDirectionActive(pov, 1));
    SetDirectInputGamepadButtonState(kGamepadButtonDpadLeft, IsDirectInputPovDirectionActive(pov, 2));
    SetDirectInputGamepadButtonState(kGamepadButtonDpadRight, IsDirectInputPovDirectionActive(pov, 3));

    SetDirectInputGamepadButtonState(kGamepadButtonLeftStickLeft, IsDirectInputAxisLow(state.lX));
    SetDirectInputGamepadButtonState(kGamepadButtonLeftStickRight, IsDirectInputAxisHigh(state.lX));
    SetDirectInputGamepadButtonState(kGamepadButtonLeftStickUp, IsDirectInputAxisLow(state.lY));
    SetDirectInputGamepadButtonState(kGamepadButtonLeftStickDown, IsDirectInputAxisHigh(state.lY));
    SetDirectInputGamepadButtonState(kGamepadButtonRightStickLeft, IsDirectInputAxisLow(state.lRx));
    SetDirectInputGamepadButtonState(kGamepadButtonRightStickRight, IsDirectInputAxisHigh(state.lRx));
    SetDirectInputGamepadButtonState(kGamepadButtonRightStickUp, IsDirectInputAxisLow(state.lRy));
    SetDirectInputGamepadButtonState(kGamepadButtonRightStickDown, IsDirectInputAxisHigh(state.lRy));
    SetDirectInputGamepadButtonState(kGamepadButtonLeftTrigger, IsDirectInputAxisLow(state.lZ));
    SetDirectInputGamepadButtonState(kGamepadButtonRightTrigger, IsDirectInputAxisHigh(state.lZ));
}

void CaptureDirectInputControllerState(LPVOID data, DWORD dataSize) {
    if (!data || dataSize < sizeof(DIJOYSTATE2)) {
        return;
    }
    CaptureDirectInputControllerStateFromJoystick(*static_cast<DIJOYSTATE2*>(data));
}

void CaptureDirectInputControllerState(LPDIDEVICEOBJECTDATA objectData, DWORD count) {
    if (!objectData) {
        return;
    }
    for (DWORD i = 0; i < count; ++i) {
        const DWORD ofs = objectData[i].dwOfs;
        const DWORD data = objectData[i].dwData;
        if (ofs >= DIJOFS_BUTTON0 && ofs < DIJOFS_BUTTON0 + 10) {
            const int buttonIndex = static_cast<int>(ofs - DIJOFS_BUTTON0);
            switch (buttonIndex) {
                case 0: SetDirectInputGamepadButtonState(kGamepadButtonA, (data & 0x80) != 0); break;
                case 1: SetDirectInputGamepadButtonState(kGamepadButtonB, (data & 0x80) != 0); break;
                case 2: SetDirectInputGamepadButtonState(kGamepadButtonX, (data & 0x80) != 0); break;
                case 3: SetDirectInputGamepadButtonState(kGamepadButtonY, (data & 0x80) != 0); break;
                case 4: SetDirectInputGamepadButtonState(kGamepadButtonLeftShoulder, (data & 0x80) != 0); break;
                case 5: SetDirectInputGamepadButtonState(kGamepadButtonRightShoulder, (data & 0x80) != 0); break;
                case 6: SetDirectInputGamepadButtonState(kGamepadButtonBack, (data & 0x80) != 0); break;
                case 7: SetDirectInputGamepadButtonState(kGamepadButtonStart, (data & 0x80) != 0); break;
                case 8: SetDirectInputGamepadButtonState(kGamepadButtonLeftThumb, (data & 0x80) != 0); break;
                case 9: SetDirectInputGamepadButtonState(kGamepadButtonRightThumb, (data & 0x80) != 0); break;
                default: break;
            }
            continue;
        }

        if (ofs == DIJOFS_POV(0)) {
            SetDirectInputGamepadButtonState(kGamepadButtonDpadUp, IsDirectInputPovDirectionActive(data, 0));
            SetDirectInputGamepadButtonState(kGamepadButtonDpadDown, IsDirectInputPovDirectionActive(data, 1));
            SetDirectInputGamepadButtonState(kGamepadButtonDpadLeft, IsDirectInputPovDirectionActive(data, 2));
            SetDirectInputGamepadButtonState(kGamepadButtonDpadRight, IsDirectInputPovDirectionActive(data, 3));
            continue;
        }

        const LONG axisValue = static_cast<LONG>(data);
        if (ofs == DIJOFS_X) {
            SetDirectInputGamepadButtonState(kGamepadButtonLeftStickLeft, IsDirectInputAxisLow(axisValue));
            SetDirectInputGamepadButtonState(kGamepadButtonLeftStickRight, IsDirectInputAxisHigh(axisValue));
        } else if (ofs == DIJOFS_Y) {
            SetDirectInputGamepadButtonState(kGamepadButtonLeftStickUp, IsDirectInputAxisLow(axisValue));
            SetDirectInputGamepadButtonState(kGamepadButtonLeftStickDown, IsDirectInputAxisHigh(axisValue));
        } else if (ofs == DIJOFS_RX) {
            SetDirectInputGamepadButtonState(kGamepadButtonRightStickLeft, IsDirectInputAxisLow(axisValue));
            SetDirectInputGamepadButtonState(kGamepadButtonRightStickRight, IsDirectInputAxisHigh(axisValue));
        } else if (ofs == DIJOFS_RY) {
            SetDirectInputGamepadButtonState(kGamepadButtonRightStickUp, IsDirectInputAxisLow(axisValue));
            SetDirectInputGamepadButtonState(kGamepadButtonRightStickDown, IsDirectInputAxisHigh(axisValue));
        } else if (ofs == DIJOFS_Z) {
            SetDirectInputGamepadButtonState(kGamepadButtonLeftTrigger, IsDirectInputAxisLow(axisValue));
            SetDirectInputGamepadButtonState(kGamepadButtonRightTrigger, IsDirectInputAxisHigh(axisValue));
        }
    }
}

void ClearGamepadHotkeyConflicts(int button, int actionToSkip = -1, int freeCamControlToSkip = -1) {
    if (button == 0) {
        return;
    }
    if (g_menuGamepadHotkey == button && actionToSkip != -2 && freeCamControlToSkip != -2) {
        g_menuGamepadHotkey = 0;
    }
    for (int i = 0; i < kActionCount; ++i) {
        if (i != actionToSkip && g_actions[i].gamepadHotkey == button) {
            g_actions[i].gamepadHotkey = 0;
        }
    }
    for (int i = 0; i < kFreeCamControlCount; ++i) {
        if (i != freeCamControlToSkip && g_freeCamControls[i].gamepadButton == button) {
            g_freeCamControls[i].gamepadButton = 0;
        }
    }
}

void AssignHotkey(int actionIndex, int vk) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    if (vk != 0 && !IsBindableHotkey(vk)) {
        return;
    }
    if (vk != 0) {
        ClearKeyboardHotkeyConflicts(vk, actionIndex, -1);
    }
    g_actions[actionIndex].hotkey = vk;
    g_suppressedHotkeyVk = vk;
    SaveConfig();
    Logf("Hotkey for %s set to %s.", g_actions[actionIndex].label, HotkeyName(vk));
}

void AssignGamepadHotkey(int actionIndex, int button) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    if (button != 0 && !IsBindableGamepadButton(button)) {
        return;
    }
    if (button != 0) {
        ClearGamepadHotkeyConflicts(button, actionIndex, -1);
    }
    g_actions[actionIndex].gamepadHotkey = button;
    g_suppressedHotkeyGamepadButton = button;
    SaveConfig();
    Logf("Gamepad hotkey for %s set to %s.", g_actions[actionIndex].label, GamepadButtonName(button));
}

void AssignFreeCamControlHotkey(int controlIndex, int vk) {
    if (controlIndex < 0 || controlIndex >= kFreeCamControlCount) {
        return;
    }
    if (vk != 0 && !IsBindableHotkey(vk)) {
        return;
    }
    if (vk != 0) {
        ClearKeyboardHotkeyConflicts(vk, -1, controlIndex);
    }
    g_freeCamControls[controlIndex].key = vk;
    g_suppressedHotkeyVk = vk;
    SaveConfig();
    Logf("Free Cam control %s set to %s.", g_freeCamControls[controlIndex].label, HotkeyName(vk));
}

void AssignFreeCamControlGamepadHotkey(int controlIndex, int button) {
    if (controlIndex < 0 || controlIndex >= kFreeCamControlCount) {
        return;
    }
    if (button != 0 && !IsBindableGamepadButton(button)) {
        return;
    }
    if (button != 0) {
        ClearGamepadHotkeyConflicts(button, -1, controlIndex);
    }
    g_freeCamControls[controlIndex].gamepadButton = button;
    g_suppressedHotkeyGamepadButton = button;
    SaveConfig();
    Logf("Free Cam control %s set to %s.", g_freeCamControls[controlIndex].label, GamepadButtonName(button));
}

void ToggleActionFromHotkey(int actionIndex) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    ToggleAction& action = g_actions[actionIndex];
    if (!action.ready || !*action.ready) {
        return;
    }
    if (!action.value) {
        if (action.afterToggle) {
            action.afterToggle();
            Logf("%s triggered from hotkey %s.", action.label, HotkeyName(action.hotkey));
        }
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

void TriggerActionFromGamepadHotkey(int actionIndex) {
    if (actionIndex < 0 || actionIndex >= kActionCount) {
        return;
    }
    ToggleAction& action = g_actions[actionIndex];
    if (!action.ready || !*action.ready) {
        return;
    }
    if (!action.value) {
        if (action.afterToggle) {
            action.afterToggle();
            Logf("%s triggered from gamepad hotkey %s.", action.label, GamepadButtonName(action.gamepadHotkey));
        }
        return;
    }
    *action.value = !*action.value;
    if (action.afterToggle) {
        action.afterToggle();
    }
    Logf("%s toggled %s from gamepad hotkey %s.",
         action.label,
         *action.value ? "ON" : "OFF",
         GamepadButtonName(action.gamepadHotkey));
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

    for (int i = 0; i < kActionCount; ++i) {
        const int button = g_actions[i].gamepadHotkey;
        if (button == 0) {
            continue;
        }
        if (g_suppressedHotkeyGamepadButton == button) {
            continue;
        }
        if (WasGamepadButtonJustPressed(button)) {
            TriggerActionFromGamepadHotkey(i);
        }
    }
}

void ProcessGamepadCapture() {
    if (!g_menuOpen || g_hotkeyCaptureAction == -1) {
        return;
    }

    for (int i = 0; i < static_cast<int>(sizeof(kBindableGamepadButtons) / sizeof(kBindableGamepadButtons[0])); ++i) {
        const int button = kBindableGamepadButtons[i];
        if (!WasGamepadButtonJustPressed(button)) {
            continue;
        }

        int freeCamControlIndex = -1;
        int actionIndex = -1;
        int freeCamControlGamepadIndex = -1;
        if (g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture) {
            AssignMenuGamepadHotkey(button);
        } else if (GetActionGamepadCaptureIndex(g_hotkeyCaptureAction, actionIndex)) {
            AssignGamepadHotkey(actionIndex, button);
        } else if (GetFreeCamControlGamepadCaptureIndex(g_hotkeyCaptureAction, freeCamControlGamepadIndex)) {
            AssignFreeCamControlGamepadHotkey(freeCamControlGamepadIndex, button);
        } else if (g_hotkeyCaptureAction == kMenuHotkeyCapture ||
                   GetFreeCamControlCaptureIndex(g_hotkeyCaptureAction, freeCamControlIndex) ||
                   (g_hotkeyCaptureAction >= 0 && g_hotkeyCaptureAction < kActionCount)) {
            return;
        } else {
            return;
        }

        g_hotkeyCaptureAction = -1;
        return;
    }
}
