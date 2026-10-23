// Private unity fragment; included by AC4Tools.cpp inside its namespace.
bool WriteMemory(void* address, const void* bytes, std::size_t size) {
    if (!g_supportedBuild || g_stopping.load() || !g_patches.HasExpectation(address, size)) return false;
    return g_patches.Write(address, bytes, size);
}

bool WriteJump(std::uint8_t* address, const void* target, std::size_t patchSize) {
    if (patchSize < 5 || patchSize > 16) return false;
    std::uint8_t patch[16]{};
    patch[0] = 0xE9;
    *reinterpret_cast<std::int32_t*>(&patch[1]) =
        static_cast<std::int32_t>(reinterpret_cast<const std::uint8_t*>(target) - address - 5);
    for (std::size_t i = 5; i < patchSize; ++i) {
        patch[i] = 0x90;
    }
    MEMORY_BASIC_INFORMATION info{};
    if (!VirtualQuery(target, &info, sizeof(info))) return false;
    FlushInstructionCache(GetCurrentProcess(), info.BaseAddress, info.RegionSize);
    // Mark before publication: even a failed protection restore may leave a live jump.
    g_caves.Publish(target);
    return WriteMemory(address, patch, patchSize);
}

bool BytesMatch(const void* address, const std::uint8_t* expected, std::size_t size) {
    std::uint8_t found[64]{};
    return size <= sizeof(found) && ReadPatchBytes(address, found, size) && memcmp(found, expected, size) == 0;
}

bool ExpectPatchBytes(const void* address, const std::uint8_t* expected, std::size_t size) {
    return g_patches.Expect(address, expected, size);
}

std::uint8_t SafeFirstByte(const void* address) {
    std::uint8_t value = 0;
    ReadPatchBytes(address, &value, 1);
    return value;
}

std::uint8_t* FindMainModulePattern(const std::uint8_t* pattern, std::size_t size) {
    if (!g_supportedBuild || g_stopping.load()) return nullptr;
    return FindUniqueImagePattern(GetModuleHandleA(nullptr), pattern, nullptr, size);
}

std::uint8_t* FindMainModulePatternMasked(const std::uint8_t* pattern, const char* mask, std::size_t size) {
    if (!g_supportedBuild || g_stopping.load() || !mask) return nullptr;
    return FindUniqueImagePattern(GetModuleHandleA(nullptr), pattern, mask, size);
}

void FormatBytes(const void* bytes, std::size_t size, char* output, std::size_t outputSize) {
    output[0] = '\0';
    std::uint8_t data[64]{};
    if (size > sizeof(data) || !ReadPatchBytes(bytes, data, size)) {
        strcpy_s(output, outputSize, "unreadable"); return;
    }
    for (std::size_t i = 0; i < size; ++i) {
        char part[8]{};
        sprintf_s(part, "%s%02X", i == 0 ? "" : " ", data[i]);
        strcat_s(output, outputSize, part);
    }
}

void LogPatchMismatch(const char* label,
                      const void* address,
                      const std::uint8_t* expected,
                      std::size_t size) {
    char expectedText[96]{};
    char foundText[96]{};
    FormatBytes(expected, size, expectedText, sizeof(expectedText));
    FormatBytes(address, size, foundText, sizeof(foundText));
    LogWarnf("Refusing %s: expected [%s], found [%s].", label, expectedText, foundText);
}
