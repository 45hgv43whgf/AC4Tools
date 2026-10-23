// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void UpdateTimeScaleInterval() {
    if (!std::isfinite(g_timeScale)) g_timeScale = 0.01f;
    if (!g_timeScaleEnabled) {
        g_timeScaleInterval = 10000;
    } else {
        const float clamped = g_timeScale < 0.0001f ? 0.0001f : g_timeScale;
        int interval = static_cast<int>((1000.0f / clamped) + 0.5f);
        if (interval < 1) {
            interval = 1;
        }
        g_timeScaleInterval = interval;
    }
}

bool TryReadPointer(std::uintptr_t address, std::uintptr_t& value) {
    if (!address) {
        return false;
    }
    __try {
            value = *reinterpret_cast<std::uintptr_t*>(address);
            return value != 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        value = 0;
        return false;
    }
}

bool TryReadFloat(std::uintptr_t address, float& value) {
    if (!address) {
        return false;
    }
    __try {
            value = *reinterpret_cast<float*>(address);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        value = 0.0f;
        return false;
    }
}

std::uintptr_t ResolveFreeCamTransform() {
    std::uintptr_t value = 0;
    if (!TryReadPointer(g_freeCamRootStaticAddress, value)) {
        return 0;
    }
    if (!TryReadPointer(value + 0x08, value)) {
        return 0;
    }
    if (!TryReadPointer(value + 0x34, value)) {
        return 0;
    }
    if (!TryReadPointer(value + 0x54, value)) {
        return 0;
    }
    if (!TryReadPointer(value, value)) {
        return 0;
    }
    return value;
}

void CaptureFreeCamCoordinates(std::uintptr_t transform) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (TryReadFloat(transform + 0x10, x) &&
        TryReadFloat(transform + 0x14, y) &&
        TryReadFloat(transform + 0x18, z)) {
        g_freeCamX = x;
        g_freeCamY = y;
        g_freeCamZ = z;
    }
}

bool IsPhysicalKeyDown(int vk) {
    if (vk == 0) {
        return false;
    }
    return (QueryPhysicalKeyState(vk) & 0x8000) != 0;
}

bool IsFreeCamControlDown(int controlIndex) {
    if (controlIndex < 0 || controlIndex >= kFreeCamControlCount) {
        return false;
    }
    return IsPhysicalKeyDown(g_freeCamControls[controlIndex].key) ||
           IsGamepadButtonDown(g_freeCamControls[controlIndex].gamepadButton);
}

LONG ScaleMouseDelta(LONG value) {
    if (!g_freeCamEnabled || g_freeCamLookSpeed == 1.0f || value == 0) {
        return value;
    }

    double scaled = static_cast<double>(value) * static_cast<double>(g_freeCamLookSpeed);
    if (scaled > 2147483647.0) {
        scaled = 2147483647.0;
    } else if (scaled < -2147483648.0) {
        scaled = -2147483648.0;
    }
    return static_cast<LONG>(scaled);
}

void ApplyFreeCamMouseLookScale(LPVOID data, DWORD dataSize) {
    if (!g_freeCamEnabled || !data || dataSize < sizeof(LONG) * 2) {
        return;
    }

    if (dataSize >= sizeof(DIMOUSESTATE2)) {
        auto* state = static_cast<DIMOUSESTATE2*>(data);
        state->lX = ScaleMouseDelta(state->lX);
        state->lY = ScaleMouseDelta(state->lY);
    } else if (dataSize >= sizeof(DIMOUSESTATE)) {
        auto* state = static_cast<DIMOUSESTATE*>(data);
        state->lX = ScaleMouseDelta(state->lX);
        state->lY = ScaleMouseDelta(state->lY);
    }
}

void ApplyFreeCamMouseLookScale(LPDIDEVICEOBJECTDATA objectData, DWORD count) {
    if (!g_freeCamEnabled || !objectData) {
        return;
    }

    for (DWORD i = 0; i < count; ++i) {
        if (objectData[i].dwOfs == DIMOFS_X || objectData[i].dwOfs == DIMOFS_Y) {
            const LONG value = static_cast<LONG>(objectData[i].dwData);
            objectData[i].dwData = static_cast<DWORD>(ScaleMouseDelta(value));
        }
    }
}

void UpdateFreeCam() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (!g_freeCamEnabled || !g_freeCamHooksInstalled || GetForegroundWindow() != g_gameWindow) {
        g_freeCamActive = false;
        return;
    }

    const std::uintptr_t transform = ResolveFreeCamTransform();
    if (!transform || !g_freeCamCamera) {
        g_freeCamTransform = transform;
        g_freeCamActive = false;
        return;
    }

    const bool wasActive = g_freeCamActive;
    g_freeCamTransform = transform;
    if (!wasActive) {
        CaptureFreeCamCoordinates(transform);
        Logf("Free Cam active at transform 0x%08X camera 0x%08X.",
             static_cast<unsigned int>(g_freeCamTransform),
             static_cast<unsigned int>(g_freeCamCamera));
    }
    g_freeCamActive = true;

    float yawA = 0.0f;
    float yawB = 0.0f;
    float pitchBasis = 0.0f;
    if (!TryReadFloat(g_freeCamCamera + 0x14, yawA) ||
        !TryReadFloat(g_freeCamCamera + 0x10, yawB) ||
        !TryReadFloat(g_freeCamCamera + 0x18, pitchBasis)) {
        g_freeCamActive = false;
        return;
    }

    if (pitchBasis < -1.0f) {
        pitchBasis = -1.0f;
    }
    if (pitchBasis > 1.0f) {
        pitchBasis = 1.0f;
    }

    constexpr float pi = 3.14159265358979323846f;
    const float yaw = atan2f(-yawA, yawB);
    const float pitch = asinf(pitchBasis);
    const float xyStep = IsFreeCamControlDown(kFreeCamBoost) ? g_freeCamSpeed * 5.0f : g_freeCamSpeed;
    const float zStep = IsFreeCamControlDown(kFreeCamBoost) ? g_freeCamVerticalSpeed * 5.0f : g_freeCamVerticalSpeed;
    const float forwardX = cosf(yaw) * xyStep;
    const float forwardY = sinf(yaw) * xyStep;
    const float forwardZ = sinf(pitch) * zStep;
    bool moved = false;

    if (IsFreeCamControlDown(kFreeCamMoveForward)) {
        g_freeCamX += forwardX;
        g_freeCamY -= forwardY;
        g_freeCamZ += forwardZ;
        moved = true;
    }
    if (IsFreeCamControlDown(kFreeCamMoveBackward)) {
        g_freeCamX -= forwardX;
        g_freeCamY += forwardY;
        g_freeCamZ -= forwardZ;
        moved = true;
    }
    if (IsFreeCamControlDown(kFreeCamStrafeLeft)) {
        const float strafeYaw = yaw - (pi * 0.5f);
        g_freeCamX += cosf(strafeYaw) * xyStep;
        g_freeCamY -= sinf(strafeYaw) * xyStep;
        moved = true;
    }
    if (IsFreeCamControlDown(kFreeCamStrafeRight)) {
        const float strafeYaw = yaw + (pi * 0.5f);
        g_freeCamX += cosf(strafeYaw) * xyStep;
        g_freeCamY -= sinf(strafeYaw) * xyStep;
        moved = true;
    }
    if (IsFreeCamControlDown(kFreeCamMoveUp)) {
        g_freeCamZ += zStep;
        moved = true;
    }
    if (IsFreeCamControlDown(kFreeCamMoveDown)) {
        g_freeCamZ -= zStep;
        moved = true;
    }

    if (IsFreeCamControlDown(kFreeCamExit)) {
        g_freeCamEnabled = false;
        g_freeCamActive = false;
        Logf("Free Cam ended from %s.", HotkeyName(g_freeCamControls[kFreeCamExit].key));
        return;
    }

    if (!moved) {
        CaptureFreeCamCoordinates(transform);
    }
}
