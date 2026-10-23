// Private unity fragment; included by AC4Tools.cpp inside its namespace.
const char* LogLevelName(LogLevel level) {
    switch (level) {
        case LogLevel::Warning:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Info:
        default:
            return "INFO";
    }
}

WORD LogLevelConsoleAttributes(LogLevel level) {
    switch (level) {
        case LogLevel::Warning:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case LogLevel::Error:
            return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case LogLevel::Info:
        default:
            return g_consoleDefaultAttributes;
    }
}

void WriteConsoleLogLine(LogLevel level, const char* line) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console && console != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(console, LogLevelConsoleAttributes(level));
    }
    fprintf(g_consoleOut, "%s\n", line);
    fflush(g_consoleOut);
    if (console && console != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(console, g_consoleDefaultAttributes);
    }
}

LogLevel ParseStampedLogLevel(const char* line) {
    const char* secondBracket = strstr(line, "] [");
    if (!secondBracket) {
        return LogLevel::Info;
    }
    const char* level = secondBracket + 3;
    if (strncmp(level, "WARN]", 5) == 0) {
        return LogLevel::Warning;
    }
    if (strncmp(level, "ERROR]", 6) == 0) {
        return LogLevel::Error;
    }
    return LogLevel::Info;
}

void LogMessage(LogLevel level, const char* message) {
    if (!g_consoleLoggingEnabled && !g_fileLoggingEnabled) {
        return;
    }

    SYSTEMTIME now{};
    GetLocalTime(&now);
    char stamped[768]{};
    _snprintf_s(stamped,
                sizeof(stamped),
                _TRUNCATE,
                "[%04u-%02u-%02u %02u:%02u:%02u.%03u] [%s] %s",
                now.wYear,
                now.wMonth,
                now.wDay,
                now.wHour,
                now.wMinute,
                now.wSecond,
                now.wMilliseconds,
                LogLevelName(level),
                message);

    if (g_consoleLoggingEnabled && g_consoleOut) {
        WriteConsoleLogLine(level, stamped);
    } else if (g_consoleLoggingEnabled) {
        const int index = (g_pendingConsoleLineStart + g_pendingConsoleLineCount) % 96;
        strncpy_s(g_pendingConsoleLines[index], stamped, _TRUNCATE);
        if (g_pendingConsoleLineCount < 96) {
            ++g_pendingConsoleLineCount;
        } else {
            g_pendingConsoleLineStart = (g_pendingConsoleLineStart + 1) % 96;
        }
    }

    if (!g_fileLoggingEnabled) {
        return;
    }

    char path[MAX_PATH]{};
    strcpy_s(path, g_moduleDir);
    strcat_s(path, "AC4Tools.log");

    FILE* file = nullptr;
    fopen_s(&file, path, "a");
    if (!file) {
        return;
    }
    fputs(stamped, file);
    fputc('\n', file);
    fclose(file);
}

void Log(const char* message) {
    LogMessage(LogLevel::Info, message);
}

void LogWarn(const char* message) {
    LogMessage(LogLevel::Warning, message);
}

void LogError(const char* message) {
    LogMessage(LogLevel::Error, message);
}

void Logf(const char* format, ...) {
    char message[512]{};
    va_list args;
    va_start(args, format);
    vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
    va_end(args);
    Log(message);
}

void LogWarnf(const char* format, ...) {
    char message[512]{};
    va_list args;
    va_start(args, format);
    vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
    va_end(args);
    LogWarn(message);
}

void LogErrorf(const char* format, ...) {
    char message[512]{};
    va_list args;
    va_start(args, format);
    vsnprintf_s(message, sizeof(message), _TRUNCATE, format, args);
    va_end(args);
    LogError(message);
}
