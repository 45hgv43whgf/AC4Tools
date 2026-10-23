// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void InitModuleDir() {
    GetModuleFileNameA(reinterpret_cast<HMODULE>(&__ImageBase), g_moduleDir, MAX_PATH);
    char* slash = strrchr(g_moduleDir, '\\');
    if (slash) {
        slash[1] = '\0';
    }
}

bool CalculateFileSha256(const char* path, char* out, std::size_t outSize) {
    if (!path || !out || outSize < 65) {
        return false;
    }
    out[0] = '\0';

    HANDLE file = CreateFileA(path,
                              GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    HCRYPTPROV provider = 0;
    HCRYPTHASH hash = 0;
    bool ok = false;
    if (CryptAcquireContextA(&provider, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT) &&
        CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash)) {
        std::uint8_t buffer[64 * 1024]{};
        DWORD bytesRead = 0;
        ok = true;
        for (;;) {
            if (!ReadFile(file, buffer, sizeof(buffer), &bytesRead, nullptr)) {
                ok = false;
                break;
            }
            if (bytesRead == 0) {
                break;
            }
            if (!CryptHashData(hash, buffer, bytesRead, 0)) {
                ok = false;
                break;
            }
        }

        std::uint8_t digest[32]{};
        DWORD digestSize = sizeof(digest);
        if (ok && CryptGetHashParam(hash, HP_HASHVAL, digest, &digestSize, 0) && digestSize == sizeof(digest)) {
            for (DWORD i = 0; i < digestSize; ++i) {
                sprintf_s(out + i * 2, outSize - i * 2, "%02X", digest[i]);
            }
            out[64] = '\0';
        } else {
            ok = false;
        }
    }

    if (hash) {
        CryptDestroyHash(hash);
    }
    if (provider) {
        CryptReleaseContext(provider, 0);
    }
    CloseHandle(file);
    return ok;
}

void InitGameInfo() {
    g_supportedBuild = false;
    g_toggleHoodReady = g_toggleCrouchReady = false;
    g_freeCamHotkeyReady = g_lockConsumablesHotkeyReady = false;
    if (!GetModuleFileNameA(nullptr, g_gameExePath, MAX_PATH)) {
        return;
    }

    const char* name = strrchr(g_gameExePath, '\\');
    strcpy_s(g_gameExeName, name ? name + 1 : g_gameExePath);

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExA(g_gameExePath, GetFileExInfoStandard, &data)) {
        return;
    }

    ULARGE_INTEGER size{};
    size.HighPart = data.nFileSizeHigh;
    size.LowPart = data.nFileSizeLow;
    sprintf_s(g_gameExeSizeText, "%llu bytes", size.QuadPart);

    FILETIME localFileTime{};
    SYSTEMTIME systemTime{};
    if (FileTimeToLocalFileTime(&data.ftLastWriteTime, &localFileTime) &&
        FileTimeToSystemTime(&localFileTime, &systemTime)) {
        sprintf_s(g_gameExeTimestampText,
                  "%04u-%02u-%02u %02u:%02u:%02u",
                  systemTime.wYear,
                  systemTime.wMonth,
                  systemTime.wDay,
                  systemTime.wHour,
                  systemTime.wMinute,
                  systemTime.wSecond);
    }

    if (!CalculateFileSha256(g_gameExePath, g_gameExeSha256Text, sizeof(g_gameExeSha256Text))) {
        strcpy_s(g_gameExeSha256Text, "unknown");
    }
    // Absolute virtual addresses also require the original image base (no relocation).
    g_supportedBuild = IsSupportedBuild(g_gameExeName, g_gameExeSha256Text,
        reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)), kSupportedGameSha256);
    g_toggleHoodReady = g_toggleCrouchReady = g_supportedBuild;
    g_freeCamHotkeyReady = g_lockConsumablesHotkeyReady = g_supportedBuild;
}

void LogGameInfo() {
    Logf("%s", g_gameExeName[0] ? g_gameExeName : "unknown executable");
    Logf("Size: %s", g_gameExeSizeText);
    Logf("Timestamp: %s", g_gameExeTimestampText);
    Logf("SHA256: %s", g_gameExeSha256Text);

    if (_stricmp(g_gameExeSha256Text, "unknown") == 0) {
        LogWarn("Could not calculate game executable SHA256; all gameplay features are disabled.");
    } else if (_stricmp(g_gameExeSha256Text, kSupportedGameSha256) != 0) {
        LogWarnf("Game executable SHA256 mismatch; all gameplay features disabled. Supported SHA256: %s", kSupportedGameSha256);
    }
}
