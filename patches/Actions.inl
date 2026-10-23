// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void ApplyNoCannonCooldownPatch() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (!g_noCannonCooldownPatchReady) {
        g_noCannonCooldown = false;
        return;
    }

    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoCannonCooldownAddress);
    const std::uint8_t desired = g_noCannonCooldown ? kEnabledNoCannonCooldownByte : kOriginalNoCannonCooldownByte;
    if (*patchAddress == desired) {
        return;
    }
    if (*patchAddress != kOriginalNoCannonCooldownByte &&
        *patchAddress != kEnabledNoCannonCooldownByte) {
        g_noCannonCooldown = false;
        g_noCannonCooldownPatchReady = false;
        LogWarnf("No Cannon Cooldown disabled: byte at 0x%08X changed unexpectedly to 0x%02X.",
             kPatchNoCannonCooldownAddress,
             *patchAddress);
        return;
    }
    if (!WriteMemory(patchAddress, &desired, sizeof(desired))) {
        g_noCannonCooldown = false;
        g_noCannonCooldownPatchReady = false;
        LogError("No Cannon Cooldown disabled: failed to write patch byte.");
    }
}

void ApplyStealthModePatch() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (!g_stealthModePatchReady) {
        g_stealthMode = false;
        return;
    }

    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchStealthModeAddress);
    const std::uint8_t desired = g_stealthMode ? kEnabledStealthModeByte : kOriginalStealthModeByte;
    if (*patchAddress == desired) {
        return;
    }
    if (*patchAddress != kOriginalStealthModeByte &&
        *patchAddress != kEnabledStealthModeByte) {
        g_stealthMode = false;
        g_stealthModePatchReady = false;
        LogWarnf("Stealth Mode disabled: byte at 0x%08X changed unexpectedly to 0x%02X.",
             kPatchStealthModeAddress,
             *patchAddress);
        return;
    }
    if (!WriteMemory(patchAddress, &desired, sizeof(desired))) {
        g_stealthMode = false;
        g_stealthModePatchReady = false;
        LogError("Stealth Mode disabled: failed to write patch byte.");
    }
}

void ApplyEagleVisionSprintPatch() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (!g_eagleVisionSprintPatchReady || !g_eagleVisionSprintAddress) {
        g_eagleVisionSprint = false;
        return;
    }

    const std::uint8_t desired = g_eagleVisionSprint ? kEnabledEagleVisionSprintByte : kOriginalEagleVisionSprintByte;
    if (*g_eagleVisionSprintAddress == desired) {
        return;
    }
    if (*g_eagleVisionSprintAddress != kOriginalEagleVisionSprintByte &&
        *g_eagleVisionSprintAddress != kEnabledEagleVisionSprintByte) {
        g_eagleVisionSprint = false;
        g_eagleVisionSprintPatchReady = false;
        LogWarnf("Allow Eagle Vision while sprinting disabled: byte at 0x%08X changed unexpectedly to 0x%02X.",
             static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_eagleVisionSprintAddress)),
             *g_eagleVisionSprintAddress);
        return;
    }
    if (!WriteMemory(g_eagleVisionSprintAddress, &desired, sizeof(desired))) {
        g_eagleVisionSprint = false;
        g_eagleVisionSprintPatchReady = false;
        LogError("Allow Eagle Vision while sprinting disabled: failed to write patch byte.");
    }
}

void ApplyKillCiviliansNoDesyncPatch() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (!g_killCiviliansNoDesyncPatchReady ||
        !g_civilianDesyncAddress1 ||
        !g_civilianDesyncAddress2) {
        g_killCiviliansNoDesync = false;
        return;
    }

    const auto* desired = g_killCiviliansNoDesync ? kEnabledCivilianDesyncBytes : kOriginalCivilianDesyncBytes;
    const auto* desiredWarningCall1 = g_killCiviliansNoDesync
        ? kDisabledCivilianWarningCallBytes
        : kOriginalCivilianWarningCall1Bytes;
    const auto* desiredWarningCall2 = g_killCiviliansNoDesync
        ? kDisabledCivilianWarningCallBytes
        : kOriginalCivilianWarningCall2Bytes;
    auto* warningCallAddress1 = g_civilianDesyncAddress1 + kCivilianWarningCallOffset;
    auto* warningCallAddress2 = g_civilianDesyncAddress2 + kCivilianWarningCallOffset;

    if (BytesMatch(g_civilianDesyncAddress1, desired, sizeof(kOriginalCivilianDesyncBytes)) &&
        BytesMatch(g_civilianDesyncAddress2, desired, sizeof(kOriginalCivilianDesyncBytes)) &&
        BytesMatch(warningCallAddress1, desiredWarningCall1, sizeof(kOriginalCivilianWarningCall1Bytes)) &&
        BytesMatch(warningCallAddress2, desiredWarningCall2, sizeof(kOriginalCivilianWarningCall2Bytes))) {
        return;
    }

    const bool site1Known =
        BytesMatch(g_civilianDesyncAddress1, kOriginalCivilianDesyncBytes, sizeof(kOriginalCivilianDesyncBytes)) ||
        BytesMatch(g_civilianDesyncAddress1, kEnabledCivilianDesyncBytes, sizeof(kEnabledCivilianDesyncBytes));
    const bool site2Known =
        BytesMatch(g_civilianDesyncAddress2, kOriginalCivilianDesyncBytes, sizeof(kOriginalCivilianDesyncBytes)) ||
        BytesMatch(g_civilianDesyncAddress2, kEnabledCivilianDesyncBytes, sizeof(kEnabledCivilianDesyncBytes));
    const bool warningCall1Known =
        BytesMatch(warningCallAddress1, kOriginalCivilianWarningCall1Bytes, sizeof(kOriginalCivilianWarningCall1Bytes)) ||
        BytesMatch(warningCallAddress1, kDisabledCivilianWarningCallBytes, sizeof(kDisabledCivilianWarningCallBytes));
    const bool warningCall2Known =
        BytesMatch(warningCallAddress2, kOriginalCivilianWarningCall2Bytes, sizeof(kOriginalCivilianWarningCall2Bytes)) ||
        BytesMatch(warningCallAddress2, kDisabledCivilianWarningCallBytes, sizeof(kDisabledCivilianWarningCallBytes));
    if (!site1Known || !site2Known || !warningCall1Known || !warningCall2Known) {
        g_killCiviliansNoDesync = false;
        g_killCiviliansNoDesyncPatchReady = false;
        LogWarn("Kill civilians without desynchronization disabled: patch bytes changed unexpectedly.");
        return;
    }

    if (!WriteMemory(g_civilianDesyncAddress1, desired, sizeof(kOriginalCivilianDesyncBytes)) ||
        !WriteMemory(g_civilianDesyncAddress2, desired, sizeof(kOriginalCivilianDesyncBytes)) ||
        !WriteMemory(warningCallAddress1, desiredWarningCall1, sizeof(kOriginalCivilianWarningCall1Bytes)) ||
        !WriteMemory(warningCallAddress2, desiredWarningCall2, sizeof(kOriginalCivilianWarningCall2Bytes))) {
        g_killCiviliansNoDesync = false;
        g_killCiviliansNoDesyncPatchReady = false;
        LogError("Kill civilians without desynchronization disabled: failed to write patch bytes.");
    }
}

void DesynchronizeYourself() {
    if (!g_supportedBuild || g_stopping.load() || !g_playerHealthPatchReady) return;
    const auto component = g_playerHealthComponent.load();
    if (!component) return;
    const bool restoreGodmode = g_playerGodmode;
    g_playerGodmode = false;
    PublishCaveState();
    __try {
            using SetPlayerHealthFn = void(__thiscall*)(void*, int);
            reinterpret_cast<SetPlayerHealthFn>(kSetPlayerHealthAddress)(reinterpret_cast<void*>(component), -1);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        LogWarn("Desynchronize yourself failed: player health component is unavailable.");
    }
    g_playerGodmode = restoreGodmode;
    PublishCaveState();
}
