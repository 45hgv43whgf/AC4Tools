#include <windows.h>
#include <cstdio>

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    HMODULE module = LoadLibraryA(argv[1]);
    if (!module) { printf("LoadLibrary failed: %lu\n", GetLastError()); return 1; }
    auto stop = reinterpret_cast<void(WINAPI*)()>(GetProcAddress(module, "_AC4ToolsRequestShutdown@0"));
    auto stopped = reinterpret_cast<BOOL(WINAPI*)()>(GetProcAddress(module, "_AC4ToolsIsStopped@0"));
    if (!stop || !stopped) { puts("Missing lifecycle exports."); return 1; }
    // Give the actual ASI time to run discovery in this unsupported process.
    Sleep(1000);
    FreeLibrary(module);
    HMODULE mapped = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(stop), &mapped) || mapped != module) {
        puts("ASI was not pinned before worker startup."); return 1;
    }
    stop();
    const ULONGLONG deadline = GetTickCount64() + 10000;
    while (!stopped() && GetTickCount64() < deadline) Sleep(25);
    if (!stopped()) { puts("ASI did not stop within 10 seconds."); return 1; }
    stop(); // repeated requests are harmless
    puts("PASS: compiled ASI loads in unsupported AC4BFSP.exe, remains pinned, and stops.");
    return 0;
}
