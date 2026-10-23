// Private unity fragment; included by AC4Tools.cpp inside its namespace.
struct ToggleAction {
    const char* id;
    const char* label;
    bool* value;
    bool* ready;
    int hotkey;
    int gamepadHotkey;
    void (*afterToggle)();
};

void AfterNoclipToggle() {
    UpdateNoclipState();
}

void AfterTimeScaleToggle() {
    UpdateTimeScaleInterval();
}

void AfterLockConsumablesToggle() {
    if (!g_lockConsumables) {
        return;
    }
    if (!g_lockConsumablesPatchReady && !InstallLockConsumablesPatch()) {
        g_lockConsumables = false;
        return;
    }
}

void AfterFreeCamToggle() {
    g_freeCamActive = false;
    if (!g_freeCamEnabled) {
        return;
    }
    if (!g_freeCamHooksInstalled && !InstallFreeCamPatch()) {
        g_freeCamEnabled = false;
        g_freeCamActive = false;
        return;
    }
}

void AfterNoCannonCooldownToggle() {
    ApplyNoCannonCooldownPatch();
}

void AfterStealthModeToggle() {
    ApplyStealthModePatch();
}

void AfterEagleVisionSprintToggle() {
    ApplyEagleVisionSprintPatch();
}

void AfterKillCiviliansNoDesyncToggle() {
    ApplyKillCiviliansNoDesyncPatch();
}

void AfterToggleHoodTrigger() {
    if (!ToggleHood()) {
        LogWarn("Toggle Hood failed: player entity not ready.");
    }
}

void AfterToggleCrouchTrigger() {
    if (!ToggleCrouch()) {
        LogWarn("Toggle Crouch failed: player entity not ready.");
    }
}

ToggleAction g_actions[] = {
    {"ShipGodmode", "Ship Godmode", &g_enabled, &g_shipPatchReady, 0, 0, nullptr},
    {"NoCannonCooldown", "No Cannon Cooldown", &g_noCannonCooldown, &g_noCannonCooldownPatchReady, 0, 0, &AfterNoCannonCooldownToggle},
    {"AllyGodmode", "Ally Godmode", &g_allyGodmode, &g_allyGodmodePatchReady, 0, 0, nullptr},
    {"PlayerGodmode", "Player Godmode", &g_playerGodmode, &g_playerHealthPatchReady, 0, 0, nullptr},
    {"DesynchronizeYourself", "Desynchronize yourself", nullptr, &g_playerHealthPatchReady, 0, 0, &DesynchronizeYourself},
    {"InfiniteBreath", "Infinite Breath", &g_infiniteBreath, &g_infiniteBreathPatchReady, 0, 0, nullptr},
    {"StealthMode", "Stealth Mode", &g_stealthMode, &g_stealthModePatchReady, 0, 0, &AfterStealthModeToggle},
    {"NoReload", "No Reload", &g_noReload, &g_noReloadPatchReady, 0, 0, nullptr},
    {"NoFallDamage", "No Fall Damage", &g_noFallDamage, &g_noFallDamagePatchReady, 0, 0, nullptr},
    {"EagleVisionSprint", "Allow Eagle Vision while sprinting", &g_eagleVisionSprint, &g_eagleVisionSprintPatchReady, 0, 0, &AfterEagleVisionSprintToggle},
    {"KillCiviliansNoDesync", "Kill civilians without desynchronization", &g_killCiviliansNoDesync, &g_killCiviliansNoDesyncPatchReady, 0, 0, &AfterKillCiviliansNoDesyncToggle},
    {"ShowCollectibles", "Show collectibles", &g_showCollectibles, &g_showCollectiblesPatchReady, 0, 0, nullptr},
    {"LockConsumables", "Lock Consumables", &g_lockConsumables, &g_lockConsumablesHotkeyReady, 0, 0, &AfterLockConsumablesToggle},
    {"FreezeMissionTimer", "Freeze Mission Timer", &g_freezeMissionTimer, &g_missionTimersPatchReady, 0, 0, nullptr},
    {"Noclip", "Noclip", &g_noclipEnabled, &g_noclipPatchReady, 0, 0, &AfterNoclipToggle},
    {"TimeScale", "Time Scale", &g_timeScaleEnabled, &g_timeScalePatchReady, 0, 0, &AfterTimeScaleToggle},
    {"ToggleHood", "Toggle Hood", nullptr, &g_toggleHoodReady, 0, 0, &AfterToggleHoodTrigger},
    {"ToggleCrouch", "Toggle Crouch", nullptr, &g_toggleCrouchReady, 0, 0, &AfterToggleCrouchTrigger},
    {"FreeCam", "Free Cam", &g_freeCamEnabled, &g_freeCamHotkeyReady, 0, 0, &AfterFreeCamToggle},
};

constexpr int kActionCount = sizeof(g_actions) / sizeof(g_actions[0]);

struct FreeCamControlBinding {
    const char* id;
    const char* label;
    int key;
    int gamepadButton;
    int defaultKey;
    int defaultGamepadButton;
};

FreeCamControlBinding g_freeCamControls[] = {
    {"MoveForward", "Move Forward", VK_NUMPAD8, kGamepadButtonDpadUp, VK_NUMPAD8, kGamepadButtonDpadUp},
    {"MoveBackward", "Move Backward", VK_NUMPAD2, kGamepadButtonDpadDown, VK_NUMPAD2, kGamepadButtonDpadDown},
    {"StrafeLeft", "Strafe Left", VK_NUMPAD4, kGamepadButtonDpadLeft, VK_NUMPAD4, kGamepadButtonDpadLeft},
    {"StrafeRight", "Strafe Right", VK_NUMPAD6, kGamepadButtonDpadRight, VK_NUMPAD6, kGamepadButtonDpadRight},
    {"MoveUp", "Move Up", VK_NUMPAD7, kGamepadButtonLeftShoulder, VK_NUMPAD7, kGamepadButtonLeftShoulder},
    {"MoveDown", "Move Down", VK_NUMPAD9, kGamepadButtonRightShoulder, VK_NUMPAD9, kGamepadButtonRightShoulder},
    {"Boost", "Boost", VK_SHIFT, kGamepadButtonLeftTrigger, VK_SHIFT, kGamepadButtonLeftTrigger},
    {"Exit", "Exit Free Cam", VK_F10, kGamepadButtonB, VK_F10, kGamepadButtonB},
};

constexpr int kFreeCamControlCount = sizeof(g_freeCamControls) / sizeof(g_freeCamControls[0]);
constexpr int kFreeCamMoveForward = 0;
constexpr int kFreeCamMoveBackward = 1;
constexpr int kFreeCamStrafeLeft = 2;
constexpr int kFreeCamStrafeRight = 3;
constexpr int kFreeCamMoveUp = 4;
constexpr int kFreeCamMoveDown = 5;
constexpr int kFreeCamBoost = 6;
constexpr int kFreeCamExit = 7;

struct InventoryRefillEntry {
    const char* name;
    std::uintptr_t offset;
    int value;
};

InventoryRefillEntry g_inventoryRefillEntries[] = {
    {"Money", 0x00, 999999},
    {"Ship Crew", 0x80, 40},
    {"Mortar Shot Ammo", 0x84, 15},
    {"Heavy Shot Ammo", 0x88, 25},
    {"Fire Barrels", 0x8C, 25},
    {"Sugar", 0x9C, 2500},
    {"Rum", 0x98, 2500},
    {"Wood", 0xA0, 2500},
    {"Cloth", 0x90, 2500},
    {"Metal", 0x94, 2500},
    {"Bullets", 0x40, 30},
    {"Smoke Bombs", 0x04, 15},
    {"Sleep Darts", 0x78, 15},
    {"Berserk Darts", 0x7C, 15},
    {"Rope Darts", 0x54, 15},
    {"Firecrackers", 0xBC, 15},
    {"Blunderbuss", 0xDC, 15},
    {"Throwing Knives", 0xAC, 1},
    {"Harpoons", 0xA4, 40},
};

constexpr int kInventoryRefillEntryCount = sizeof(g_inventoryRefillEntries) / sizeof(g_inventoryRefillEntries[0]);
