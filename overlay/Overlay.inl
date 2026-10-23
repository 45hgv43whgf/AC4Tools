// Private unity fragment; included by AC4Tools.cpp inside its namespace.
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

void ReleaseRenderTarget() {
    if (g_renderTarget) {
        g_renderTarget->Release();
        g_renderTarget = nullptr;
    }
}

bool CreateRenderTarget(IDXGISwapChain* swapChain) {
    ReleaseRenderTarget();
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))) ||
        !backBuffer) {
        LogWarn("DX11 back buffer unavailable; AC4Tools UI render target was not created.");
        return false;
    }
    const HRESULT hr = g_device->CreateRenderTargetView(backBuffer, nullptr, &g_renderTarget);
    backBuffer->Release();
    if (FAILED(hr) || !g_renderTarget) {
        LogWarn("DX11 render target creation failed; AC4Tools UI may not be visible.");
        return false;
    }
    return true;
}

bool IsBindableGamepadButton(int button) {
    for (int i = 0; i < static_cast<int>(sizeof(kBindableGamepadButtons) / sizeof(kBindableGamepadButtons[0])); ++i) {
        if (kBindableGamepadButtons[i] == button) {
            return true;
        }
    }
    return false;
}

bool IsGamepadButtonDownInState(const XINPUT_STATE& state, int button) {
    switch (button) {
        case kGamepadButtonA: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
        case kGamepadButtonB: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
        case kGamepadButtonX: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
        case kGamepadButtonY: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
        case kGamepadButtonDpadUp: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
        case kGamepadButtonDpadDown: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
        case kGamepadButtonDpadLeft: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
        case kGamepadButtonDpadRight: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
        case kGamepadButtonLeftShoulder: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
        case kGamepadButtonRightShoulder: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
        case kGamepadButtonLeftThumb: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
        case kGamepadButtonRightThumb: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
        case kGamepadButtonBack: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
        case kGamepadButtonStart: return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
        case kGamepadButtonLeftTrigger: return state.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        case kGamepadButtonRightTrigger: return state.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
        case kGamepadButtonLeftStickUp: return state.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        case kGamepadButtonLeftStickDown: return state.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        case kGamepadButtonLeftStickLeft: return state.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        case kGamepadButtonLeftStickRight: return state.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        case kGamepadButtonRightStickUp: return state.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
        case kGamepadButtonRightStickDown: return state.Gamepad.sThumbRY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
        case kGamepadButtonRightStickLeft: return state.Gamepad.sThumbRX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
        case kGamepadButtonRightStickRight: return state.Gamepad.sThumbRX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
        default: return false;
    }
}

void UpdateGamepadStates() {
    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        g_previousGamepadStates[i] = g_gamepadStates[i];
        g_previousGamepadConnected[i] = g_gamepadConnected[i];
        ZeroMemory(&g_gamepadStates[i], sizeof(g_gamepadStates[i]));
        XInputGetStateFn getState = g_originalXInputGetState;
        g_gamepadConnected[i] = getState && getState(i, &g_gamepadStates[i]) == ERROR_SUCCESS;
        if (!g_gamepadConnected[i]) {
            ZeroMemory(&g_gamepadStates[i], sizeof(g_gamepadStates[i]));
        }
    }
}

bool IsGamepadButtonDown(int button) {
    if (!IsBindableGamepadButton(button)) {
        return false;
    }
    if (IsDirectInputGamepadButtonDown(button)) {
        return true;
    }
    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        if (g_gamepadConnected[i] && IsGamepadButtonDownInState(g_gamepadStates[i], button)) {
            return true;
        }
    }
    return false;
}

bool WasGamepadButtonJustPressed(int button) {
    if (!IsBindableGamepadButton(button)) {
        return false;
    }
    if (WasDirectInputGamepadButtonJustPressed(button)) {
        return true;
    }
    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        const bool downNow = g_gamepadConnected[i] && IsGamepadButtonDownInState(g_gamepadStates[i], button);
        const bool downBefore = g_previousGamepadConnected[i] && IsGamepadButtonDownInState(g_previousGamepadStates[i], button);
        if (downNow && !downBefore) {
            return true;
        }
    }
    return false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    StateGuard stateGuard(g_stateLock, true);
    if (!stateGuard || g_stopping.load()) return CallWindowProcA(g_originalWndProc, hwnd, msg, wparam, lparam);
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
            int freeCamControlIndex = -1;
            int gamepadActionIndex = -1;
            int freeCamGamepadControlIndex = -1;
            const bool isFreeCamControlCapture =
                GetFreeCamControlCaptureIndex(g_hotkeyCaptureAction, freeCamControlIndex);
            const bool isActionGamepadCapture =
                GetActionGamepadCaptureIndex(g_hotkeyCaptureAction, gamepadActionIndex);
            const bool isFreeCamControlGamepadCapture =
                GetFreeCamControlGamepadCaptureIndex(g_hotkeyCaptureAction, freeCamGamepadControlIndex);
            if (vk == VK_ESCAPE) {
                g_hotkeyCaptureAction = -1;
                Log("Hotkey capture cancelled.");
            } else if (vk == VK_BACK || vk == VK_DELETE) {
                if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                    AssignMenuHotkey(0);
                } else if (g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture) {
                    AssignMenuGamepadHotkey(0);
                } else if (isFreeCamControlCapture) {
                    AssignFreeCamControlHotkey(freeCamControlIndex, 0);
                } else if (isFreeCamControlGamepadCapture) {
                    AssignFreeCamControlGamepadHotkey(freeCamGamepadControlIndex, 0);
                } else if (isActionGamepadCapture) {
                    AssignGamepadHotkey(gamepadActionIndex, 0);
                } else {
                    AssignHotkey(g_hotkeyCaptureAction, 0);
                }
                g_hotkeyCaptureAction = -1;
            } else if (IsBindableHotkey(vk)) {
                if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                    AssignMenuHotkey(vk);
                } else if (isFreeCamControlCapture) {
                    AssignFreeCamControlHotkey(freeCamControlIndex, vk);
                } else if (g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture ||
                           isActionGamepadCapture ||
                           isFreeCamControlGamepadCapture) {
                    return true;
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
    PublishCaveState();
    stateGuard.unlock();
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

    // Note for future overlay troubleshooting:
    // We briefly tested a Present-time "re-hook" here that checked the current
    // GWLP_WNDPROC every frame and reinstalled AC4Tools' WndProc if another
    // module replaced it after startup. That experiment was added while
    // investigating a report that mouse input was not being blocked while the
    // UI was open, but the report turned out to be from an unsupported
    // AC4BFSP.exe build with a different SHA256 and many rejected patches.
    // Because the issue was not on the supported executable, we removed the
    // runtime re-hook from the official build to avoid carrying extra window
    // subclassing behavior in a sensitive code path without a confirmed need.
    // If a supported-build overlay conflict ever appears, restore the helper
    // that re-checks GWLP_WNDPROC and call it from HookPresent() after ImGui
    // initialization.
    // Publish the forwarding proc before installing our callback: WndProc can
    // arrive on the window thread while Present still holds the state lock.
    g_originalWndProc.store(reinterpret_cast<WNDPROC>(GetWindowLongPtrA(g_gameWindow, GWLP_WNDPROC)));
    SetLastError(0);
    const LONG_PTR previousProc = SetWindowLongPtrA(g_gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));
    if (!previousProc && GetLastError() != 0) {
        ImGui_ImplDX11_Shutdown(); ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext();
        ReleaseRenderTarget(); g_context->Release(); g_context = nullptr;
        g_device->Release(); g_device = nullptr;
        return false;
    }
    if (previousProc) g_originalWndProc.store(reinterpret_cast<WNDPROC>(previousProc));
    g_imguiReady = true;
    Log("Dear ImGui DX11 overlay initialized.");
    if (g_installed) {
        Log("AC4Tools patches installed.");
    } else {
        LogWarn("AC4Tools patches not installed.");
    }
    return true;
}

void DrawHotkeysTab() {
    if (ImGui::BeginTable("HotkeyTable", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Function", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("Keyboard", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Gamepad", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Set", ImGuiTableColumnFlags_WidthFixed, 220.0f);
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
        if (g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture) {
            ImGui::TextUnformatted("Press button...");
        } else {
            ImGui::TextUnformatted(GamepadButtonName(g_menuGamepadHotkey));
        }
        ImGui::TableSetColumnIndex(4);
        ImGui::PushID("MenuOpen");
        if (ImGui::Button(g_hotkeyCaptureAction == kMenuHotkeyCapture ? "Cancel" : "Set Key", ImVec2(64.0f, 0.0f))) {
            if (g_hotkeyCaptureAction == kMenuHotkeyCapture) {
                g_hotkeyCaptureAction = -1;
            } else {
                g_hotkeyCaptureAction = kMenuHotkeyCapture;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture ? "Cancel" : "Set Pad", ImVec2(64.0f, 0.0f))) {
            if (g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture) {
                g_hotkeyCaptureAction = -1;
            } else {
                g_hotkeyCaptureAction = kMenuGamepadHotkeyCapture;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear", ImVec2(64.0f, 0.0f))) {
            AssignMenuHotkey(0);
            AssignMenuGamepadHotkey(0);
            if (g_hotkeyCaptureAction == kMenuHotkeyCapture ||
                g_hotkeyCaptureAction == kMenuGamepadHotkeyCapture) {
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
            } else if (!action.value) {
                ImGui::TextUnformatted("Action");
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
            if (g_hotkeyCaptureAction == ActionGamepadCaptureId(i)) {
                ImGui::TextUnformatted("Press button...");
            } else {
                ImGui::TextUnformatted(GamepadButtonName(action.gamepadHotkey));
            }

            ImGui::TableSetColumnIndex(4);
            ImGui::PushID(i);
            if (ImGui::Button(g_hotkeyCaptureAction == i ? "Cancel" : "Set Key", ImVec2(64.0f, 0.0f))) {
                if (g_hotkeyCaptureAction == i) {
                    g_hotkeyCaptureAction = -1;
                } else {
                    g_hotkeyCaptureAction = i;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(g_hotkeyCaptureAction == ActionGamepadCaptureId(i) ? "Cancel" : "Set Pad", ImVec2(64.0f, 0.0f))) {
                if (g_hotkeyCaptureAction == ActionGamepadCaptureId(i)) {
                    g_hotkeyCaptureAction = -1;
                } else {
                    g_hotkeyCaptureAction = ActionGamepadCaptureId(i);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear", ImVec2(64.0f, 0.0f))) {
                AssignHotkey(i, 0);
                AssignGamepadHotkey(i, 0);
                if (g_hotkeyCaptureAction == i ||
                    g_hotkeyCaptureAction == ActionGamepadCaptureId(i)) {
                    g_hotkeyCaptureAction = -1;
                }
            }
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextUnformatted("Free Cam Control Hotkeys");
    ImGui::TextDisabled("These are held while Free Cam is active; they are not toggle hotkeys.");
    if (ImGui::BeginTable("FreeCamControlHotkeyTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Keyboard", ImGuiTableColumnFlags_WidthFixed, 110.0f);
        ImGui::TableSetupColumn("Gamepad", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Set", ImGuiTableColumnFlags_WidthFixed, 220.0f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < kFreeCamControlCount; ++i) {
            const int captureId = FreeCamControlCaptureId(i);
            const int gamepadCaptureId = FreeCamControlGamepadCaptureId(i);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(g_freeCamControls[i].label);

            ImGui::TableSetColumnIndex(1);
            if (g_hotkeyCaptureAction == captureId) {
                ImGui::TextUnformatted("Press key...");
            } else {
                ImGui::TextUnformatted(HotkeyName(g_freeCamControls[i].key));
            }

            ImGui::TableSetColumnIndex(2);
            if (g_hotkeyCaptureAction == gamepadCaptureId) {
                ImGui::TextUnformatted("Press button...");
            } else {
                ImGui::TextUnformatted(GamepadButtonName(g_freeCamControls[i].gamepadButton));
            }

            ImGui::TableSetColumnIndex(3);
            ImGui::PushID(captureId);
            if (ImGui::Button(g_hotkeyCaptureAction == captureId ? "Cancel" : "Set Key", ImVec2(64.0f, 0.0f))) {
                if (g_hotkeyCaptureAction == captureId) {
                    g_hotkeyCaptureAction = -1;
                } else {
                    g_hotkeyCaptureAction = captureId;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(g_hotkeyCaptureAction == gamepadCaptureId ? "Cancel" : "Set Pad", ImVec2(64.0f, 0.0f))) {
                if (g_hotkeyCaptureAction == gamepadCaptureId) {
                    g_hotkeyCaptureAction = -1;
                } else {
                    g_hotkeyCaptureAction = gamepadCaptureId;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear", ImVec2(64.0f, 0.0f))) {
                AssignFreeCamControlHotkey(i, 0);
                AssignFreeCamControlGamepadHotkey(i, 0);
                if (g_hotkeyCaptureAction == captureId ||
                    g_hotkeyCaptureAction == gamepadCaptureId) {
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

    if (!g_supportedBuild) ImGui::TextWrapped("Unsupported executable: all gameplay features are disabled. See System for the detected SHA256.");
    if (ImGui::BeginTabBar("Tabs")) {
        ImGui::BeginDisabled(!g_supportedBuild || g_stopping.load());
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
            const bool lockConsumablesCanBeTried = !g_lockConsumablesInstallFailed;
            const bool hasPlayerOptions =
                g_playerHealthPatchReady ||
                g_infiniteBreathPatchReady ||
                g_stealthModePatchReady ||
                g_noReloadPatchReady ||
                g_noFallDamagePatchReady ||
                g_eagleVisionSprintPatchReady ||
                g_killCiviliansNoDesyncPatchReady ||
                g_showCollectiblesPatchReady ||
                lockConsumablesCanBeTried ||
                g_inventoryPointerPatchReady;
            if (!hasPlayerOptions) {
                ImGui::TextDisabled("Player options unavailable: hooks were not installed.");
            } else {
                ImGui::Columns(2, "PlayerColumns", false);

                if (g_playerHealthPatchReady) {
                    ImGui::Checkbox("God Mode", &g_playerGodmode);
                } else {
                    ImGui::TextDisabled("God Mode unavailable");
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
                if (g_noFallDamagePatchReady) {
                    ImGui::Checkbox("No Fall Damage", &g_noFallDamage);
                } else {
                    ImGui::TextDisabled("No Fall Damage unavailable");
                }
                if (g_eagleVisionSprintPatchReady) {
                    bool value = g_eagleVisionSprint;
                    if (ImGui::Checkbox("Allow Eagle Vision while sprinting", &value)) {
                        g_eagleVisionSprint = value;
                        ApplyEagleVisionSprintPatch();
                        Log(g_eagleVisionSprint ? "Allow Eagle Vision while sprinting enabled." :
                                                  "Allow Eagle Vision while sprinting disabled.");
                    }
                } else {
                    ImGui::TextDisabled("Allow Eagle Vision while sprinting unavailable");
                }
                if (g_killCiviliansNoDesyncPatchReady) {
                    bool value = g_killCiviliansNoDesync;
                    if (ImGui::Checkbox("Kill civilians without desynchronization", &value)) {
                        g_killCiviliansNoDesync = value;
                        ApplyKillCiviliansNoDesyncPatch();
                        Log(g_killCiviliansNoDesync ? "Kill civilians without desynchronization enabled." :
                                                       "Kill civilians without desynchronization disabled.");
                    }
                } else {
                    ImGui::TextDisabled("Kill civilians without desynchronization unavailable");
                }
                if (g_showCollectiblesPatchReady) {
                    ImGui::Checkbox("Show collectibles", &g_showCollectibles);
                } else {
                    ImGui::TextDisabled("Show collectibles unavailable");
                }

                ImGui::NextColumn();
                bool lockConsumables = g_lockConsumables;
                if (ImGui::Checkbox("Lock Consumables (incl Ship Ammo)", &lockConsumables)) {
                    g_lockConsumables = lockConsumables;
                    AfterLockConsumablesToggle();
                    Log(g_lockConsumables ? "Lock Consumables enabled." :
                                            "Lock Consumables disabled.");
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "Prevents the shared consumable decrement from lowering values while enabled.\n"
                        "Installs its hook only when enabled. Best used after your save is fully loaded.");
                }
                if (g_lockConsumablesInstallFailed) {
                    ImGui::TextDisabled("Lock Consumables unavailable: hook install failed.");
                } else if (!g_lockConsumablesPatchReady) {
                    ImGui::TextDisabled("Lock Consumables hook will install when enabled.");
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::TextUnformatted("One-time Inventory Refill");
                ImGui::TextWrapped("Choose one resource/ammo type and refill it once to the preset or custom amount.");
                ImGui::Checkbox("Turn Refills into a Setter", &g_inventoryRefillsAreSetters);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(
                        "When enabled, preset and custom refills write the exact amount.\n"
                        "When disabled, they only raise values that are below the target.");
                }
                if (g_inventoryRefillSelected < 0 || g_inventoryRefillSelected >= kInventoryRefillEntryCount) {
                    g_inventoryRefillSelected = 0;
                }
                const InventoryRefillEntry& selectedRefill = g_inventoryRefillEntries[g_inventoryRefillSelected];
                ImGui::SetNextItemWidth(260.0f);
                if (ImGui::BeginCombo("Resource", selectedRefill.name)) {
                    for (int i = 0; i < kInventoryRefillEntryCount; ++i) {
                        const bool selected = i == g_inventoryRefillSelected;
                        if (ImGui::Selectable(g_inventoryRefillEntries[i].name, selected)) {
                            g_inventoryRefillSelected = i;
                            g_inventoryRefillLastResult = 0;
                            g_inventoryCustomRefillAmount = g_inventoryRefillEntries[i].value;
                        }
                        if (selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                char refillButton[64]{};
                sprintf_s(refillButton, "Refill to %d", selectedRefill.value);
                if (!g_inventoryPointerPatchReady || g_inventoryBase == 0) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Button(refillButton)) {
                    RefillSelectedInventoryEntry();
                }
                if (ImGui::Button("Refill to custom amount")) {
                    g_inventoryCustomRefillSelected = g_inventoryRefillSelected;
                    g_inventoryCustomRefillAmount = selectedRefill.value;
                    const ImVec2 windowPos = ImGui::GetWindowPos();
                    const ImVec2 windowSize = ImGui::GetWindowSize();
                    ImGui::SetNextWindowPos(ImVec2(windowPos.x + windowSize.x * 0.5f,
                                                   windowPos.y + windowSize.y * 0.5f),
                                            ImGuiCond_Appearing,
                                            ImVec2(0.5f, 0.5f));
                    ImGui::OpenPopup("Custom Refill Amount");
                }
                if (!g_inventoryPointerPatchReady || g_inventoryBase == 0) {
                    ImGui::EndDisabled();
                    ImGui::TextDisabled("Inventory pointer not ready yet; load a save and open inventory/ship menus first.");
                }
                if (ImGui::BeginPopupModal("Custom Refill Amount", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                    if (g_inventoryCustomRefillSelected < 0 ||
                        g_inventoryCustomRefillSelected >= kInventoryRefillEntryCount) {
                        g_inventoryCustomRefillSelected = g_inventoryRefillSelected;
                    }
                    const InventoryRefillEntry& customRefill =
                        g_inventoryRefillEntries[g_inventoryCustomRefillSelected];

                    ImGui::Text("Refill %s to:", customRefill.name);
                    ImGui::SetNextItemWidth(180.0f);
                    if (ImGui::InputInt("##CustomRefillAmount", &g_inventoryCustomRefillAmount)) {
                        if (g_inventoryCustomRefillAmount < 0) {
                            g_inventoryCustomRefillAmount = 0;
                        }
                    }
                    ImGui::Spacing();
                    if (!g_inventoryPointerPatchReady || g_inventoryBase == 0) {
                        ImGui::BeginDisabled();
                    }
                    if (ImGui::Button("Refill now")) {
                        RefillInventoryEntryToAmount(g_inventoryCustomRefillSelected,
                                                     g_inventoryCustomRefillAmount);
                        ImGui::CloseCurrentPopup();
                    }
                    if (!g_inventoryPointerPatchReady || g_inventoryBase == 0) {
                        ImGui::EndDisabled();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                if (g_inventoryRefillLastResult > 0) {
                    ImGui::TextDisabled("Last refill succeeded.");
                } else if (g_inventoryRefillLastResult < 0) {
                    ImGui::TextDisabled("Last refill failed.");
                }

                ImGui::Columns(1);

                ImGui::Spacing();
                ImGui::Spacing();
                if (g_playerHealthPatchReady) {
                    if (ImGui::Button("Desynchronize yourself")) {
                        DesynchronizeYourself();
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("One-shot action to trigger player desynchronization.");
                    }
                    if (g_playerHealthComponent == 0) {
                        ImGui::TextDisabled("Waiting for player health state");
                    }
                } else {
                    ImGui::TextDisabled("Desynchronize yourself unavailable");
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
                         InterlockedCompareExchange(&g_timeIntervalHits, 0, 0));
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
                     InterlockedCompareExchange(&g_timeIntervalHits, 0, 0));
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

            const int hoodState = GetPlayerHoodState();
            if (hoodState < 0) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Toggle Hood")) {
                if (ToggleHood()) {
                    const int updatedHoodState = GetPlayerHoodState();
                    if (updatedHoodState == 0) {
                        Log("Toggle Hood set hood UP from ImGui.");
                    } else if (updatedHoodState == 1) {
                        Log("Toggle Hood set hood DOWN from ImGui.");
                    } else {
                        Log("Toggle Hood triggered from ImGui.");
                    }
                } else {
                    LogWarn("Toggle Hood failed from ImGui: player entity not ready.");
                }
            }
            if (hoodState < 0) {
                ImGui::EndDisabled();
                ImGui::TextDisabled("Toggle Hood unavailable: player entity not ready.");
            } else {
                ImGui::SameLine();
                ImGui::TextDisabled("Current hood: %s", hoodState == 0 ? "up" : "down");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Raises or lowers Edward's hood based on the current state.");
            }

            const int crouchState = GetPlayerCrouchState();
            if (crouchState < 0) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Toggle Crouch")) {
                if (ToggleCrouch()) {
                    const int updatedCrouchState = GetPlayerCrouchState();
                    if (updatedCrouchState == 1) {
                        Log("Toggle Crouch set crouch ON from ImGui.");
                    } else if (updatedCrouchState == 0) {
                        Log("Toggle Crouch set crouch OFF from ImGui.");
                    } else {
                        Log("Toggle Crouch triggered from ImGui.");
                    }
                } else {
                    LogWarn("Toggle Crouch failed from ImGui: player entity not ready.");
                }
            }
            if (crouchState < 0) {
                ImGui::EndDisabled();
                ImGui::TextDisabled("Toggle Crouch unavailable: player entity not ready.");
            } else {
                ImGui::SameLine();
                ImGui::TextDisabled("Current crouch: %s", crouchState == 1 ? "on" : "off");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Toggles Edward's crouch state by switching the live crouch flags.");
            }
            ImGui::Separator();
            bool freeCam = g_freeCamEnabled;
            if (ImGui::Checkbox("Free Cam", &freeCam)) {
                g_freeCamEnabled = freeCam;
                AfterFreeCamToggle();
                SaveConfig();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Camera-only free roam. Hooks install only when this is enabled.\n"
                    "Mouse aims the camera. Movement, boost, and exit keys are configurable\n"
                    "in the Free Cam Control Hotkeys block under the Hotkeys tab.");
            }

            float freeCamSpeed = g_freeCamSpeed;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Free Cam Speed", &freeCamSpeed, 0.01f, 0.001f, 100.0f, "%.3f")) {
                if (freeCamSpeed < 0.001f) {
                    freeCamSpeed = 0.001f;
                }
                if (freeCamSpeed > 100.0f) {
                    freeCamSpeed = 100.0f;
                }
                g_freeCamSpeed = freeCamSpeed;
                SaveConfig();
            }

            float freeCamVerticalSpeed = g_freeCamVerticalSpeed;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Free Cam Vertical Speed", &freeCamVerticalSpeed, 0.01f, 0.001f, 100.0f, "%.3f")) {
                if (freeCamVerticalSpeed < 0.001f) {
                    freeCamVerticalSpeed = 0.001f;
                }
                if (freeCamVerticalSpeed > 100.0f) {
                    freeCamVerticalSpeed = 100.0f;
                }
                g_freeCamVerticalSpeed = freeCamVerticalSpeed;
                SaveConfig();
            }

            float freeCamLookSpeed = g_freeCamLookSpeed;
            ImGui::SetNextItemWidth(280.0f);
            if (ImGui::DragFloat("Free Cam Look Speed", &freeCamLookSpeed, 0.01f, 0.05f, 20.0f, "%.3f")) {
                if (freeCamLookSpeed < 0.05f) {
                    freeCamLookSpeed = 0.05f;
                }
                if (freeCamLookSpeed > 20.0f) {
                    freeCamLookSpeed = 20.0f;
                }
                g_freeCamLookSpeed = freeCamLookSpeed;
                SaveConfig();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Scales mouse look while Free Cam is enabled.");
            }

            ImGui::Text("Free Cam state: %s",
                        g_freeCamActive ? "active" :
                        (g_freeCamEnabled ? "waiting for camera" :
                        (g_freeCamHooksInstalled ? "installed" :
                        (g_freeCamInstallFailed ? "install failed" : "not installed"))));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Unlocks")) {
            ImGui::TextDisabled("Back up your save first: unlocks can become permanent/irreversible once the game saves.");
            ImGui::TextDisabled("Unlocks can softlock saves if something was meant to unlock later through progression.");
            ImGui::Separator();

            ImGui::TextUnformatted("Targeted unlock checkboxes");
            ImGui::TextWrapped(
                "Use the checkboxes below when you only want specific listed unlocks. "
                "Best used from the ship cabin, where weapon, outfit, and ship records are usually loaded.");
            ImGui::TextDisabled("Checked entries are scanned/applied in the background and auto-uncheck after success.");
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
                            g_unlockLastScan = 0;
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
            ImGui::Text("Done this session: %d", g_unlockRecordsPatched);
            if (g_unlockScanRunning != 0) {
                ImGui::TextDisabled("Scan running in background...");
            }
            if (enabledCount > 0) {
                ImGui::TextDisabled("If an item stays waiting, go to the ship cabin and keep it checked.");
            }

            ImGui::Separator();
            ImGui::TextUnformatted("Global hidden unlocks");
            ImGui::TextWrapped(
                "Alternative to the targeted checkbox list above: installs a broad hidden-unlock patch for this session. "
                "It unlocks all at once and can expose additional hidden rewards that are not available as individual checkboxes.");
            ImGui::TextDisabled("Use at the main menu before loading a save. Recommended only with a save backup.");
            if (g_globalHiddenUnlockInstalled) {
                ImGui::TextDisabled("Global Hidden Unlocks installed for this session.");
            } else if (ImGui::Button("Install Global Hidden Unlocks")) {
                InstallGlobalHiddenUnlockPatch();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Freedom Cry")) {
            ImGui::TextWrapped(
                "Adds to your Freedom Cry resistance progress. "
                "Use this while loaded into Freedom Cry.");

            const bool freedomCryReady = g_inventoryPointerPatchReady && g_inventoryBase != 0;
            if (!freedomCryReady) {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Add 100 Liberated Slaves")) {
                g_freedomCryLastResult =
                    AddInventoryValueByOffset(0xB4, 100, "Liberated Slaves") ? 1 : -1;
            }
            if (ImGui::Button("Add 100 Recruited Maroons")) {
                g_freedomCryLastResult =
                    AddInventoryValueByOffset(0xB8, 100, "Recruited Maroons") ? 1 : -1;
            }

            if (!freedomCryReady) {
                ImGui::EndDisabled();
                ImGui::TextDisabled("Not ready yet; load Freedom Cry and open an inventory or ship menu first.");
            }

            if (g_freedomCryLastResult > 0) {
                ImGui::TextDisabled("Last Freedom Cry add succeeded.");
            } else if (g_freedomCryLastResult < 0) {
                ImGui::TextDisabled("Last Freedom Cry add failed.");
            }
            ImGui::EndTabItem();
        }
        ImGui::EndDisabled();
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
            if (ImGui::Checkbox("Disable Controller Input while UI is open", &g_disableControllerInputWhenUiOpen)) {
                SaveConfig();
                RefreshInputCaptureState();
                Log(g_disableControllerInputWhenUiOpen ? "Disable Controller Input while UI is open enabled." :
                                                          "Disable Controller Input while UI is open disabled.");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(
                    "Blocks controller input from reaching the game while the AC4Tools UI is open.\n"
                    "AC4Tools can still read controller buttons for menu close and Set Pad capture.");
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
                DrawInfoRow("Game SHA256", g_gameExeSha256Text);
                DrawInfoRow("Gameplay build gate", g_supportedBuild ? "supported" : "disabled: unsupported build or image base");
                DrawInfoRow("Supported exe", kSupportedGameExe);
                DrawInfoRow("Supported size", kSupportedGameSize);
                DrawInfoRow("Supported timestamp", kSupportedGameTimestamp);
                DrawInfoRow("Supported SHA256", kSupportedGameSha256);
                DrawInfoRow("Console logging", g_consoleLoggingEnabled ? "enabled" : "disabled");
                DrawInfoRow("File logging", g_fileLoggingEnabled ? "enabled" : "disabled");
                DrawInfoRow("Mouse window lock", g_lockMouseToWindow ? "enabled" : "disabled");
                DrawInfoRow("Disable mouse input on UI open", g_disableMouseInputWhenUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Disable keyboard input on UI open", g_disableKeyboardInputWhenUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Disable controller input on UI open", g_disableControllerInputWhenUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Disable hotkeys on UI open", g_disableHotkeysWhileUiOpen ? "enabled" : "disabled");
                DrawInfoRow("Input captured", g_inputCaptured ? "yes" : "no");
                DrawInfoRowf("Inventory base", "0x%08X", static_cast<unsigned int>(g_inventoryBase));
                DrawInfoRowf("Inventory refill writes", "%d", g_inventoryPointerLastWrites);
                DrawInfoRowf("Unlock entries", "%d total", kUnlockEntryCount);
                DrawInfoRowf("Unlock queue", "%d pending", CountEnabledUnlocks());
                DrawInfoRowf("Unlocks done this session", "%d", g_unlockRecordsPatched);
                DrawInfoRowf("Unlock records found", "%d last pass", g_unlockRecordsFound);
                DrawInfoRow("Unlock scan", g_unlockScanRunning != 0 ? "running" : "idle");
                DrawInfoRow("Community completion", g_finishCommunityChallengesForUnlocks ? "enabled" : "disabled");
                DrawInfoRow("Global hidden unlocks", g_globalHiddenUnlockInstalled ? "installed" : "not installed");
                DrawInfoRowf("TimeScale interval", "%d", g_timeScaleInterval);
                DrawInfoRow("Free Cam", g_freeCamActive ? "active" : (g_freeCamEnabled ? "waiting" : (g_freeCamHooksInstalled ? "installed" : "not installed")));
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
                DrawPatchRow("Time Scale", g_timeScalePatchReady, "0x00A047EA", InterlockedCompareExchange(&g_timeIntervalHits, 0, 0));
                DrawPatchRow("Player Godmode", g_playerHealthPatchReady, "0x0115AD07", InterlockedCompareExchange(&g_playerHealthHits, 0, 0));
                DrawPatchRow("Infinite Breath", g_infiniteBreathPatchReady, "0x01148936", InterlockedCompareExchange(&g_infiniteBreathHits, 0, 0));
                DrawPatchRow("Stealth Mode", g_stealthModePatchReady, "0x0101CB66");
                DrawPatchRow("No Reload", g_noReloadPatchReady, "0x016B8A96", InterlockedCompareExchange(&g_noReloadHits, 0, 0));
                DrawPatchRow("No Fall Damage", g_noFallDamagePatchReady, "AOB", InterlockedCompareExchange(&g_noFallDamageHits, 0, 0));
                DrawPatchRow("Eagle Vision Sprint", g_eagleVisionSprintPatchReady, "AOB");
                DrawPatchRow("Civilian No Desync", g_killCiviliansNoDesyncPatchReady, "AOB");
                DrawPatchRow("Show Collectibles", g_showCollectiblesPatchReady, "0x014C1D84", InterlockedCompareExchange(&g_showCollectiblesHits, 0, 0));
                DrawPatchRow("Lock Consumables", g_lockConsumablesPatchReady, "0x011A1F6D", InterlockedCompareExchange(&g_lockConsumablesHits, 0, 0));
                DrawPatchRow("Inventory Pointer", g_inventoryPointerPatchReady, "0x01CFD381", InterlockedCompareExchange(&g_inventoryPointerHits, 0, 0));
                DrawPatchRow("Mission Timer", g_missionTimerPatchReady, "0x019446C3", InterlockedCompareExchange(&g_missionTimerHits, 0, 0));
                DrawPatchRow("Mission Timer II", g_missionTimer2PatchReady, "0x016869B7", InterlockedCompareExchange(&g_missionTimer2Hits, 0, 0));
                DrawPatchRow("Noclip Update", g_noclipPatchReady, "0x016FAACD", InterlockedCompareExchange(&g_noclipUpdateHits, 0, 0));
                DrawPatchRow("Free Cam Transform", g_freeCamHooksInstalled, "lazy AOB", InterlockedCompareExchange(&g_freeCamTransformHits, 0, 0));
                DrawPatchRow("Free Cam Camera", g_freeCamHooksInstalled, "lazy AOB", InterlockedCompareExchange(&g_freeCamCameraHits, 0, 0));
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

HRESULT __stdcall HookPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
    StateGuard stateGuard(g_stateLock);
    if (g_stopping.load()) {
        stateGuard.unlock();
        return g_originalPresent(swapChain, syncInterval, flags);
    }
    if (!g_imguiReady) {
        InitImGui(swapChain);
    }

    BeginControllerPollingFrame();
    UpdateGamepadStates();
    PollControllerDevices();

    static bool menuWasDown = false;
    const bool menuIsDown =
        ((g_menuHotkey != 0 &&
          g_suppressedHotkeyVk != g_menuHotkey &&
          (QueryPhysicalKeyState(g_menuHotkey) & 0x8000) != 0) ||
         (g_menuGamepadHotkey != 0 &&
          g_suppressedHotkeyGamepadButton != g_menuGamepadHotkey &&
          IsGamepadButtonDown(g_menuGamepadHotkey)));
    if (menuIsDown && !menuWasDown) {
        g_menuOpen = !g_menuOpen;
        if (g_menuHotkey != 0 && IsPhysicalKeyDown(g_menuHotkey)) {
            Logf("Menu %s from hotkey %s.", g_menuOpen ? "opened" : "closed", HotkeyName(g_menuHotkey));
        } else {
            Logf("Menu %s from gamepad hotkey %s.", g_menuOpen ? "opened" : "closed", GamepadButtonName(g_menuGamepadHotkey));
        }
        RefreshInputCaptureState();
        if (!g_menuOpen) {
            ReleaseMenuInputState();
        }
    }
    menuWasDown = menuIsDown;
    if (g_suppressedHotkeyVk != 0 && (QueryPhysicalKeyState(g_suppressedHotkeyVk) & 0x8000) == 0) {
        g_suppressedHotkeyVk = 0;
    }
    if (g_suppressedHotkeyGamepadButton != 0 && !IsGamepadButtonDown(g_suppressedHotkeyGamepadButton)) {
        g_suppressedHotkeyGamepadButton = 0;
    }

    if (g_imguiReady) {
        if (g_menuOpen && GetForegroundWindow() != g_gameWindow) {
            g_menuOpen = false;
            ReleaseMenuInputState();
        }
        UpdateMouseWindowLock();
        ProcessGamepadCapture();
        ProcessHotkeys();
        if (g_freeCamEnabled) {
            UpdateFreeCam();
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = g_menuOpen;
        io.MouseDown[0] = g_menuOpen && ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
        io.MouseDown[1] = g_menuOpen && ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
        io.MouseDown[2] = g_menuOpen && ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
        if (g_menuOpen) {
            io.MouseWheel += g_pendingMouseWheel;
            io.MouseWheelH += g_pendingMouseWheelH;
        }
        g_pendingMouseWheel = 0.0f;
        g_pendingMouseWheelH = 0.0f;
        ImGui::NewFrame();

        if (g_menuOpen) {
            DrawMenu();
        }

        ImGui::Render();
        if (!g_renderTarget) {
            CreateRenderTarget(swapChain);
        }
        if (g_renderTarget) {
            g_context->OMSetRenderTargets(1, &g_renderTarget, nullptr);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    }

    PublishCaveState();
    stateGuard.unlock();
    return g_originalPresent(swapChain, syncInterval, flags);
}

HRESULT __stdcall HookResizeBuffers(IDXGISwapChain* swapChain,
                                    UINT bufferCount,
                                    UINT width,
                                    UINT height,
                                    DXGI_FORMAT newFormat,
                                    UINT swapChainFlags) {
    StateGuard stateGuard(g_stateLock);
    if (g_imguiReady) {
        ImGui_ImplDX11_InvalidateDeviceObjects();
        ReleaseRenderTarget();
        Logf("DX11 swap chain resize detected: %ux%u.", width, height);
    }

    const HRESULT hr = g_originalResizeBuffers(
        swapChain,
        bufferCount,
        width,
        height,
        newFormat,
        swapChainFlags);

    if (SUCCEEDED(hr) && g_imguiReady && !g_stopping.load()) {
        CreateRenderTarget(swapChain);
        ImGui_ImplDX11_CreateDeviceObjects();
    } else if (FAILED(hr)) {
        LogWarnf("DX11 ResizeBuffers failed: HRESULT=0x%08X.", static_cast<unsigned int>(hr));
    }

    return hr;
}
