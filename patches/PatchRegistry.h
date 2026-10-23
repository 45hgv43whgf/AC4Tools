#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <cstring>

inline bool ReadPatchBytes(const void* address, void* out, std::size_t size) {
    __try { memcpy(out, address, size); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// No heap allocations or logging while peers are suspended. Refuse a write if
// any thread cannot be inspected or is inside an instruction being replaced.
class FrozenThreads {
public:
    FrozenThreads() {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return;
        THREADENTRY32 entry{};
        entry.dwSize = sizeof(entry);
        bool ok = Thread32First(snapshot, &entry) != FALSE;
        if (ok) do {
            if (entry.th32OwnerProcessID != GetCurrentProcessId() ||
                entry.th32ThreadID == GetCurrentThreadId()) continue;
            if (count_ == _countof(threads_)) { ok = false; break; }
            HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT |
                                       THREAD_QUERY_INFORMATION, FALSE, entry.th32ThreadID);
            if (!thread) {
                if (GetLastError() == ERROR_INVALID_PARAMETER) continue; // exited
                ok = false; break;
            }
            threads_[count_++] = thread;
        } while (Thread32Next(snapshot, &entry));
        if (ok && GetLastError() != ERROR_NO_MORE_FILES) ok = false;
        CloseHandle(snapshot);
        if (!ok) return;
        for (; suspended_ < count_; ++suspended_) {
            if (SuspendThread(threads_[suspended_]) == DWORD(-1)) return;
        }
        ready_ = true;
    }
    ~FrozenThreads() {
        for (std::size_t i = 0; i < suspended_; ++i) ResumeThread(threads_[i]);
        for (std::size_t i = 0; i < count_; ++i) CloseHandle(threads_[i]);
    }
    bool CanWrite(const void* address, std::size_t size) const {
        if (!ready_) return false;
        const auto start = reinterpret_cast<std::uintptr_t>(address);
        for (std::size_t i = 0; i < count_; ++i) {
            CONTEXT context{};
            context.ContextFlags = CONTEXT_CONTROL;
            if (!GetThreadContext(threads_[i], &context)) return false;
            if (context.Eip > start && context.Eip < start + size) return false;
        }
        return true;
    }
private:
    HANDLE threads_[4096]{};
    std::size_t count_ = 0;
    std::size_t suspended_ = 0;
    bool ready_ = false;
};

class PatchRegistry {
public:
    bool Expect(const void* address, const void* bytes, std::size_t size) {
        if (!address || !bytes || !size || size > 16) return false;
        std::uint8_t actual[16]{};
        if (!ReadPatchBytes(address, actual, size) || memcmp(actual, bytes, size)) return false;
        for (std::size_t i = 0; i < expectedCount_; ++i) {
            if (expected_[i].address == address) {
                return expected_[i].size == size && !memcmp(expected_[i].original, bytes, size);
            }
        }
        if (expectedCount_ == _countof(expected_)) return false;
        Record& record = expected_[expectedCount_++];
        record.address = const_cast<void*>(address); record.size = size;
        memcpy(record.original, bytes, size);
        return true;
    }
    bool Write(void* address, const void* bytes, std::size_t size) {
        if (!address || !bytes || size == 0 || size > 16) return false;
        Record* record = nullptr;
        for (std::size_t i = 0; i < count_; ++i) {
            if (records_[i].address == address) { record = &records_[i]; break; }
            const auto start = reinterpret_cast<std::uintptr_t>(address);
            const auto other = reinterpret_cast<std::uintptr_t>(records_[i].address);
            if (start < other + records_[i].size && other < start + size) return false;
        }
        if (record && record->size != size) return false;
        if (!record && count_ == _countof(records_)) return false;
        FrozenThreads frozen;
        if (!frozen.CanWrite(address, size)) return false;
        std::uint8_t current[16]{};
        if (!ReadPatchBytes(address, current, size)) return false;
        if (record && memcmp(current, record->last, size) != 0) return false;
        if (!record) {
            // Revalidate the installer's expected bytes while peers are frozen,
            // rather than trusting an earlier probe before cave construction.
            for (std::size_t i = 0; i < expectedCount_; ++i) {
                if (expected_[i].address == address &&
                    (expected_[i].size != size || memcmp(current, expected_[i].original, size))) return false;
            }
            record = &records_[count_];
            record->address = address;
            record->size = size;
            memcpy(record->original, current, size);
        }
        DWORD oldProtect = 0;
        if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) return false;
        memcpy(address, bytes, size);
        FlushInstructionCache(GetCurrentProcess(), address, size);
        DWORD ignored = 0;
        const bool protectedAgain = VirtualProtect(address, size, oldProtect, &ignored) != FALSE;
        memcpy(record->last, bytes, size);
        if (record == &records_[count_]) ++count_;
        return protectedAgain;
    }
    bool HasExpectation(const void* address, std::size_t size) const {
        for (std::size_t i = 0; i < expectedCount_; ++i) {
            if (expected_[i].address == address && expected_[i].size == size) return true;
        }
        return false;
    }
    std::size_t checkpoint() const { return count_; }
    bool RestoreFrom(std::size_t first = 0) {
        bool ok = true;
        // Keep records on failure so cleanup can be retried without touching
        // bytes another tool has acquired since our last write.
        for (std::size_t i = count_; i > first; --i) {
            Record& record = records_[i - 1];
            if (!Write(record.address, record.original, record.size)) ok = false;
        }
        return ok;
    }
private:
    struct Record {
        void* address = nullptr;
        std::size_t size = 0;
        std::uint8_t original[16]{};
        std::uint8_t last[16]{};
    } records_[64]{};
    std::size_t count_ = 0;
    Record expected_[64]{};
    std::size_t expectedCount_ = 0;
};

class CaveRegistry {
public:
    void* Allocate(std::size_t size) {
        if (count_ == _countof(caves_)) return nullptr;
        void* memory = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (memory) caves_[count_++] = { memory, size, false };
        return memory;
    }
    void Publish(const void* target) {
        const auto value = reinterpret_cast<std::uintptr_t>(target);
        for (std::size_t i = 0; i < count_; ++i) {
            const auto start = reinterpret_cast<std::uintptr_t>(caves_[i].address);
            if (value >= start && value < start + caves_[i].size) caves_[i].published = true;
        }
    }
    void ReleaseUnpublished() {
        for (std::size_t i = 0; i < count_; ++i) {
            if (!caves_[i].published && caves_[i].address) {
                VirtualFree(caves_[i].address, 0, MEM_RELEASE);
                caves_[i].address = nullptr;
            }
        }
    }
private:
    struct Cave { void* address; std::size_t size; bool published; } caves_[32]{};
    std::size_t count_ = 0;
};
