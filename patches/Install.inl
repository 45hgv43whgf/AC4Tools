// Private unity fragment; included by AC4Tools.cpp inside its namespace.
bool InstallHealthPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchHealthAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing ship godmode patch: 0x01225C3F is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalHealthBytes, sizeof(kOriginalHealthBytes))) {
        LogPatchMismatch("ship godmode patch at 0x01225C3F",
                         patchAddress,
                         kOriginalHealthBytes,
                         sizeof(kOriginalHealthBytes));
        return false;
    }

    g_cave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_cave) {
        LogError("VirtualAlloc failed.");
        return false;
    }

    std::uint8_t* p = g_cave;

    // cmp byte ptr [&g_enabled], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_enabledCell.bits);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // cmp edx, 0
    *p++ = 0x83;
    *p++ = 0xFA;
    *p++ = 0x00;

    // jne filtered
    *p++ = 0x75;
    const auto jneOriginalOffset = p++;

    // mov dword ptr [esi+0xD0], 0x47C35000 ; 100000.0f
    *p++ = 0xC7;
    *p++ = 0x86;
    *reinterpret_cast<std::uint32_t*>(p) = 0x000000D0;
    p += 4;
    *reinterpret_cast<std::uint32_t*>(p) = 0x47C35000;
    p += 4;

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchHealthReturnAddress) - p - 4);
    p += 4;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    *jneOriginalOffset = static_cast<std::uint8_t>(originalLabel - jneOriginalOffset - 1);

    // movss [esi+0xD0], xmm0
    memcpy(p, kOriginalHealthBytes, sizeof(kOriginalHealthBytes));
    p += sizeof(kOriginalHealthBytes);

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchHealthReturnAddress) - p - 4);
    p += 4;

    if (!WriteJump(patchAddress, g_cave, sizeof(kOriginalHealthBytes))) {
        LogError("Refusing ship godmode patch: failed to write hook at 0x01225C3F.");
        return false;
    }
    return true;
}

bool InstallNoCannonCooldownPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoCannonCooldownAddress);
    if (SafeFirstByte(patchAddress) == kEnabledNoCannonCooldownByte) {
        LogWarn("Refusing No Cannon Cooldown patch: 0x01C97031 is already patched by another tool.");
        return false;
    }
    if (SafeFirstByte(patchAddress) != kOriginalNoCannonCooldownByte) {
        const std::uint8_t expected[] = {kOriginalNoCannonCooldownByte};
        LogPatchMismatch("No Cannon Cooldown patch at 0x01C97031",
                         patchAddress,
                         expected,
                         sizeof(expected));
        return false;
    }
    Log("No Cannon Cooldown byte patch ready at 0x01C97031.");
    if (!ExpectPatchBytes(patchAddress, &kOriginalNoCannonCooldownByte, 1)) return false;
    return true;
}

bool InstallStealthModePatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchStealthModeAddress);
    if (SafeFirstByte(patchAddress) == kEnabledStealthModeByte) {
        LogWarn("Refusing Stealth Mode patch: 0x0101CB66 is already patched by another tool.");
        return false;
    }
    if (SafeFirstByte(patchAddress) != kOriginalStealthModeByte) {
        const std::uint8_t expected[] = {kOriginalStealthModeByte};
        LogPatchMismatch("Stealth Mode patch at 0x0101CB66",
                         patchAddress,
                         expected,
                         sizeof(expected));
        return false;
    }
    Log("Stealth Mode byte patch ready at 0x0101CB66.");
    if (!ExpectPatchBytes(patchAddress, &kOriginalStealthModeByte, 1)) return false;
    return true;
}

bool InstallAllyGodmodePatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchAllyGodmodeAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Ally Godmode patch: 0x01BFF520 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalAllyGodmodeBytes, sizeof(kOriginalAllyGodmodeBytes))) {
        LogPatchMismatch("Ally Godmode patch at 0x01BFF520",
                         patchAddress,
                         kOriginalAllyGodmodeBytes,
                         sizeof(kOriginalAllyGodmodeBytes));
        return false;
    }

    g_allyGodmodeCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_allyGodmodeCave) {
        LogError("VirtualAlloc failed for Ally Godmode patch.");
        return false;
    }

    std::uint8_t* p = g_allyGodmodeCave;

    // cmp byte ptr [&g_allyGodmode], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_allyGodmodeCell.bits);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // cmp dword ptr [ecx+0xF4], 1
    *p++ = 0x83;
    *p++ = 0xB9;
    *reinterpret_cast<std::uint32_t*>(p) = 0x000000F4;
    p += 4;
    *p++ = 0x01;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOwnerOffset = p++;

    // mov ax, word ptr [ecx+0x5E]
    *p++ = 0x66;
    *p++ = 0x8B;
    *p++ = 0x41;
    *p++ = 0x5E;

    // mov word ptr [ecx+0x5C], ax
    *p++ = 0x66;
    *p++ = 0x89;
    *p++ = 0x41;
    *p++ = 0x5C;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    *jeOriginalOwnerOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOwnerOffset - 1);

    memcpy(p, kOriginalAllyGodmodeBytes, sizeof(kOriginalAllyGodmodeBytes));
    p += sizeof(kOriginalAllyGodmodeBytes);

    if (!WriteJump(patchAddress, g_allyGodmodeCave, sizeof(kOriginalAllyGodmodeBytes))) {
        LogError("Refusing Ally Godmode patch: failed to write hook at 0x01BFF520.");
        return false;
    }
    Log("Installed Ally Godmode hook at 0x01BFF520.");
    return true;
}

bool InstallTimeIntervalPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchTimeIntervalAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing TimeScale interval patch: 0x00A047EA is already hooked by another ASI.");
        return false;
    }

    if (!ExpectPatchBytes(patchAddress, kOriginalTimeIntervalBytes, sizeof(kOriginalTimeIntervalBytes))) {
        LogPatchMismatch("TimeScale interval patch at 0x00A047EA",
                         patchAddress,
                         kOriginalTimeIntervalBytes,
                         sizeof(kOriginalTimeIntervalBytes));
        return false;
    }

    g_timeIntervalCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_timeIntervalCave) {
        LogError("VirtualAlloc failed for TimeScale interval patch.");
        return false;
    }

    std::uint8_t* p = g_timeIntervalCave;

    // inc dword ptr [&g_timeIntervalHits]
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_timeIntervalHits);
    p += 4;

    // mov edx, dword ptr [&g_timeScaleInterval]
    *p++ = 0x8B;
    *p++ = 0x15;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_timeScaleIntervalCell.bits);
    p += 4;

    // jmp return
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(kPatchTimeIntervalReturnAddress) - p - 4);
    p += 4;

    if (!WriteJump(patchAddress, g_timeIntervalCave, sizeof(kOriginalTimeIntervalBytes))) {
        LogError("Refusing TimeScale interval patch: failed to write hook at 0x00A047EA.");
        return false;
    }
    Log("Installed standalone TimeScale interval hook at 0x00A047EA.");
    return true;
}

bool InstallPlayerHealthPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchPlayerHealthAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing player godmode patch: 0x0115AD07 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalPlayerHealthBytes, sizeof(kOriginalPlayerHealthBytes))) {
        LogPatchMismatch("player godmode patch at 0x0115AD07",
                         patchAddress,
                         kOriginalPlayerHealthBytes,
                         sizeof(kOriginalPlayerHealthBytes));
        return false;
    }

    g_playerHealthCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_playerHealthCave) {
        LogError("VirtualAlloc failed for player godmode patch.");
        return false;
    }

    std::uint8_t* p = g_playerHealthCave;

    // inc dword ptr [&g_playerHealthHits]
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerHealthHits);
    p += 4;

    // mov [&g_playerHealthComponent], esi
    *p++ = 0x89;
    *p++ = 0x35;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerHealthComponent);
    p += 4;

    // cmp byte ptr [&g_playerGodmode], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerGodmodeCell.bits);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    auto* jeOriginalOffset = p++;

    // mov ax, word ptr [esi+0x5E]
    *p++ = 0x66;
    *p++ = 0x8B;
    *p++ = 0x46;
    *p++ = 0x5E;

    // mov word ptr [esi+0x5C], ax
    *p++ = 0x66;
    *p++ = 0x89;
    *p++ = 0x46;
    *p++ = 0x5C;

    // test edi, edi
    *p++ = 0x85;
    *p++ = 0xFF;
    EmitJump(p, kPatchPlayerHealthReturnAddress);

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);
    memcpy(p, kOriginalPlayerHealthBytes, sizeof(kOriginalPlayerHealthBytes));
    p += sizeof(kOriginalPlayerHealthBytes);
    EmitJump(p, kPatchPlayerHealthReturnAddress);

    if (!WriteJump(patchAddress, g_playerHealthCave, sizeof(kOriginalPlayerHealthBytes))) {
        LogError("Refusing player godmode patch: failed to write hook at 0x0115AD07.");
        return false;
    }
    Log("Installed player godmode hook at 0x0115AD07.");
    return true;
}

bool InstallInfiniteBreathPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInfiniteBreathAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Infinite Breath patch: 0x01148936 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalInfiniteBreathBytes, sizeof(kOriginalInfiniteBreathBytes))) {
        LogPatchMismatch("Infinite Breath patch at 0x01148936",
                         patchAddress,
                         kOriginalInfiniteBreathBytes,
                         sizeof(kOriginalInfiniteBreathBytes));
        return false;
    }

    g_infiniteBreathCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_infiniteBreathCave) {
        LogError("VirtualAlloc failed for Infinite Breath patch.");
        return false;
    }

    std::uint8_t* p = g_infiniteBreathCave;

    // inc dword ptr [&g_infiniteBreathHits]
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_infiniteBreathHits);
    p += 4;

    // cmp byte ptr [&g_infiniteBreath], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_infiniteBreathCell.bits);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    const auto jeOriginalOffset = p++;

    // mov dword ptr [eax+0x80], 0x3F800000 ; 1.0f
    *p++ = 0xC7;
    *p++ = 0x80;
    *reinterpret_cast<std::uint32_t*>(p) = 0x00000080;
    p += 4;
    *reinterpret_cast<std::uint32_t*>(p) = 0x3F800000;
    p += 4;

    auto* originalLabel = p;
    *jeOriginalOffset = static_cast<std::uint8_t>(originalLabel - jeOriginalOffset - 1);

    memcpy(p, kOriginalInfiniteBreathBytes, sizeof(kOriginalInfiniteBreathBytes));
    p += sizeof(kOriginalInfiniteBreathBytes);
    EmitJump(p, kPatchInfiniteBreathReturnAddress);

    if (!WriteJump(patchAddress, g_infiniteBreathCave, sizeof(kOriginalInfiniteBreathBytes))) {
        LogError("Refusing Infinite Breath patch: failed to write hook at 0x01148936.");
        return false;
    }
    Log("Installed Infinite Breath hook at 0x01148936.");
    return true;
}

bool InstallNoFallDamagePatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = FindMainModulePattern(kFallDamagePattern, sizeof(kFallDamagePattern));
    if (!patchAddress) {
        LogWarn("Refusing No Fall Damage patch: fall damage AOB was not found.");
        return false;
    }
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing No Fall Damage patch: fall damage site is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalFallDamageBytes, sizeof(kOriginalFallDamageBytes))) {
        LogPatchMismatch("No Fall Damage patch",
                         patchAddress,
                         kOriginalFallDamageBytes,
                         sizeof(kOriginalFallDamageBytes));
        return false;
    }

    g_noFallDamageCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(128));
    if (!g_noFallDamageCave) {
        LogError("VirtualAlloc failed for No Fall Damage patch.");
        return false;
    }

    g_fallDamageAddress = patchAddress;
    std::uint8_t* p = g_noFallDamageCave;

    // inc dword ptr [&g_noFallDamageHits]
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noFallDamageHits);
    p += 4;

    // cmp byte ptr [&g_noFallDamage], 0
    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noFallDamageCell.bits);
    p += 4;
    *p++ = 0x00;

    // je original
    *p++ = 0x74;
    auto* disabledJumpOffset = p++;

    // cmp dword ptr [&g_playerEntity], edi
    *p++ = 0x39;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerEntity);
    p += 4;

    // jne original
    *p++ = 0x75;
    auto* notPlayerJumpOffset = p++;

    // movss xmm0, xmm7
    *p++ = 0xF3;
    *p++ = 0x0F;
    *p++ = 0x10;
    *p++ = 0xC7;

    auto* originalLabel = p;
    *disabledJumpOffset = static_cast<std::uint8_t>(originalLabel - disabledJumpOffset - 1);
    *notPlayerJumpOffset = static_cast<std::uint8_t>(originalLabel - notPlayerJumpOffset - 1);

    memcpy(p, kOriginalFallDamageBytes, sizeof(kOriginalFallDamageBytes));
    p += sizeof(kOriginalFallDamageBytes);
    EmitJump(p, reinterpret_cast<std::uintptr_t>(patchAddress + sizeof(kOriginalFallDamageBytes)));

    if (!WriteJump(patchAddress, g_noFallDamageCave, sizeof(kOriginalFallDamageBytes))) {
        LogError("Refusing No Fall Damage patch: failed to write fall damage hook.");
        return false;
    }

    Logf("Installed No Fall Damage hook at 0x%08X.",
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(patchAddress)));
    return true;
}

bool InstallEagleVisionSprintPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = FindMainModulePattern(kEagleVisionSprintPattern, sizeof(kEagleVisionSprintPattern));
    if (!patchAddress) {
        LogWarn("Refusing Allow Eagle Vision while sprinting patch: AOB was not found.");
        return false;
    }
    if (SafeFirstByte(patchAddress) != kOriginalEagleVisionSprintByte) {
        LogPatchMismatch("Allow Eagle Vision while sprinting patch",
                         patchAddress,
                         &kOriginalEagleVisionSprintByte,
                         sizeof(kOriginalEagleVisionSprintByte));
        return false;
    }

    g_eagleVisionSprintAddress = patchAddress;
    Logf("Allow Eagle Vision while sprinting patch ready at 0x%08X.",
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(patchAddress)));
    if (!ExpectPatchBytes(g_eagleVisionSprintAddress, &kOriginalEagleVisionSprintByte, 1)) return false;
    return true;
}

bool InstallKillCiviliansNoDesyncPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress1 = FindMainModulePattern(kCivilianDesyncPattern1, sizeof(kCivilianDesyncPattern1));
    auto* patchAddress2 = FindMainModulePattern(kCivilianDesyncPattern2, sizeof(kCivilianDesyncPattern2));
    if (!patchAddress1 || !patchAddress2) {
        LogWarn("Refusing Kill civilians without desynchronization patch: one or more AOBs were not found.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress1, kOriginalCivilianDesyncBytes, sizeof(kOriginalCivilianDesyncBytes))) {
        LogPatchMismatch("Kill civilians without desynchronization patch site 1",
                         patchAddress1,
                         kOriginalCivilianDesyncBytes,
                         sizeof(kOriginalCivilianDesyncBytes));
        return false;
    }
    if (!ExpectPatchBytes(patchAddress2, kOriginalCivilianDesyncBytes, sizeof(kOriginalCivilianDesyncBytes))) {
        LogPatchMismatch("Kill civilians without desynchronization patch site 2",
                         patchAddress2,
                         kOriginalCivilianDesyncBytes,
                         sizeof(kOriginalCivilianDesyncBytes));
        return false;
    }
    if (!ExpectPatchBytes(patchAddress1 + kCivilianWarningCallOffset,
                    kOriginalCivilianWarningCall1Bytes,
                    sizeof(kOriginalCivilianWarningCall1Bytes))) {
        LogPatchMismatch("Kill civilians without desynchronization warning call site 1",
                         patchAddress1 + kCivilianWarningCallOffset,
                         kOriginalCivilianWarningCall1Bytes,
                         sizeof(kOriginalCivilianWarningCall1Bytes));
        return false;
    }
    if (!ExpectPatchBytes(patchAddress2 + kCivilianWarningCallOffset,
                    kOriginalCivilianWarningCall2Bytes,
                    sizeof(kOriginalCivilianWarningCall2Bytes))) {
        LogPatchMismatch("Kill civilians without desynchronization warning call site 2",
                         patchAddress2 + kCivilianWarningCallOffset,
                         kOriginalCivilianWarningCall2Bytes,
                         sizeof(kOriginalCivilianWarningCall2Bytes));
        return false;
    }

    g_civilianDesyncAddress1 = patchAddress1;
    g_civilianDesyncAddress2 = patchAddress2;
    Logf("Kill civilians without desynchronization patch ready at 0x%08X and 0x%08X.",
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(patchAddress1)),
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(patchAddress2)));
    return true;
}

bool InstallShowCollectiblesPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchShowCollectiblesAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Show collectibles patch: 0x014C1D84 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalShowCollectiblesBytes, sizeof(kOriginalShowCollectiblesBytes))) {
        LogPatchMismatch("Show collectibles patch at 0x014C1D84",
                         patchAddress,
                         kOriginalShowCollectiblesBytes,
                         sizeof(kOriginalShowCollectiblesBytes));
        return false;
    }

    g_showCollectiblesCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(96));
    if (!g_showCollectiblesCave) {
        LogError("VirtualAlloc failed for Show collectibles patch.");
        return false;
    }

    std::uint8_t* p = g_showCollectiblesCave;

    // pushad
    *p++ = 0x60;

    // push edx
    *p++ = 0x52;

    EmitCall(p, reinterpret_cast<const void*>(&ShowCollectiblesVisit));

    // popad
    *p++ = 0x61;

    memcpy(p, kOriginalShowCollectiblesBytes, sizeof(kOriginalShowCollectiblesBytes));
    p += sizeof(kOriginalShowCollectiblesBytes);
    EmitJump(p, kPatchShowCollectiblesReturnAddress);

    if (!WriteJump(patchAddress, g_showCollectiblesCave, sizeof(kOriginalShowCollectiblesBytes))) {
        LogError("Refusing Show collectibles patch: failed to write hook at 0x014C1D84.");
        return false;
    }

    Log("Installed Show collectibles hook at 0x014C1D84.");
    return true;
}

bool InstallLockConsumablesPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    if (g_lockConsumablesPatchReady) {
        return true;
    }
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchLockConsumablesAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Lock Consumables patch: 0x011A1F6D is already hooked by another tool.");
        g_lockConsumablesInstallFailed = true;
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalLockConsumablesBytes, sizeof(kOriginalLockConsumablesBytes))) {
        LogPatchMismatch("Lock Consumables patch at 0x011A1F6D",
                         patchAddress,
                         kOriginalLockConsumablesBytes,
                         sizeof(kOriginalLockConsumablesBytes));
        g_lockConsumablesInstallFailed = true;
        return false;
    }

    g_lockConsumablesCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(80));
    if (!g_lockConsumablesCave) {
        LogError("VirtualAlloc failed for Lock Consumables patch.");
        g_lockConsumablesInstallFailed = true;
        return false;
    }

    std::uint8_t* p = g_lockConsumablesCave;

    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_lockConsumablesHits);
    p += 4;

    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_lockConsumablesCell.bits);
    p += 4;
    *p++ = 0x00;
    *p++ = 0x74;
    auto* jeOriginal = p++;

    // Skip the decrement, then store the unchanged amount back.
    *p++ = 0x89;
    *p++ = 0x41;
    *p++ = 0x0C;
    EmitJump(p, kPatchLockConsumablesReturnAddress);

    auto* original = p;
    *jeOriginal = static_cast<std::uint8_t>(original - jeOriginal - 1);
    memcpy(p, kOriginalLockConsumablesBytes, sizeof(kOriginalLockConsumablesBytes));
    p += sizeof(kOriginalLockConsumablesBytes);
    EmitJump(p, kPatchLockConsumablesReturnAddress);

    if (!WriteJump(patchAddress, g_lockConsumablesCave, sizeof(kOriginalLockConsumablesBytes))) {
        LogError("Refusing Lock Consumables patch: failed to write hook at 0x011A1F6D.");
        g_lockConsumablesInstallFailed = true;
        return false;
    }
    g_lockConsumablesPatchReady = true;
    g_lockConsumablesInstallFailed = false;
    Log("Installed Lock Consumables hook at 0x011A1F6D.");
    return true;
}

bool InstallInventoryPointerPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchInventoryPointerAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing inventory pointer patch: 0x01CFD381 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalInventoryPointerBytes, sizeof(kOriginalInventoryPointerBytes))) {
        LogPatchMismatch("inventory pointer patch at 0x01CFD381",
                         patchAddress,
                         kOriginalInventoryPointerBytes,
                         sizeof(kOriginalInventoryPointerBytes));
        return false;
    }

    g_inventoryPointerCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(192));
    if (!g_inventoryPointerCave) {
        LogError("VirtualAlloc failed for inventory pointer patch.");
        return false;
    }

    std::uint8_t* p = g_inventoryPointerCave;

    memcpy(p, kOriginalInventoryPointerBytes, sizeof(kOriginalInventoryPointerBytes));
    p += sizeof(kOriginalInventoryPointerBytes);

    *p++ = 0x9C; // pushfd
    *p++ = 0x53; // push ebx

    *p++ = 0x85;
    *p++ = 0xC0; // test eax,eax
    *p++ = 0x74;
    auto* jeNoBhv = p++;

    *p++ = 0xA3; // mov [&g_bhvAssassin],eax
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_bhvAssassin);
    p += 4;

    *p++ = 0x8B;
    *p++ = 0x58;
    *p++ = 0x08; // mov ebx,[eax+08]
    *p++ = 0x89;
    *p++ = 0x1D; // mov [&g_playerEntity],ebx
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_playerEntity);
    p += 4;

    *p++ = 0x8B;
    *p++ = 0x58;
    *p++ = 0x1C; // mov ebx,[eax+1C]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoCharacter = p++;

    *p++ = 0x8B;
    *p++ = 0x5B;
    *p++ = 0x5C; // mov ebx,[ebx+5C]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoInventoryHolder = p++;

    *p++ = 0x8B;
    *p++ = 0x1B; // mov ebx,[ebx]
    *p++ = 0x85;
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeNoInventory = p++;

    *p++ = 0x89;
    *p++ = 0x1D; // mov [&g_inventoryBase],ebx
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryBase);
    p += 4;

    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_inventoryPointerHits);
    p += 4;

    auto* done = p;
    *jeNoBhv = static_cast<std::uint8_t>(done - jeNoBhv - 1);
    *jeNoCharacter = static_cast<std::uint8_t>(done - jeNoCharacter - 1);
    *jeNoInventoryHolder = static_cast<std::uint8_t>(done - jeNoInventoryHolder - 1);
    *jeNoInventory = static_cast<std::uint8_t>(done - jeNoInventory - 1);

    *p++ = 0x5B; // pop ebx
    *p++ = 0x9D; // popfd
    EmitJump(p, kPatchInventoryPointerReturnAddress);

    if (!WriteJump(patchAddress, g_inventoryPointerCave, sizeof(kOriginalInventoryPointerBytes))) {
        LogError("Refusing inventory pointer patch: failed to write hook at 0x01CFD381.");
        return false;
    }
    Log("Installed inventory pointer hook at 0x01CFD381.");
    return true;
}

bool InstallNoclipUpdatePatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoclipUpdateAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing noclip update patch: 0x016FAACD is already hooked by another ASI.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalNoclipUpdateBytes, sizeof(kOriginalNoclipUpdateBytes))) {
        LogPatchMismatch("noclip update patch at 0x016FAACD",
                         patchAddress,
                         kOriginalNoclipUpdateBytes,
                         sizeof(kOriginalNoclipUpdateBytes));
        return false;
    }

    g_noclipUpdateCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(64));
    if (!g_noclipUpdateCave) {
        LogError("VirtualAlloc failed for noclip update patch.");
        return false;
    }

    std::uint8_t* p = g_noclipUpdateCave;

    // inc dword ptr [&g_noclipUpdateHits]
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noclipUpdateHits);
    p += 4;

    // pushad; call UpdateNoclipState; popad
    *p++ = 0x60;
    EmitCall(p, &UpdateNoclipState);
    *p++ = 0x61;

    memcpy(p, kOriginalNoclipUpdateBytes, sizeof(kOriginalNoclipUpdateBytes));
    p += sizeof(kOriginalNoclipUpdateBytes);
    EmitJump(p, kPatchNoclipUpdateReturnAddress);

    if (!WriteJump(patchAddress, g_noclipUpdateCave, sizeof(kOriginalNoclipUpdateBytes))) {
        LogError("Refusing noclip update patch: failed to write hook at 0x016FAACD.");
        return false;
    }
    Log("Installed noclip update hook at 0x016FAACD.");
    return true;
}

bool InstallFreeCamPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    if (g_freeCamHooksInstalled) {
        return true;
    }

    auto* rootPattern = FindMainModulePattern(kFreeCamRootPattern, sizeof(kFreeCamRootPattern));
    if (!rootPattern) {
        LogWarn("Free Cam unavailable: camera root signature was not found.");
        g_freeCamInstallFailed = true;
        return false;
    }
    g_freeCamRootStaticAddress = *reinterpret_cast<std::uint32_t*>(rootPattern + 10);

    g_freeCamTransformAddress = FindMainModulePattern(kFreeCamTransformPattern, sizeof(kFreeCamTransformPattern));
    if (!g_freeCamTransformAddress) {
        LogWarn("Free Cam unavailable: transform signature was not found.");
        g_freeCamInstallFailed = true;
        return false;
    }
    if (SafeFirstByte(g_freeCamTransformAddress) == 0xE9) {
        LogWarn("Refusing Free Cam transform patch: target is already hooked by another tool.");
        g_freeCamInstallFailed = true;
        return false;
    }
    if (!ExpectPatchBytes(g_freeCamTransformAddress,
                    kOriginalFreeCamTransformBytes,
                    sizeof(kOriginalFreeCamTransformBytes))) {
        LogPatchMismatch("Free Cam transform patch",
                         g_freeCamTransformAddress,
                         kOriginalFreeCamTransformBytes,
                         sizeof(kOriginalFreeCamTransformBytes));
        g_freeCamInstallFailed = true;
        return false;
    }

    auto* cameraPattern = FindMainModulePattern(kFreeCamCameraPattern, sizeof(kFreeCamCameraPattern));
    if (!cameraPattern) {
        LogWarn("Free Cam unavailable: camera orientation signature was not found.");
        g_freeCamInstallFailed = true;
        return false;
    }
    g_freeCamCameraAddress = cameraPattern + kFreeCamCameraPatchOffset;
    if (SafeFirstByte(g_freeCamCameraAddress) == 0xE9) {
        LogWarn("Refusing Free Cam camera patch: target is already hooked by another tool.");
        g_freeCamInstallFailed = true;
        return false;
    }
    if (!ExpectPatchBytes(g_freeCamCameraAddress,
                    kOriginalFreeCamCameraBytes,
                    sizeof(kOriginalFreeCamCameraBytes))) {
        LogPatchMismatch("Free Cam camera patch",
                         g_freeCamCameraAddress,
                         kOriginalFreeCamCameraBytes,
                         sizeof(kOriginalFreeCamCameraBytes));
        g_freeCamInstallFailed = true;
        return false;
    }

    g_freeCamTransformCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(224));
    g_freeCamCameraCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(96));
    if (!g_freeCamTransformCave || !g_freeCamCameraCave) {
        LogError("VirtualAlloc failed for Free Cam patch.");
        g_freeCamInstallFailed = true;
        return false;
    }

    std::uint8_t* p = g_freeCamTransformCave;
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamTransformHits);
    p += 4;

    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamEnabledCell.bits);
    p += 4;
    *p++ = 0x00;
    *p++ = 0x74;
    auto* jeOriginalTransform = p++;

    *p++ = 0x53; // push ebx
    *p++ = 0x52; // push edx
    *p++ = 0xBB; // mov ebx, &g_freeCamTransform
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamTransform);
    p += 4;
    *p++ = 0x8B; // mov ebx, [ebx]
    *p++ = 0x1B;
    *p++ = 0x85; // test ebx, ebx
    *p++ = 0xDB;
    *p++ = 0x74;
    auto* jeRestoreTransform = p++;

    *p++ = 0x8B; // mov edx, [&g_freeCamX]
    *p++ = 0x15;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamXCell.bits);
    p += 4;
    *p++ = 0x89; // mov [ebx+10], edx
    *p++ = 0x53;
    *p++ = 0x10;
    *p++ = 0x8B; // mov edx, [&g_freeCamY]
    *p++ = 0x15;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamYCell.bits);
    p += 4;
    *p++ = 0x89; // mov [ebx+14], edx
    *p++ = 0x53;
    *p++ = 0x14;
    *p++ = 0x8B; // mov edx, [&g_freeCamZ]
    *p++ = 0x15;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamZCell.bits);
    p += 4;
    *p++ = 0x89; // mov [ebx+18], edx
    *p++ = 0x53;
    *p++ = 0x18;

    auto* restoreTransform = p;
    *jeRestoreTransform = static_cast<std::uint8_t>(restoreTransform - jeRestoreTransform - 1);
    *p++ = 0x5A; // pop edx
    *p++ = 0x5B; // pop ebx
    *p++ = 0x0F; // movaps xmm0, [esi+10]
    *p++ = 0x28;
    *p++ = 0x46;
    *p++ = 0x10;
    *p++ = 0x0F; // movaps [esi+10], xmm0
    *p++ = 0x29;
    *p++ = 0x46;
    *p++ = 0x10;
    *p++ = 0x0F; // movaps xmm0, [edi+20]
    *p++ = 0x28;
    *p++ = 0x47;
    *p++ = 0x20;
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_freeCamTransformAddress + sizeof(kOriginalFreeCamTransformBytes)));

    auto* originalTransform = p;
    *jeOriginalTransform = static_cast<std::uint8_t>(originalTransform - jeOriginalTransform - 1);
    memcpy(p, kOriginalFreeCamTransformBytes, sizeof(kOriginalFreeCamTransformBytes));
    p += sizeof(kOriginalFreeCamTransformBytes);
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_freeCamTransformAddress + sizeof(kOriginalFreeCamTransformBytes)));

    p = g_freeCamCameraCave;
    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamCameraHits);
    p += 4;
    *p++ = 0x89; // mov [&g_freeCamCamera], edi
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freeCamCamera);
    p += 4;
    memcpy(p, kOriginalFreeCamCameraBytes, sizeof(kOriginalFreeCamCameraBytes));
    p += sizeof(kOriginalFreeCamCameraBytes);
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_freeCamCameraAddress + sizeof(kOriginalFreeCamCameraBytes)));

    const auto checkpoint = g_patches.checkpoint();
    if (!WriteJump(g_freeCamTransformAddress,
                   g_freeCamTransformCave,
                   sizeof(kOriginalFreeCamTransformBytes))) {
        LogError("Refusing Free Cam transform patch: failed to write hook.");
        g_freeCamInstallFailed = true;
        g_patches.RestoreFrom(checkpoint);
        return false;
    }
    if (!WriteJump(g_freeCamCameraAddress,
                   g_freeCamCameraCave,
                   sizeof(kOriginalFreeCamCameraBytes))) {
        LogError("Refusing Free Cam camera patch: failed to write hook.");
        g_freeCamInstallFailed = true;
        g_patches.RestoreFrom(checkpoint);
        return false;
    }

    g_freeCamHooksInstalled = true;
    g_freeCamInstallFailed = false;
    Logf("Installed Free Cam hooks: transform=0x%08X camera=0x%08X root=0x%08X.",
         reinterpret_cast<unsigned int>(g_freeCamTransformAddress),
         reinterpret_cast<unsigned int>(g_freeCamCameraAddress),
         static_cast<unsigned int>(g_freeCamRootStaticAddress));
    return true;
}

bool InstallGlobalHiddenUnlockPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    if (g_globalHiddenUnlockInstalled) {
        return true;
    }

    auto* write1Pattern = FindMainModulePattern(kGlobalHiddenUnlockWrite1Pattern, sizeof(kGlobalHiddenUnlockWrite1Pattern));
    auto* write2Pattern = FindMainModulePattern(kGlobalHiddenUnlockWrite2Pattern, sizeof(kGlobalHiddenUnlockWrite2Pattern));
    auto* visibilityPattern = FindMainModulePattern(kGlobalHiddenUnlockVisibilityPattern, sizeof(kGlobalHiddenUnlockVisibilityPattern));
    if (!write1Pattern || !write2Pattern || !visibilityPattern) {
        LogWarn("Global Hidden Unlocks unavailable: signatures were not found.");
        return false;
    }

    g_globalHiddenUnlockWrite1Address = write1Pattern + 7;
    g_globalHiddenUnlockWrite2Address = write2Pattern + 2;
    g_globalHiddenUnlockVisibilityAddress = visibilityPattern + 6;

    if (!ExpectPatchBytes(g_globalHiddenUnlockWrite1Address,
                    kOriginalGlobalHiddenUnlockWrite1Bytes,
                    sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)) ||
        !ExpectPatchBytes(g_globalHiddenUnlockWrite2Address,
                    kOriginalGlobalHiddenUnlockWrite2Bytes,
                    sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)) ||
        !ExpectPatchBytes(g_globalHiddenUnlockVisibilityAddress,
                    kOriginalGlobalHiddenUnlockVisibilityBytes,
                    sizeof(kOriginalGlobalHiddenUnlockVisibilityBytes))) {
        LogWarn("Global Hidden Unlocks unavailable: patch bytes are not in their original state.");
        return false;
    }

    g_globalHiddenUnlockCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(0x100));
    if (!g_globalHiddenUnlockCave) {
        LogError("VirtualAlloc failed for Global Hidden Unlocks patch.");
        return false;
    }

    std::uint8_t* p = g_globalHiddenUnlockCave;
    *p++ = 0xC6; *p++ = 0x41; *p++ = 0x38; *p++ = 0x00; // mov byte ptr [ecx+38],0
    *p++ = 0x89; *p++ = 0x51; *p++ = 0x0C; // mov [ecx+0C],edx
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite1Address + sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)));

    p = g_globalHiddenUnlockCave + 0x20;
    *p++ = 0x8B; *p++ = 0xF1; // mov esi,ecx
    *p++ = 0x57; // push edi
    *p++ = 0xC6; *p++ = 0x46; *p++ = 0x38; *p++ = 0x00; // mov byte ptr [esi+38],0
    EmitJump(p, reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite2Address + sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)));

    const auto checkpoint = g_patches.checkpoint();
    if (!WriteJump(g_globalHiddenUnlockWrite1Address,
                   g_globalHiddenUnlockCave,
                   sizeof(kOriginalGlobalHiddenUnlockWrite1Bytes)) ||
        !WriteJump(g_globalHiddenUnlockWrite2Address,
                   g_globalHiddenUnlockCave + 0x20,
                   sizeof(kOriginalGlobalHiddenUnlockWrite2Bytes)) ||
        !WriteMemory(g_globalHiddenUnlockVisibilityAddress,
                     kEnabledGlobalHiddenUnlockVisibilityBytes,
                     sizeof(kEnabledGlobalHiddenUnlockVisibilityBytes))) {
        LogError("Global Hidden Unlocks failed: could not write patches.");
        g_patches.RestoreFrom(checkpoint);
        return false;
    }

    g_globalHiddenUnlockInstalled = true;
    Logf("Installed Global Hidden Unlocks patches at 0x%08X, 0x%08X, visibility 0x%08X.",
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite1Address)),
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockWrite2Address)),
         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(g_globalHiddenUnlockVisibilityAddress)));
    return true;
}

bool InstallNoReloadPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchNoReloadAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing No Reload patch: 0x016B8A96 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalNoReloadBytes, sizeof(kOriginalNoReloadBytes))) {
        LogPatchMismatch("No Reload patch at 0x016B8A96",
                         patchAddress,
                         kOriginalNoReloadBytes,
                         sizeof(kOriginalNoReloadBytes));
        return false;
    }

    g_noReloadCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(96));
    if (!g_noReloadCave) {
        LogError("VirtualAlloc failed for No Reload patch.");
        return false;
    }

    std::uint8_t* p = g_noReloadCave;

    memcpy(p, kOriginalNoReloadBytes, sizeof(kOriginalNoReloadBytes));
    p += sizeof(kOriginalNoReloadBytes);

    // Keep flags exactly as the original two MOV instructions would.
    *p++ = 0x9C; // pushfd

    *p++ = 0xF0; // lock: shared hit counter
    *p++ = 0xFF;
    *p++ = 0x05;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noReloadHits);
    p += 4;

    *p++ = 0x80;
    *p++ = 0x3D;
    *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_noReloadCell.bits);
    p += 4;
    *p++ = 0x00;

    *p++ = 0x74;
    auto* jeDone = p++;

    *p++ = 0x31;
    *p++ = 0xD2; // xor edx,edx

    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);

    *p++ = 0x9D; // popfd
    EmitJump(p, kPatchNoReloadReturnAddress);

    if (!WriteJump(patchAddress, g_noReloadCave, sizeof(kOriginalNoReloadBytes))) {
        LogError("Refusing No Reload patch: failed to write hook at 0x016B8A96.");
        return false;
    }
    Log("Installed No Reload hook at 0x016B8A96.");
    return true;
}

bool InstallMissionTimerPatch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchMissionTimerAddress);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Mission Timer patch: 0x019446C3 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalMissionTimerBytes, sizeof(kOriginalMissionTimerBytes))) {
        LogPatchMismatch("Mission Timer patch at 0x019446C3",
                         patchAddress,
                         kOriginalMissionTimerBytes,
                         sizeof(kOriginalMissionTimerBytes));
        return false;
    }

    g_missionTimerCave = static_cast<std::uint8_t*>(
        g_caves.Allocate(192));
    if (!g_missionTimerCave) {
        LogError("VirtualAlloc failed for Mission Timer patch.");
        return false;
    }

    std::uint8_t* p = g_missionTimerCave;
    *p++ = 0x9C; // pushfd
    *p++ = 0xF0; *p++ = 0xFF; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerHits); p += 4;
    *p++ = 0x53; *p++ = 0x57; // push ebx; push edi
    *p++ = 0x85; *p++ = 0xC9; // test ecx,ecx
    *p++ = 0x74; auto* jeDone = p++;
    *p++ = 0x80; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freezeMissionTimerCell.bits); p += 4; *p++ = 0;
    *p++ = 0x74; auto* jeReset = p++;
    *p++ = 0x8B; *p++ = 0x59; *p++ = 0x48; // mov ebx,[ecx+48]
    *p++ = 0x8B; *p++ = 0x79; *p++ = 0x5C; // mov edi,[ecx+5C]
    *p++ = 0x85; *p++ = 0xFF;
    *p++ = 0x74; auto* jeDone2 = p++;
    *p++ = 0x8B; *p++ = 0x3F; // mov edi,[edi]
    *p++ = 0x83; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4; *p++ = 0;
    *p++ = 0x75; auto* jneHaveDiff = p++;
    *p++ = 0x2B; *p++ = 0xDF; // sub ebx,edi
    *p++ = 0x89; *p++ = 0x1D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4;
    auto* haveDiff = p;
    *jneHaveDiff = static_cast<std::uint8_t>(haveDiff - jneHaveDiff - 1);
    *p++ = 0x03; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4;
    *p++ = 0x89; *p++ = 0x79; *p++ = 0x48; // mov [ecx+48],edi
    *p++ = 0xEB; auto* jmpDone = p++;
    auto* reset = p;
    *jeReset = static_cast<std::uint8_t>(reset - jeReset - 1);
    *p++ = 0xC7; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimerDiffer); p += 4; *reinterpret_cast<std::uint32_t*>(p) = 0; p += 4;
    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);
    *jeDone2 = static_cast<std::uint8_t>(done - jeDone2 - 1);
    *jmpDone = static_cast<std::uint8_t>(done - jmpDone - 1);
    *p++ = 0x5F; *p++ = 0x5B; // pop edi; pop ebx
    *p++ = 0x9D; // popfd
    memcpy(p, kOriginalMissionTimerBytes, sizeof(kOriginalMissionTimerBytes));
    p += sizeof(kOriginalMissionTimerBytes);
    EmitJump(p, kPatchMissionTimerReturnAddress);

    if (!WriteJump(patchAddress, g_missionTimerCave, sizeof(kOriginalMissionTimerBytes))) {
        LogError("Refusing Mission Timer patch: failed to write hook at 0x019446C3.");
        return false;
    }
    Log("Installed Mission Timer hook at 0x019446C3.");
    return true;
}

bool InstallMissionTimer2Patch() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    auto* patchAddress = reinterpret_cast<std::uint8_t*>(kPatchMissionTimer2Address);
    if (SafeFirstByte(patchAddress) == 0xE9) {
        LogWarn("Refusing Mission Timer II patch: 0x016869B7 is already hooked by another tool.");
        return false;
    }
    if (!ExpectPatchBytes(patchAddress, kOriginalMissionTimer2Bytes, sizeof(kOriginalMissionTimer2Bytes))) {
        LogPatchMismatch("Mission Timer II patch at 0x016869B7",
                         patchAddress,
                         kOriginalMissionTimer2Bytes,
                         sizeof(kOriginalMissionTimer2Bytes));
        return false;
    }

    g_missionTimer2Cave = static_cast<std::uint8_t*>(
        g_caves.Allocate(192));
    if (!g_missionTimer2Cave) {
        LogError("VirtualAlloc failed for Mission Timer II patch.");
        return false;
    }

    std::uint8_t* p = g_missionTimer2Cave;
    *p++ = 0x9C; // pushfd
    *p++ = 0xF0; *p++ = 0xFF; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Hits); p += 4;
    *p++ = 0x53; *p++ = 0x57;
    *p++ = 0x85; *p++ = 0xF6; // test esi,esi
    *p++ = 0x74; auto* jeDone = p++;
    *p++ = 0x80; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_freezeMissionTimerCell.bits); p += 4; *p++ = 0;
    *p++ = 0x74; auto* jeReset = p++;
    *p++ = 0x8B; *p++ = 0x9E; *reinterpret_cast<std::uint32_t*>(p) = 0x00000110; p += 4;
    *p++ = 0x8B; *p++ = 0xBE; *reinterpret_cast<std::uint32_t*>(p) = 0x00000124; p += 4;
    *p++ = 0x85; *p++ = 0xFF;
    *p++ = 0x74; auto* jeDone2 = p++;
    *p++ = 0x8B; *p++ = 0x3F;
    *p++ = 0x83; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4; *p++ = 0;
    *p++ = 0x75; auto* jneHaveDiff = p++;
    *p++ = 0x2B; *p++ = 0xDF;
    *p++ = 0x89; *p++ = 0x1D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4;
    auto* haveDiff = p;
    *jneHaveDiff = static_cast<std::uint8_t>(haveDiff - jneHaveDiff - 1);
    *p++ = 0x03; *p++ = 0x3D; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4;
    *p++ = 0x89; *p++ = 0xBE; *reinterpret_cast<std::uint32_t*>(p) = 0x00000110; p += 4;
    *p++ = 0xEB; auto* jmpDone = p++;
    auto* reset = p;
    *jeReset = static_cast<std::uint8_t>(reset - jeReset - 1);
    *p++ = 0xC7; *p++ = 0x05; *reinterpret_cast<std::uint32_t*>(p) = reinterpret_cast<std::uint32_t>(&g_missionTimer2Differ); p += 4; *reinterpret_cast<std::uint32_t*>(p) = 0; p += 4;
    auto* done = p;
    *jeDone = static_cast<std::uint8_t>(done - jeDone - 1);
    *jeDone2 = static_cast<std::uint8_t>(done - jeDone2 - 1);
    *jmpDone = static_cast<std::uint8_t>(done - jmpDone - 1);
    *p++ = 0x5F; *p++ = 0x5B;
    *p++ = 0x9D; // popfd
    memcpy(p, kOriginalMissionTimer2Bytes, sizeof(kOriginalMissionTimer2Bytes));
    p += sizeof(kOriginalMissionTimer2Bytes);
    EmitJump(p, kPatchMissionTimer2ReturnAddress);

    if (!WriteJump(patchAddress, g_missionTimer2Cave, sizeof(kOriginalMissionTimer2Bytes))) {
        LogError("Refusing Mission Timer II patch: failed to write hook at 0x016869B7.");
        return false;
    }
    Log("Installed Mission Timer II hook at 0x016869B7.");
    return true;
}

bool InstallPatches() {
    if (!g_supportedBuild || g_stopping.load()) return false;
    g_shipPatchReady = InstallHealthPatch();
    g_noCannonCooldownPatchReady = InstallNoCannonCooldownPatch();
    g_allyGodmodePatchReady = InstallAllyGodmodePatch();
    g_timeScalePatchReady = InstallTimeIntervalPatch();
    g_playerHealthPatchReady = InstallPlayerHealthPatch();
    g_infiniteBreathPatchReady = InstallInfiniteBreathPatch();
    g_stealthModePatchReady = InstallStealthModePatch();
    g_noReloadPatchReady = InstallNoReloadPatch();
    g_noFallDamagePatchReady = InstallNoFallDamagePatch();
    g_eagleVisionSprintPatchReady = InstallEagleVisionSprintPatch();
    g_killCiviliansNoDesyncPatchReady = InstallKillCiviliansNoDesyncPatch();
    g_showCollectiblesPatchReady = InstallShowCollectiblesPatch();
    g_missionTimerPatchReady = InstallMissionTimerPatch();
    g_missionTimer2PatchReady = InstallMissionTimer2Patch();
    g_missionTimersPatchReady = g_missionTimerPatchReady || g_missionTimer2PatchReady;
    g_inventoryPointerPatchReady = InstallInventoryPointerPatch();
    g_noclipPatchReady = InstallNoclipUpdatePatch();

    if (!g_shipPatchReady) {
        g_enabled = false;
    }
    if (!g_noCannonCooldownPatchReady) {
        g_noCannonCooldown = false;
    }
    if (!g_allyGodmodePatchReady) {
        g_allyGodmode = false;
    }
    if (!g_timeScalePatchReady) {
        g_timeScaleEnabled = false;
    }
    if (!g_playerHealthPatchReady) {
        g_playerGodmode = false;
    }
    if (!g_infiniteBreathPatchReady) {
        g_infiniteBreath = false;
    }
    if (!g_stealthModePatchReady) {
        g_stealthMode = false;
    }
    if (!g_noReloadPatchReady) {
        g_noReload = false;
    }
    if (!g_noFallDamagePatchReady) {
        g_noFallDamage = false;
    }
    if (!g_eagleVisionSprintPatchReady) {
        g_eagleVisionSprint = false;
    }
    if (!g_killCiviliansNoDesyncPatchReady) {
        g_killCiviliansNoDesync = false;
    }
    if (!g_showCollectiblesPatchReady) {
        g_showCollectibles = false;
    }
    if (!g_missionTimersPatchReady) {
        g_freezeMissionTimer = false;
    }
    if (!g_noclipPatchReady) {
        g_noclipEnabled = false;
    }

    if (!g_shipPatchReady &&
        !g_noCannonCooldownPatchReady &&
        !g_allyGodmodePatchReady &&
        !g_timeScalePatchReady &&
        !g_playerHealthPatchReady &&
        !g_infiniteBreathPatchReady &&
        !g_stealthModePatchReady &&
        !g_noReloadPatchReady &&
        !g_noFallDamagePatchReady &&
        !g_eagleVisionSprintPatchReady &&
        !g_killCiviliansNoDesyncPatchReady &&
        !g_showCollectiblesPatchReady &&
        !g_missionTimersPatchReady &&
        !g_inventoryPointerPatchReady &&
        !g_noclipPatchReady) {
        LogWarn("No gameplay patches installed; UI will load with features unavailable.");
        return false;
    }
    g_installed = true;
    Logf("AC4Tools standalone patches installed: ship=%d cannonCooldown=%d allyGodmode=%d timescale=%d player=%d breath=%d stealth=%d noReload=%d noFallDamage=%d eagleVisionSprint=%d civilianNoDesync=%d showCollectibles=%d timers=%d lockConsumables=%d inventoryPointer=%d noclip=%d.",
         g_shipPatchReady ? 1 : 0,
         g_noCannonCooldownPatchReady ? 1 : 0,
         g_allyGodmodePatchReady ? 1 : 0,
         g_timeScalePatchReady ? 1 : 0,
         g_playerHealthPatchReady ? 1 : 0,
         g_infiniteBreathPatchReady ? 1 : 0,
         g_stealthModePatchReady ? 1 : 0,
         g_noReloadPatchReady ? 1 : 0,
         g_noFallDamagePatchReady ? 1 : 0,
         g_eagleVisionSprintPatchReady ? 1 : 0,
         g_killCiviliansNoDesyncPatchReady ? 1 : 0,
         g_showCollectiblesPatchReady ? 1 : 0,
         g_missionTimersPatchReady ? 1 : 0,
         g_lockConsumablesPatchReady ? 1 : 0,
         g_inventoryPointerPatchReady ? 1 : 0,
         g_noclipPatchReady ? 1 : 0);
    return true;
}
