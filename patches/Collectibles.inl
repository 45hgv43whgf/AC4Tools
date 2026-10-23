// Private unity fragment; included by AC4Tools.cpp inside its namespace.
bool SafeReadBytes(const void* address, void* out, std::size_t size) {
    if (!address || !out || size == 0) {
        return false;
    }
    __try {
            memcpy(out, address, size);
            return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool SafeReadU8(const void* address, std::uint8_t* out) {
    return SafeReadBytes(address, out, sizeof(*out));
}

bool SafeReadU16(const void* address, std::uint16_t* out) {
    return SafeReadBytes(address, out, sizeof(*out));
}

bool SafeReadU32(const void* address, std::uint32_t* out) {
    return SafeReadBytes(address, out, sizeof(*out));
}

bool SafeReadPtr(const std::uint8_t* base, std::uintptr_t offset, std::uint8_t** out) {
    std::uintptr_t value = 0;
    if (!SafeReadBytes(base + offset, &value, sizeof(value))) {
        return false;
    }
    *out = reinterpret_cast<std::uint8_t*>(value);
    return *out != nullptr;
}

bool DwordAtEquals(const std::uint8_t* address, std::uint32_t expected) {
    std::uint32_t value = 0;
    return SafeReadU32(address, &value) && value == expected;
}

bool IsTrackedCollectibleGroupName(const std::uint8_t* name) {
    return (DwordAtEquals(name, 0x74736241) && DwordAtEquals(name + 4, 0x6F677265)) ||
           (DwordAtEquals(name, 0x6C657641) && DwordAtEquals(name + 4, 0x00656E69));
}

bool IsTrackedCollectibleIconName(const std::uint8_t* name) {
    return (DwordAtEquals(name, 0x006F0043) &&
            DwordAtEquals(name + 4, 0x006C006C) &&
            DwordAtEquals(name + 8, 0x00630065)) ||
           (DwordAtEquals(name, 0x00720070) &&
            DwordAtEquals(name + 4, 0x00730065) &&
            DwordAtEquals(name + 8, 0x006F0050));
}

void SetCollectibleIconVisible(std::uint8_t* icon) {
    using GetMapIconVisibilityFn = bool(__thiscall*)(void*);
    using SetMapIconVisibilityFn = void(__thiscall*)(void*, int);

    auto getVisibility = reinterpret_cast<GetMapIconVisibilityFn>(kGetMapIconVisibilityAddress);
    auto setVisibility = reinterpret_cast<SetMapIconVisibilityFn>(kSetMapIconVisibilityAddress);

    __try {
            const int visible = g_showCollectibles ? 1 : (getVisibility(icon) ? 1 : 0);
            setVisibility(icon, visible);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

void VisitCollectibleIconList(std::uint8_t* iconTable, std::uint16_t count) {
    for (std::uint16_t i = 0; i < count; ++i) {
        std::uint8_t* icon = nullptr;
        if (!SafeReadPtr(iconTable, i * sizeof(std::uintptr_t), &icon)) {
            continue;
        }

        std::uint8_t* visibilityState = nullptr;
        std::uint8_t completed = 0;
        std::uint8_t suppressed = 0;
        if (!SafeReadPtr(icon, 0x58, &visibilityState) ||
            !SafeReadU8(icon + 0x6E, &completed) ||
            !SafeReadU8(icon + 0x6D, &suppressed) ||
            completed == 1 ||
            suppressed == 1) {
            continue;
        }

        std::uint8_t* info = nullptr;
        std::uint8_t* settings = nullptr;
        std::uint8_t* nameOwner = nullptr;
        std::uint8_t* name = nullptr;
        if (!SafeReadPtr(icon, 0x20, &info) ||
            !SafeReadPtr(info, 0x00, &settings) ||
            !SafeReadPtr(settings, 0x2C, &nameOwner) ||
            !SafeReadPtr(nameOwner, 0x00, &nameOwner) ||
            !SafeReadPtr(nameOwner, 0x00, &nameOwner) ||
            !SafeReadPtr(nameOwner, 0x28, &name) ||
            !IsTrackedCollectibleIconName(name)) {
            continue;
        }

        SetCollectibleIconVisible(icon);
    }
}

void __stdcall ShowCollectiblesVisit(std::uint8_t* mapRoot) {
    StateGuard stateGuard(g_stateLock);
    if (!g_supportedBuild || g_stopping.load() || !g_showCollectibles || !mapRoot) {
        return;
    }

    InterlockedIncrement(&g_showCollectiblesHits);

    std::uint8_t* group = nullptr;
    std::uint8_t* groupName = nullptr;
    std::uint8_t* iconTable = nullptr;
    std::uint16_t iconCount = 0;

    if (SafeReadPtr(mapRoot, 0x10, &group) &&
        SafeReadPtr(group, 0x00, &groupName) &&
        SafeReadPtr(groupName, 0x44, &groupName) &&
        SafeReadPtr(groupName, 0x30, &groupName) &&
        IsTrackedCollectibleGroupName(groupName) &&
        SafeReadPtr(mapRoot, 0x24, &iconTable) &&
        SafeReadU16(mapRoot + 0x2A, &iconCount)) {
        VisitCollectibleIconList(iconTable, iconCount);
        return;
    }

    std::uint8_t* listOwner = nullptr;
    if (SafeReadPtr(mapRoot, 0x14, &listOwner) &&
        SafeReadPtr(listOwner, 0x00, &listOwner) &&
        SafeReadPtr(listOwner, 0x0C, &iconTable) &&
        SafeReadU16(listOwner + 0x12, &iconCount)) {
        VisitCollectibleIconList(iconTable, iconCount);
    }
}
