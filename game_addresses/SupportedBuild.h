// Private unity fragment; included by AC4Tools.cpp inside its namespace.
constexpr const char* kToolName = "AC4Tools";
constexpr const char* kToolVersion = "v1.08";
constexpr const char* kToolTitle = "AC4Tools v1.08";
constexpr const char* kSupportedGameExe = "AC4BFSP.exe";
constexpr const char* kSupportedGameSize = "45,056,040 bytes";
constexpr const char* kSupportedGameTimestamp = "2023-11-14 14:41:36";
constexpr const char* kSupportedGameSha256 =
    "732AAE5679D068EE58736C35D2627473EB6ED34B28A1AE3B11076D7AD3212ACD";

constexpr std::uintptr_t kGetPlayerRootAddress = 0x0054D4D0;
constexpr std::uintptr_t kGetActorFromRootAddress = 0x0070E810;
constexpr std::uintptr_t kGetEntityFromActorAddress = 0x013905A0;
constexpr std::uintptr_t kToggleHoodDownAddress = 0x0169F260;
constexpr std::uintptr_t kToggleHoodUpAddress = 0x0169F3C0;
constexpr std::size_t kPlayerHoodStateOffset = 0x188;
constexpr std::size_t kPlayerHoodControllerOffset = 0x10C;
constexpr std::size_t kPlayerHoodBusyFlagOffset = 0x200;
constexpr std::size_t kPlayerCrouchStateOffsetA = 0xDB8;
constexpr std::size_t kPlayerCrouchStateOffsetB = 0xDC0;
constexpr std::size_t kPlayerCrouchStateOffsetC = 0xDC8;

constexpr std::uintptr_t kPatchHealthAddress = 0x01225C3F;
constexpr std::uintptr_t kPatchHealthReturnAddress = 0x01225C47;
constexpr std::uint8_t kOriginalHealthBytes[] = {
    0xF3, 0x0F, 0x11, 0x86, 0xD0, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchNoCannonCooldownAddress = 0x01C97031;
constexpr std::uint8_t kOriginalNoCannonCooldownByte = 0x77;
constexpr std::uint8_t kEnabledNoCannonCooldownByte = 0xEB;

constexpr std::uintptr_t kPatchStealthModeAddress = 0x0101CB66;
constexpr std::uint8_t kOriginalStealthModeByte = 0x75;
constexpr std::uint8_t kEnabledStealthModeByte = 0xEB;

constexpr std::uintptr_t kPatchAllyGodmodeAddress = 0x01BFF520;
constexpr std::uint8_t kOriginalAllyGodmodeBytes[] = {
    0x0F, 0xBF, 0x41, 0x5C, 0xC3,
};

constexpr std::uintptr_t kPatchTimeIntervalAddress = 0x00A047EA;
constexpr std::uintptr_t kPatchTimeIntervalReturnAddress = 0x00A047F0;
constexpr std::uint8_t kOriginalTimeIntervalBytes[] = {
    0x8B, 0x15, 0x68, 0x3E, 0xDD, 0x04,
};

constexpr std::uintptr_t kPatchPlayerHealthAddress = 0x0115AD07;
constexpr std::uintptr_t kPatchPlayerHealthReturnAddress = 0x0115AD0D;
constexpr std::uintptr_t kSetPlayerHealthAddress = 0x0115ACC0;
constexpr std::uint8_t kOriginalPlayerHealthBytes[] = {
    0x66, 0x89, 0x46, 0x5C, 0x85, 0xFF,
};

constexpr std::uintptr_t kPatchInfiniteBreathAddress = 0x01148936;
constexpr std::uintptr_t kPatchInfiniteBreathReturnAddress = 0x0114893C;
constexpr std::uint8_t kOriginalInfiniteBreathBytes[] = {
    0xD9, 0x80, 0x80, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchNoReloadAddress = 0x016B8A96;
constexpr std::uintptr_t kPatchNoReloadReturnAddress = 0x016B8A9C;
constexpr std::uint8_t kOriginalNoReloadBytes[] = {
    0x8B, 0x50, 0x10, 0x8B, 0x48, 0x08,
};

constexpr std::uintptr_t kPatchLockConsumablesAddress = 0x011A1F6D;
constexpr std::uintptr_t kPatchLockConsumablesReturnAddress = 0x011A1F72;
constexpr std::uint8_t kOriginalLockConsumablesBytes[] = {
    0x2B, 0xC2, 0x89, 0x41, 0x0C,
};

constexpr std::uintptr_t kPatchMissionTimerAddress = 0x019446C3;
constexpr std::uintptr_t kPatchMissionTimerReturnAddress = 0x019446CB;
constexpr std::uint8_t kOriginalMissionTimerBytes[] = {
    0x51, 0x56, 0x8B, 0xF1, 0x80, 0x7E, 0x58, 0x00,
};

constexpr std::uintptr_t kPatchMissionTimer2Address = 0x016869B7;
constexpr std::uintptr_t kPatchMissionTimer2ReturnAddress = 0x016869BD;
constexpr std::uint8_t kOriginalMissionTimer2Bytes[] = {
    0xD9, 0x9E, 0xEC, 0x00, 0x00, 0x00,
};

constexpr std::uintptr_t kPatchInventoryPointerAddress = 0x01CFD381;
constexpr std::uintptr_t kPatchInventoryPointerReturnAddress = 0x01CFD387;
constexpr std::uint8_t kOriginalInventoryPointerBytes[] = {
    0x8B, 0x82, 0xE8, 0x00, 0x00, 0x00,
};

constexpr std::uint8_t kFallDamagePattern[] = {
    0x51, 0x53, 0x51, 0xF3, 0x0F, 0x11, 0x04, 0x24, 0x57,
};
constexpr std::uint8_t kOriginalFallDamageBytes[] = {
    0x51, 0x53, 0x51, 0xF3, 0x0F, 0x11, 0x04, 0x24,
};

constexpr std::uint8_t kEagleVisionSprintPattern[] = {
    0x74, 0x08, 0x50, 0x8B, 0xCA, 0xE8, 0x65, 0x0F, 0x6F,
    0xFF, 0x8B, 0x45, 0xF4, 0xF6, 0x40, 0x1C, 0x01,
};
constexpr std::uint8_t kOriginalEagleVisionSprintByte = 0x74;
constexpr std::uint8_t kEnabledEagleVisionSprintByte = 0xEB;

constexpr std::uint8_t kCivilianDesyncPattern1[] = {
    0xF3, 0x0F, 0x10, 0x80, 0xD0, 0x03, 0x00, 0x00, 0x51,
    0x8D, 0x8E, 0xD0, 0x00, 0x00, 0x00, 0xF3, 0x0F, 0x11,
    0x04, 0x24, 0xE8, 0xA6, 0xF8, 0xA0, 0xFE, 0xF3, 0x0F,
    0x10, 0x05, 0x4C, 0xE9, 0x70, 0x02, 0x6A, 0x00, 0x6A,
    0x00, 0x51, 0x8B, 0x0D, 0x48, 0x6D, 0xDE, 0x04, 0x81,
    0xC1, 0xEC,
};
constexpr std::uint8_t kCivilianDesyncPattern2[] = {
    0xF3, 0x0F, 0x10, 0x80, 0xD0, 0x03, 0x00, 0x00, 0x51,
    0x8D, 0x8E, 0xD0, 0x00, 0x00, 0x00, 0xF3, 0x0F, 0x11,
    0x04, 0x24, 0xE8, 0x7F, 0xF5, 0xA0, 0xFE, 0xF3, 0x0F,
    0x10, 0x05, 0x4C, 0xE9, 0x70, 0x02, 0x6A, 0x00, 0x6A,
    0x00, 0x51, 0x8B, 0x0D, 0x48, 0x6D, 0xDE, 0x04, 0x81,
    0xC1, 0xF4,
};
constexpr std::uint8_t kOriginalCivilianDesyncBytes[] = {
    0xF3, 0x0F, 0x10, 0x80, 0xD0, 0x03, 0x00, 0x00,
};
constexpr std::uint8_t kEnabledCivilianDesyncBytes[] = {
    0x0F, 0x57, 0xC0, 0x90, 0x90, 0x90, 0x90, 0x90,
};
constexpr std::size_t kCivilianWarningCallOffset = 0x38;
constexpr std::uint8_t kOriginalCivilianWarningCall1Bytes[] = {
    0xE8, 0x72, 0xB9, 0x5A, 0xFF,
};
constexpr std::uint8_t kOriginalCivilianWarningCall2Bytes[] = {
    0xE8, 0x4B, 0xB6, 0x5A, 0xFF,
};
constexpr std::uint8_t kDisabledCivilianWarningCallBytes[] = {
    0x90, 0x90, 0x90, 0x90, 0x90,
};

constexpr std::uintptr_t kPatchShowCollectiblesAddress = 0x014C1D84;
constexpr std::uintptr_t kPatchShowCollectiblesReturnAddress = 0x014C1D8A;
constexpr std::uintptr_t kGetMapIconVisibilityAddress = 0x00F870B0;
constexpr std::uintptr_t kSetMapIconVisibilityAddress = 0x00F73D90;
constexpr std::uint8_t kOriginalShowCollectiblesBytes[] = {
    0x8B, 0x72, 0x14, 0x8B, 0x46, 0x08,
};

constexpr std::uintptr_t kPatchNoclipUpdateAddress = 0x016FAACD;
constexpr std::uintptr_t kPatchNoclipUpdateReturnAddress = 0x016FAAD3;
constexpr std::uint8_t kOriginalNoclipUpdateBytes[] = {
    0x8B, 0x8E, 0x08, 0x01, 0x00, 0x00,
};

constexpr std::uint8_t kFreeCamRootPattern[] = {
    0x8B, 0x50, 0x4C, 0x56, 0x8B, 0xCF, 0xFF, 0xD2, 0x8B, 0x0D,
};
constexpr std::uint8_t kFreeCamTransformPattern[] = {
    0x0F, 0x29, 0x46, 0x10, 0x0F, 0x28, 0x47, 0x20,
    0x0F, 0x29, 0x46, 0x20, 0xD9, 0x87, 0xC8,
};
constexpr std::uint8_t kOriginalFreeCamTransformBytes[] = {
    0x0F, 0x29, 0x46, 0x10, 0x0F, 0x28, 0x47, 0x20,
};
constexpr std::uint8_t kFreeCamCameraPattern[] = {
    0x07, 0x0F, 0x29, 0x46, 0x40, 0x0F, 0x28, 0x47,
    0x10, 0x0F, 0x29, 0x46, 0x50,
};
constexpr std::size_t kFreeCamCameraPatchOffset = 5;
constexpr std::uint8_t kOriginalFreeCamCameraBytes[] = {
    0x0F, 0x28, 0x47, 0x10, 0x0F, 0x29, 0x46, 0x50,
};

constexpr std::uint8_t kGlobalHiddenUnlockWrite1Pattern[] = {
    0xCC, 0x8A, 0x41, 0x3D, 0x8B, 0x51, 0x24, 0x88, 0x41,
};
constexpr std::uint8_t kGlobalHiddenUnlockWrite2Pattern[] = {
    0x53, 0x56, 0x8B, 0xF1, 0x57, 0x88, 0x46, 0x38, 0xE8,
};
constexpr std::uint8_t kGlobalHiddenUnlockVisibilityPattern[] = {
    0x32, 0xC0, 0x5D, 0xC2, 0x04, 0x00, 0x8A, 0x40, 0x25, 0x5D, 0xC2,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockWrite1Bytes[] = {
    0x88, 0x41, 0x38, 0x89, 0x51, 0x0C,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockWrite2Bytes[] = {
    0x8B, 0xF1, 0x57, 0x88, 0x46, 0x38,
};
constexpr std::uint8_t kOriginalGlobalHiddenUnlockVisibilityBytes[] = {
    0x8A, 0x40, 0x25,
};
constexpr std::uint8_t kEnabledGlobalHiddenUnlockVisibilityBytes[] = {
    0xB0, 0x01, 0x90,
};
