// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void* GetPlayerActor() {
    __try {
        if (!g_supportedBuild || (g_stopping.load() && !g_finishingGameState)) return nullptr;
        using GetPlayerRootFn = void*(__cdecl*)();
        using GetActorFn = void*(__thiscall*)(void*);

        void* root = reinterpret_cast<GetPlayerRootFn>(kGetPlayerRootAddress)();
        if (!root) {
            return nullptr;
        }
        return reinterpret_cast<GetActorFn>(kGetActorFromRootAddress)(root);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

void* GetPlayerEntity() {
    __try {
        if (!g_supportedBuild || (g_stopping.load() && !g_finishingGameState)) return nullptr;
        using GetEntityFn = void*(__cdecl*)(void*);
        void* actor = GetPlayerActor();
        return actor ? reinterpret_cast<GetEntityFn>(kGetEntityFromActorAddress)(actor) : nullptr;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

int GetPlayerHoodState() {
    __try {
        void* entity = GetPlayerEntity();
        if (!entity) {
            return -1;
        }

        return *reinterpret_cast<int*>(static_cast<std::uint8_t*>(entity) + kPlayerHoodStateOffset);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

int GetPlayerCrouchState() {
    __try {
        void* entity = GetPlayerEntity();
        if (!entity) {
            return -1;
        }

        auto* entityBytes = static_cast<std::uint8_t*>(entity);
        const int stateA = *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetA);
        const int stateB = *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetB);
        const int stateC = *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetC);
        return (stateA > 0 || stateB > 0 || stateC > 0) ? 1 : 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

bool ToggleHood() {
    __try {
        if (!g_supportedBuild || g_stopping.load()) return false;
        void* entity = GetPlayerEntity();
        if (!entity) return false;
        auto* bytes = static_cast<std::uint8_t*>(entity);
        const int hoodState = *reinterpret_cast<int*>(bytes + kPlayerHoodStateOffset);
        const std::uint8_t previousBusy = bytes[kPlayerHoodBusyFlagOffset];
        bytes[kPlayerHoodBusyFlagOffset] = 1;
        const std::uintptr_t targetAddress =
            hoodState == 1 ? kToggleHoodUpAddress : kToggleHoodDownAddress;
        bool changed = false;
        __try {
            __asm {
                mov ecx, entity
                add ecx, kPlayerHoodControllerOffset
                xor edx, edx
                push 0
                push 0
                push 0
                call targetAddress
            }
            changed = GetPlayerHoodState() != hoodState;
        } __finally {
            // Restore the original entity's flag even if the game call faults.
            TryWriteByte(reinterpret_cast<std::uintptr_t>(bytes + kPlayerHoodBusyFlagOffset), previousBusy);
        }
        return changed;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool ToggleCrouch() {
    __try {
        if (!g_supportedBuild || g_stopping.load()) return false;
        void* entity = GetPlayerEntity();
        if (!entity) {
            return false;
        }

        auto* entityBytes = static_cast<std::uint8_t*>(entity);
        const int crouchState = GetPlayerCrouchState();
        if (crouchState < 0) {
            return false;
        }

        const int nextState = crouchState == 0 ? 1 : 0;
        *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetA) = nextState;
        *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetB) = nextState;
        *reinterpret_cast<int*>(entityBytes + kPlayerCrouchStateOffsetC) = nextState;
        return GetPlayerCrouchState() != crouchState;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool IsGameNoclipActive() {
    __try {
        if (!g_supportedBuild || (g_stopping.load() && !g_finishingGameState)) return false;
        void* entity = GetPlayerEntity();
        if (!entity) {
            return false;
        }
        using GetStateFn = int(__thiscall*)(void*, int);
        return reinterpret_cast<GetStateFn>(0x0052C2E0)(entity, 1) == 3;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void* GetNoclipComponent() {
    __try {
        if (!g_supportedBuild || (g_stopping.load() && !g_finishingGameState)) return nullptr;
        void* actor = GetPlayerActor();
        if (!actor) {
            return nullptr;
        }
        using GetComponentFn = void*(__thiscall*)(void*);
        return reinterpret_cast<GetComponentFn>(0x005220D0)(actor);
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

void UpdateNoclipStateImpl() {
    __try {
        if (!g_supportedBuild || (g_stopping.load() && !g_finishingGameState)) return;
        if (!g_noclipPatchReady) {
            g_noclipEnabled = false;
            return;
        }

        const bool active = IsGameNoclipActive();
        g_noclipActive = active;
        if (active == g_noclipEnabled) {
            return;
        }

        void* component = GetNoclipComponent();
        if (!component) {
            return;
        }

        if (g_noclipEnabled) {
            using EnableFn = void(__thiscall*)(void*, int);
            reinterpret_cast<EnableFn>(0x017FFDD0)(component, 0);
            Log("Noclip enable requested through game component.");
        } else {
            using DisableFn = void(__thiscall*)(void*);
            reinterpret_cast<DisableFn>(0x017FFE40)(component);
            Log("Noclip disable requested through game component.");
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) { g_noclipEnabled = false; }
}

void __stdcall UpdateNoclipState() {
    StateGuard stateGuard(g_stateLock);
    UpdateNoclipStateImpl();
    PublishCaveState();
}
