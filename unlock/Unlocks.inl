// Private unity fragment; included by AC4Tools.cpp inside its namespace.
enum class UnlockCategory {
    Pistols,
    Swords,
    Outfits,
    ShipCosmetics,
    EliteUnlocks,
};

enum class UnlockMode {
    ItemFlag,
    NormalBundle,
};

struct UnlockEntry {
    const char* name;
    UnlockCategory category;
    UnlockMode mode;
    std::uint8_t group;
    std::uint8_t id[8];
    std::uintptr_t flagAddress;
    bool enabled;
    bool found;
    bool patched;
};

UnlockEntry g_unlockEntries[] = {
    {"Golden Flintlock Pistols", UnlockCategory::Pistols, UnlockMode::ItemFlag, 0x01, {0xC0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Captain's Wheellock Pistols (CC)", UnlockCategory::Pistols, UnlockMode::ItemFlag, 0x01, {0xF0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Precision Shooter (Freedom Cry)", UnlockCategory::Pistols, UnlockMode::NormalBundle, 0x02, {0x1A, 0x9A, 0x37, 0x66, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Pistol Swords", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x01, {0xCA, 0x7A, 0x0A, 0x1D, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Scottish Broadsword", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x01, {0xA8, 0xC9, 0x45, 0xD2, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Persian Scimitars (CC)", UnlockCategory::Swords, UnlockMode::ItemFlag, 0x01, {0xE2, 0x30, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Crude Iron Machete (Freedom Cry)", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x02, {0xFD, 0xCB, 0x57, 0x7B, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Mayan Machete (Freedom Cry)", UnlockCategory::Swords, UnlockMode::NormalBundle, 0x02, {0x22, 0x2F, 0x67, 0x81, 0x0F, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Governor Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0x27, 0x70, 0x48, 0xD5, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Templar Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0x12, 0x43, 0x85, 0x16, 0x0D, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Stealth Outfit", UnlockCategory::Outfits, UnlockMode::NormalBundle, 0x01, {0xCB, 0x7A, 0x0A, 0x1D, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Explorer Outfit (CC)", UnlockCategory::Outfits, UnlockMode::ItemFlag, 0x01, {0x33, 0x6D, 0x1F, 0x1C, 0x09, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Gilded Sails", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xF7, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"The Ranger Figurehead & Queen Anne's Revenge Wheel", UnlockCategory::ShipCosmetics, UnlockMode::NormalBundle, 0x01, {0x83, 0xD9, 0x85, 0x3E, 0x0D, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Aquila Figurehead", UnlockCategory::ShipCosmetics, UnlockMode::NormalBundle, 0x01, {0x2A, 0x70, 0x48, 0xD5, 0x0C, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"El Impoluto Figurehead (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xC1, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"El Impoluto Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x78, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Black and Red Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xE8, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Queen Anne Figurehead (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x9E, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Blackwood Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xF7, 0x2D, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Aquila Wheel (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x8C, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Flower Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0x85, 0x2B, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Grey Sails (CC)", UnlockCategory::ShipCosmetics, UnlockMode::ItemFlag, 0x01, {0xED, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, 0, false, false, false},

    {"Elite Hull (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xE2, 0x45, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Set of Cannons (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x65, 0xD4, 0x41, 0x72, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Ram (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x1E, 0x4D, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Round Shot (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xA5, 0x4A, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Mortars (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x94, 0x34, 0x9F, 0x5F, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Swivel Guns (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x4C, 0x4E, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Heavy Shot (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xC9, 0x4B, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Fire Barrel (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x2A, 0x50, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Heavy Shot Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xDB, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Mortar Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xC7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Fire Barrel Storage (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0xE7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
    {"Elite Harpoon (CC)", UnlockCategory::EliteUnlocks, UnlockMode::ItemFlag, 0x01, {0x67, 0x51, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, 0, false, false, false},
};

constexpr int kUnlockEntryCount = sizeof(g_unlockEntries) / sizeof(g_unlockEntries[0]);
constexpr std::uintptr_t kUnlockRecordPointerBackOffset = 8;

int CountUnlockEntries(UnlockCategory category) {
    int count = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].category == category) {
            ++count;
        }
    }
    return count;
}

int CountEnabledUnlocks() {
    int count = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled) {
            ++count;
        }
    }
    return count;
}

void UpdateUnlockCounters() {
    g_unlockRecordsFound = 0;
    g_unlockRecordsPatched = 0;
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].found) {
            ++g_unlockRecordsFound;
        }
        if (g_unlockEntries[i].patched) {
            ++g_unlockRecordsPatched;
        }
    }
}

bool IsReadableDataPage(DWORD protect) {
    if ((protect & PAGE_GUARD) != 0 || (protect & PAGE_NOACCESS) != 0) {
        return false;
    }

    const DWORD basicProtect = protect & 0xFF;
    return basicProtect == PAGE_READONLY ||
           basicProtect == PAGE_READWRITE ||
           basicProtect == PAGE_WRITECOPY;
}

bool TryReadPointer(std::uintptr_t address, std::uintptr_t* value) {
    if (!value) {
        return false;
    }

    __try {
            *value = *reinterpret_cast<std::uintptr_t*>(address);
            return *value != 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadByte(std::uintptr_t address, std::uint8_t* value) {
    if (!value) {
        return false;
    }

    __try {
            *value = *reinterpret_cast<std::uint8_t*>(address);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadWord(std::uintptr_t address, std::uint16_t* value) {
    if (!value) {
        return false;
    }

    __try {
            *value = *reinterpret_cast<std::uint16_t*>(address);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryReadDword(std::uintptr_t address, std::uint32_t* value) {
    if (!value) {
        return false;
    }

    __try {
            *value = *reinterpret_cast<std::uint32_t*>(address);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteByte(std::uintptr_t address, std::uint8_t value) {
    __try {
            *reinterpret_cast<std::uint8_t*>(address) = value;
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteWord(std::uintptr_t address, std::uint16_t value) {
    __try {
            *reinterpret_cast<std::uint16_t*>(address) = value;
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool TryWriteDword(std::uintptr_t address, std::uint32_t value) {
    __try {
            *reinterpret_cast<std::uint32_t*>(address) = value;
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool UnlockPatternMatches(const std::uint8_t* bytes, const UnlockEntry& entry) {
    return bytes[0] == entry.group &&
           bytes[1] == 0x00 &&
           bytes[2] == 0x00 &&
           bytes[3] == 0x80 &&
           memcmp(bytes + 4, entry.id, 8) == 0;
}

UnlockEntry* FindUnlockEntry(const std::uint8_t* bytes) {
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled && UnlockPatternMatches(bytes, g_unlockEntries[i])) {
            return &g_unlockEntries[i];
        }
    }
    return nullptr;
}

struct CommunityChallengeMapping {
    std::uint8_t itemId[8];
    std::uint8_t challengeId[8];
};

CommunityChallengeMapping g_communityChallengeMappings[] = {
    {{0xE2, 0x30, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, {0x1A, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xF0, 0x2D, 0x03, 0xEC, 0x07, 0x00, 0x00, 0x00}, {0x0A, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x33, 0x6D, 0x1F, 0x1C, 0x09, 0x00, 0x00, 0x00}, {0x16, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xC1, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x12, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x78, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x06, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0xE8, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x0E, 0x47, 0x9E, 0xD4, 0x0C, 0x00, 0x00, 0x00}},
    {{0x9E, 0xEC, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x1F, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xF7, 0x2D, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, {0xC2, 0xB0, 0x24, 0x2F, 0x08, 0x00, 0x00, 0x00}},
    {{0x8C, 0xF2, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x23, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x85, 0x2B, 0x4A, 0x7B, 0x07, 0x00, 0x00, 0x00}, {0xC6, 0xB0, 0x24, 0x2F, 0x08, 0x00, 0x00, 0x00}},
    {{0xED, 0xF1, 0x34, 0x7A, 0x08, 0x00, 0x00, 0x00}, {0x17, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xE2, 0x45, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xE3, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x65, 0xD4, 0x41, 0x72, 0x07, 0x00, 0x00, 0x00}, {0xEB, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x1E, 0x4D, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xE7, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xA5, 0x4A, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xFB, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x94, 0x34, 0x9F, 0x5F, 0x07, 0x00, 0x00, 0x00}, {0xF3, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x4C, 0x4E, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0xF7, 0x36, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xC9, 0x4B, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x07, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0x2A, 0x50, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x0B, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
    {{0xDB, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xEA, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0xC7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xE2, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0xE7, 0x90, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}, {0xE6, 0x9A, 0xCA, 0xF3, 0x07, 0x00, 0x00, 0x00}},
    {{0x67, 0x51, 0xC8, 0x73, 0x07, 0x00, 0x00, 0x00}, {0x13, 0x37, 0xCF, 0xB0, 0x07, 0x00, 0x00, 0x00}},
};

const std::uint8_t* FindCommunityChallengeId(const UnlockEntry& entry) {
    for (int i = 0; i < static_cast<int>(sizeof(g_communityChallengeMappings) / sizeof(g_communityChallengeMappings[0])); ++i) {
        if (memcmp(entry.id, g_communityChallengeMappings[i].itemId, sizeof(entry.id)) == 0) {
            return g_communityChallengeMappings[i].challengeId;
        }
    }
    return nullptr;
}

std::uintptr_t FindUnlockRecordSlot(const std::uint8_t* id) {
    if (!id) {
        return 0;
    }

    const std::uint8_t patternPrefix[] = {0x01, 0x00, 0x00, 0x80};
    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= 12 &&
            mbi.RegionSize <= 0x400000) {
            auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
            __try {
                for (std::size_t offset = 0; offset + 12 <= mbi.RegionSize; ++offset) {
                    if (memcmp(bytes + offset, patternPrefix, sizeof(patternPrefix)) == 0 &&
                        memcmp(bytes + offset + 4, id, 8) == 0 &&
                        base + offset >= kUnlockRecordPointerBackOffset) {
                        return base + offset - kUnlockRecordPointerBackOffset;
                    }
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
            }
        }

        if (next <= address) {
            break;
        }
        address = next;
    }
    return 0;
}

std::uintptr_t FindCommunityHandlerValue() {
    const std::uint8_t pattern[] = {
        0x8D, 0x4E, 0x5C, 0xC7, 0x06, 0x00, 0x00, 0x00, 0x00,
        0xE8, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x46, 0x64, 0x00,
    };
    const char mask[] = "xxxxx????x????xxxx";
    auto* hit = FindMainModulePatternMasked(pattern, mask, sizeof(pattern));
    if (!hit) {
        return 0;
    }

    std::uint32_t value = 0;
    return TryReadDword(reinterpret_cast<std::uintptr_t>(hit + 5), &value) ? value : 0;
}

std::uintptr_t FindCommunityMemMap2() {
    const std::uint8_t pattern[] = {
        0x3B, 0xF3, 0x74, 0x35, 0x57, 0x8B, 0x3E, 0x85, 0xFF, 0x74, 0x1D, 0x8B, 0x0D,
    };
    auto* hit = FindMainModulePattern(pattern, sizeof(pattern));
    if (!hit) {
        return 0;
    }

    std::uint32_t memMapAddress = 0;
    std::uint32_t memMapRoot = 0;
    std::uint32_t memMap2 = 0;
    if (!TryReadDword(reinterpret_cast<std::uintptr_t>(hit + 13), &memMapAddress) ||
        !TryReadDword(memMapAddress, &memMapRoot) ||
        !TryReadDword(memMapRoot + 444, &memMap2)) {
        return 0;
    }
    return memMap2;
}

void MarkCommunityAllocation(std::uintptr_t allocation) {
    const std::uintptr_t memMap2 = FindCommunityMemMap2();
    if (memMap2 != 0) {
        TryWriteByte(memMap2 + (allocation >> 16), 129);
    }
}

std::uint8_t* AllocateCommunityMemory(std::size_t size) {
    auto* memory = static_cast<std::uint8_t*>(
        VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (memory) {
        MarkCommunityAllocation(reinterpret_cast<std::uintptr_t>(memory));
    }
    return memory;
}

bool FindCommunityChallengeList(std::uintptr_t* listFound) {
    if (!listFound) {
        return false;
    }

    const std::uintptr_t handlerValue = FindCommunityHandlerValue();
    if (handlerValue == 0) {
        return false;
    }

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= sizeof(std::uint32_t) &&
            mbi.RegionSize <= 0x400000) {
            auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
            __try {
                for (std::size_t offset = 0; offset + sizeof(std::uint32_t) <= mbi.RegionSize; offset += 4) {
                    if (*reinterpret_cast<const std::uint32_t*>(bytes + offset) != handlerValue) {
                        continue;
                    }
                    const std::uintptr_t candidate = base + offset;
                    std::uintptr_t point1 = 0;
                    std::uintptr_t point1Level1 = 0;
                    std::uintptr_t point1Level2 = 0;
                    std::uintptr_t point2 = 0;
                    std::uintptr_t list = 0;
                    std::uint8_t marker = 0;
                    if (TryReadPointer(candidate + 0x20, &point1) &&
                        point1 != candidate &&
                        TryReadPointer(point1 + 0x5C, &point1Level1) &&
                        TryReadPointer(point1Level1, &point1Level2) &&
                        TryReadPointer(point1Level2 + 0x0C, &point2) &&
                        TryReadPointer(point2 + 0x08, &list) &&
                        TryReadByte(list + 0x04, &marker) &&
                        marker == 9) {
                        *listFound = list;
                        return true;
                    }
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
            }
        }

        if (next <= address) {
            break;
        }
        address = next;
    }
    return false;
}

bool FinishCommunityChallengeForUnlock(const UnlockEntry& entry) {
    const std::uint8_t* challengeId = FindCommunityChallengeId(entry);
    if (!challengeId) {
        return false;
    }

    const std::uintptr_t challengeRecordSlot = FindUnlockRecordSlot(challengeId);
    if (challengeRecordSlot == 0) {
        LogWarnf("%s community challenge not finished: challenge record was not found.", entry.name);
        return false;
    }

    std::uintptr_t listFound = 0;
    if (!FindCommunityChallengeList(&listFound)) {
        LogWarnf("%s community challenge not finished: challenge list was not found.", entry.name);
        return false;
    }

    std::uint16_t length = 0;
    std::uint16_t capacity = 0;
    std::uintptr_t listArray = 0;
    if (!TryReadWord(listFound + 0x16, &length) ||
        !TryReadWord(listFound + 0x14, &capacity) ||
        !TryReadPointer(listFound + 0x10, &listArray) ||
        length > 1760) {
        LogWarnf("%s community challenge not finished: challenge list metadata was invalid.", entry.name);
        return false;
    }

    if (!g_communityChallengeStorage) {
        g_communityChallengeStorage = AllocateCommunityMemory(8192);
        if (!g_communityChallengeStorage) {
            LogError("Community challenge storage allocation failed.");
            return false;
        }
        g_communityChallengeNext = g_communityChallengeStorage + 0x400;
    }

    if (g_communityChallengeNext + 12 > g_communityChallengeStorage + 8192) {
        LogWarn("Community challenge storage is full.");
        return false;
    }

    std::uint32_t const1 = 0;
    std::uint32_t const2 = 0;
    std::uintptr_t firstEntry = 0;
    if (!TryReadPointer(listArray, &firstEntry) ||
        !TryReadDword(firstEntry, &const1) ||
        !TryReadDword(firstEntry + 0x08, &const2)) {
        LogWarnf("%s community challenge not finished: challenge list template was invalid.", entry.name);
        return false;
    }

    if (static_cast<unsigned int>(length) + 1 >= capacity) {
        const std::size_t expandedSize = static_cast<std::size_t>(capacity) * sizeof(std::uint32_t) + 100;
        std::uint8_t* expanded = AllocateCommunityMemory(expandedSize);
        if (!expanded) {
            LogError("Community challenge list expansion allocation failed.");
            return false;
        }
        memcpy(expanded, reinterpret_cast<void*>(listArray), static_cast<std::size_t>(length) * sizeof(std::uint32_t));
        if (!TryWriteDword(reinterpret_cast<std::uintptr_t>(expanded) + 4 * length,
                           reinterpret_cast<std::uint32_t>(g_communityChallengeNext)) ||
            !TryWriteDword(listFound + 0x10, reinterpret_cast<std::uint32_t>(expanded)) ||
            !TryWriteWord(listFound + 0x14, capacity + 10)) {
            return false;
        }
    } else if (!TryWriteDword(listArray + 4 * length, reinterpret_cast<std::uint32_t>(g_communityChallengeNext))) {
        return false;
    }

    const std::uintptr_t newEntry = reinterpret_cast<std::uintptr_t>(g_communityChallengeNext);
    if (!TryWriteDword(newEntry, const1) ||
        !TryWriteDword(newEntry + 0x04, static_cast<std::uint32_t>(challengeRecordSlot)) ||
        !TryWriteDword(newEntry + 0x08, const2) ||
        !TryWriteWord(listFound + 0x16, length + 1)) {
        return false;
    }

    g_communityChallengeNext += 12;
    Logf("%s community challenge completion added.", entry.name);
    return true;
}

void ApplyItemFlagUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    std::uintptr_t item = 0;
    if (matchAddress < kUnlockRecordPointerBackOffset ||
        !TryReadPointer(matchAddress - kUnlockRecordPointerBackOffset, &item)) {
        return;
    }

    std::uint8_t currentValue = 0;
    const std::uintptr_t flagAddress = item + 0x38;
    if (!TryReadByte(flagAddress, &currentValue)) {
        return;
    }

    entry.found = true;
    entry.flagAddress = flagAddress;

    const bool unlockedNow = currentValue != 0 && TryWriteByte(flagAddress, 0);
    if (unlockedNow) {
        entry.patched = true;
        if (g_finishCommunityChallengesForUnlocks) {
            FinishCommunityChallengeForUnlock(entry);
        }
    } else if (currentValue == 0) {
        entry.patched = true;
    }
}

void ApplyNormalBundleUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    std::uintptr_t bundle = 0;
    if (matchAddress < kUnlockRecordPointerBackOffset ||
        !TryReadPointer(matchAddress - kUnlockRecordPointerBackOffset, &bundle)) {
        return;
    }

    std::uint8_t bundleState = 0;
    if (!TryReadByte(bundle + 0x0C, &bundleState)) {
        return;
    }

    entry.found = true;
    entry.flagAddress = bundle + 0x0C;
    bool patched = false;
    if (bundleState == 1 && TryWriteByte(bundle + 0x0C, 3)) {
        patched = true;
    } else if (bundleState == 3) {
        patched = true;
    }

    std::uintptr_t first = 0;
    std::uintptr_t second = 0;
    std::uintptr_t list = 0;
    if (TryReadPointer(bundle + 0x3C, &first) &&
        TryReadPointer(first, &second) &&
        TryReadPointer(second + 0x08, &list)) {
        std::uintptr_t itemPointerTable = 0;
        std::uint8_t count = 0;
        if (TryReadPointer(list + 0x24, &itemPointerTable) &&
            TryReadByte(list + 0x2A, &count)) {
            for (int i = 0; i < count && i < 64; ++i) {
                std::uintptr_t holder = 0;
                std::uintptr_t child = 0;
                if (TryReadPointer(itemPointerTable + i * sizeof(std::uintptr_t), &holder) &&
                    TryReadPointer(holder, &child)) {
                    std::uint8_t childState = 0;
                    if (TryReadByte(child + 0x38, &childState)) {
                        if (childState != 0 && TryWriteByte(child + 0x38, 0)) {
                            patched = true;
                        } else if (childState == 0) {
                            patched = true;
                        }
                    }
                }
            }
        }
    }

    entry.patched = patched;
}

void ApplyUnlockEntry(UnlockEntry& entry, std::uintptr_t matchAddress) {
    if (entry.mode == UnlockMode::ItemFlag) {
        ApplyItemFlagUnlockEntry(entry, matchAddress);
    } else {
        ApplyNormalBundleUnlockEntry(entry, matchAddress);
    }
}

void ScanUnlockRegion(std::uintptr_t base, std::size_t size) {
    auto* bytes = reinterpret_cast<const std::uint8_t*>(base);
    __try {
            for (std::size_t offset = 0; offset + 12 <= size; ++offset) {
                if (bytes[offset + 1] != 0x00 ||
                    bytes[offset + 2] != 0x00 ||
                    bytes[offset + 3] != 0x80) {
                    continue;
                }
                UnlockEntry* entry = FindUnlockEntry(bytes + offset);
                if (entry) {
                    ApplyUnlockEntry(*entry, base + offset);
                }
            }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void ScanAndApplyUnlocks() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (InterlockedCompareExchange(&g_unlockScanRunning, 1, 0) != 0) {
        return;
    }

    for (int i = 0; i < kUnlockEntryCount; ++i) {
        g_unlockEntries[i].found = false;
        g_unlockEntries[i].patched = false;
    }

    SYSTEM_INFO systemInfo{};
    GetSystemInfo(&systemInfo);
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(systemInfo.lpMinimumApplicationAddress);
    const std::uintptr_t maxAddress = reinterpret_cast<std::uintptr_t>(systemInfo.lpMaximumApplicationAddress);

    while (address < maxAddress) {
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0) {
            address += 0x10000;
            continue;
        }

        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t next = base + mbi.RegionSize;
        if (mbi.State == MEM_COMMIT &&
            IsReadableDataPage(mbi.Protect) &&
            mbi.RegionSize >= 12 &&
            mbi.RegionSize <= 0x400000) {
            ScanUnlockRegion(base, mbi.RegionSize);
        }

        if (next <= address) {
            break;
        }
        address = next;
    }

    UpdateUnlockCounters();
    Logf("Unlock scan finished: enabled=%d found=%d patched=%d.",
         CountEnabledUnlocks(),
         g_unlockRecordsFound,
         g_unlockRecordsPatched);
    for (int i = 0; i < kUnlockEntryCount; ++i) {
        if (g_unlockEntries[i].enabled && g_unlockEntries[i].patched) {
            g_unlockEntries[i].enabled = false;
        }
    }
    InterlockedExchange(&g_unlockScanRunning, 0);
}

void MaintainUnlocks() {
    if (!g_supportedBuild || g_stopping.load()) return;
    if (CountEnabledUnlocks() == 0) {
        g_unlockLastScan = 0;
        UpdateUnlockCounters();
        InterlockedExchange(&g_unlockScanRequested, 0);
        return;
    }

    const DWORD now = GetTickCount();
    const bool requested = InterlockedExchange(&g_unlockScanRequested, 0) != 0;
    if (!requested && g_unlockLastScan != 0 && now - g_unlockLastScan < 15000) {
        return;
    }
    g_unlockLastScan = now;
    ScanAndApplyUnlocks();
}
