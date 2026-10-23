#pragma once
#include <windows.h>
#include <cstdint>

// Search only committed executable pages; refuse ambiguous signatures.
// SEH also handles pages whose protection changes during the scan.
inline std::uint8_t* FindUniqueImagePattern(HMODULE module, const std::uint8_t* pattern,
                                          const char* mask, std::size_t size) {
    if (!module || !pattern || !size) return nullptr;
    __try {
        auto* base = reinterpret_cast<std::uint8_t*>(module);
        auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 || dos->e_lfanew > 0x100000) return nullptr;
        auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.SizeOfImage < size) return nullptr;
        const auto start = reinterpret_cast<std::uintptr_t>(base);
        const auto end = start + nt->OptionalHeader.SizeOfImage;
        if (end < start) return nullptr;
        std::uint8_t* found = nullptr;
        for (auto address = start; address < end;) {
            MEMORY_BASIC_INFORMATION info{};
            if (!VirtualQuery(reinterpret_cast<void*>(address), &info, sizeof(info))) return nullptr;
            auto regionEnd = reinterpret_cast<std::uintptr_t>(info.BaseAddress) + info.RegionSize;
            if (regionEnd <= address) return nullptr;
            if (regionEnd > end) regionEnd = end;
            const DWORD protect = info.Protect & 0xFF;
            if (info.State == MEM_COMMIT && !(info.Protect & PAGE_GUARD) &&
                (protect == PAGE_EXECUTE_READ || protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY) &&
                regionEnd - address >= size) {
                for (auto candidate = address; candidate <= regionEnd - size; ++candidate) {
                    auto* bytes = reinterpret_cast<std::uint8_t*>(candidate);
                    bool match = true;
                    for (std::size_t i = 0; i < size; ++i) {
                        if ((!mask || mask[i] == 'x') && bytes[i] != pattern[i]) { match = false; break; }
                    }
                    if (match) {
                        if (found) return nullptr;
                        found = bytes;
                    }
                }
            }
            address = regionEnd;
        }
        return found;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
