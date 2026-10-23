#pragma once
#include <cstdint>
#include <cstring>

inline bool IsSupportedBuild(const char* exe, const char* sha256, std::uintptr_t imageBase,
                             const char* expectedSha256) {
    return exe && sha256 && expectedSha256 &&
        _stricmp(exe, "AC4BFSP.exe") == 0 &&
        _stricmp(sha256, expectedSha256) == 0 && imageBase == 0x00400000;
}
