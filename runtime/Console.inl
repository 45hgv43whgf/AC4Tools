// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void InitConsole() {
    if (!g_consoleLoggingEnabled) {
        return;
    }
    if (g_consoleReady) {
        return;
    }
    g_consoleMutex = CreateMutexA(nullptr, FALSE, "Local\\AC4ToolsConsole");
    if (!g_consoleMutex) return;
    if (GetLastError() == ERROR_ALREADY_EXISTS || !AllocConsole()) {
        CloseHandle(g_consoleMutex); g_consoleMutex = nullptr;
        return;
    }
    SetConsoleTitleA("AC4Tools v1.08 Log");
    freopen_s(&g_consoleOut, "CONOUT$", "w", stdout);
    freopen_s(&g_consoleErr, "CONOUT$", "w", stderr);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console && console != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (GetConsoleScreenBufferInfo(console, &info)) {
            g_consoleDefaultAttributes = info.wAttributes;
        }
    }
    g_consoleReady = true;
    for (int i = 0; i < g_pendingConsoleLineCount; ++i) {
        const int index = (g_pendingConsoleLineStart + i) % 96;
        WriteConsoleLogLine(ParseStampedLogLevel(g_pendingConsoleLines[index]), g_pendingConsoleLines[index]);
    }
    g_pendingConsoleLineCount = 0;
    g_pendingConsoleLineStart = 0;
    Log("AC4Tools console attached.");
}
