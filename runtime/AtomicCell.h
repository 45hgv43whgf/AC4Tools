#pragma once
#include <windows.h>
#include <cstring>
#include <type_traits>

// ABI for generated Win32 x86 code: an aligned, exactly four-byte cell.
// Aligned MOV loads/stores are atomic on x86; counters use LOCK INC.
// C++ accesses use Interlocked operations rather than ordinary global accesses.
template<class T> struct alignas(4) AtomicCell {
    static_assert(sizeof(T) == sizeof(LONG), "Cave cells must be 32-bit");
    static_assert(std::is_trivially_copyable<T>::value, "Cave cells hold scalars");
    volatile LONG bits = 0;
    T load() const {
        LONG raw = InterlockedCompareExchange(const_cast<volatile LONG*>(&bits), 0, 0);
        T value;
        memcpy(&value, &raw, sizeof(value));
        return value;
    }
    void store(T value) {
        LONG raw;
        memcpy(&raw, &value, sizeof(raw));
        InterlockedExchange(&bits, raw);
    }
    operator T() const { return load(); }
    AtomicCell& operator=(T value) { store(value); return *this; }
};
static_assert(sizeof(AtomicCell<LONG>) == 4, "Generated code ABI");
