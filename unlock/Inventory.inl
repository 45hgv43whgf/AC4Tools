// Private unity fragment; included by AC4Tools.cpp inside its namespace.
bool WriteInventoryValueByOffset(std::uintptr_t itemOffset, int value, bool forceSet) {
    if (!g_supportedBuild || g_stopping.load()) return false;
    if (!g_inventoryBase) {
        return false;
    }

    __try {
            auto* inventory = reinterpret_cast<std::uint8_t*>(g_inventoryBase.load());
            auto* itemTable = *reinterpret_cast<std::uint8_t**>(inventory + 0x0C);
            if (!itemTable) {
                return false;
            }

            auto* item = *reinterpret_cast<std::uint8_t**>(itemTable + itemOffset);
            if (!item) {
                return false;
            }

            auto* amount = reinterpret_cast<int*>(item + 0x0C);
            if (forceSet || *amount < value) {
                *amount = value;
                ++g_inventoryPointerLastWrites;
            }
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool AddInventoryValueByOffset(std::uintptr_t itemOffset, int delta, const char* label) {
    if (!g_supportedBuild || g_stopping.load()) return false;
    if (!g_inventoryBase) {
        return false;
    }

    __try {
            auto* inventory = reinterpret_cast<std::uint8_t*>(g_inventoryBase.load());
            auto* itemTable = *reinterpret_cast<std::uint8_t**>(inventory + 0x0C);
            if (!itemTable) {
                return false;
            }

            auto* item = *reinterpret_cast<std::uint8_t**>(itemTable + itemOffset);
            if (!item) {
                return false;
            }

            auto* amount = reinterpret_cast<int*>(item + 0x0C);
            const int oldValue = *amount;
            const int newValue = SaturatingAdd(oldValue, delta);
            *amount = newValue;
            ++g_inventoryPointerLastWrites;
            Logf("Added %d %s via inventory pointer: %d -> %d.", delta, label, oldValue, newValue);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool RefillSelectedInventoryEntry() {
    if (g_inventoryRefillSelected < 0 || g_inventoryRefillSelected >= kInventoryRefillEntryCount) {
        g_inventoryRefillLastResult = -1;
        return false;
    }

    const InventoryRefillEntry& entry = g_inventoryRefillEntries[g_inventoryRefillSelected];
    const bool ok = WriteInventoryValueByOffset(entry.offset, entry.value, g_inventoryRefillsAreSetters);
    g_inventoryRefillLastResult = ok ? 1 : -1;
    if (ok) {
        Logf("%s %s to %d via inventory pointer.",
             g_inventoryRefillsAreSetters ? "Set" : "Refilled",
             entry.name,
             entry.value);
    } else {
        LogWarnf("Refill failed for %s: inventory pointer is not ready.", entry.name);
    }
    return ok;
}

bool RefillInventoryEntryToAmount(int entryIndex, int value) {
    if (entryIndex < 0 || entryIndex >= kInventoryRefillEntryCount) {
        g_inventoryRefillLastResult = -1;
        return false;
    }

    if (value < 0) {
        value = 0;
    }

    const InventoryRefillEntry& entry = g_inventoryRefillEntries[entryIndex];
    const bool ok = WriteInventoryValueByOffset(entry.offset, value, g_inventoryRefillsAreSetters);
    g_inventoryRefillLastResult = ok ? 1 : -1;
    if (ok) {
        Logf("%s %s to custom amount %d via inventory pointer.",
             g_inventoryRefillsAreSetters ? "Set" : "Refilled",
             entry.name,
             value);
    } else {
        LogWarnf("Custom refill failed for %s: inventory pointer is not ready.", entry.name);
    }
    return ok;
}
