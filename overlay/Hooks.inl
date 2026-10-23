// Private unity fragment; included by AC4Tools.cpp inside its namespace.
LRESULT CALLBACK DummyWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

bool InstallDx11Hook() {
    WNDCLASSA wc{};
    wc.lpfnWndProc = DummyWndProc;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "AC4ToolsDummyWindow";
    RegisterClassA(&wc);
    HWND hwnd = CreateWindowA(wc.lpszClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    D3D_FEATURE_LEVEL featureLevel{};
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr,
                                               D3D_DRIVER_TYPE_HARDWARE,
                                               nullptr,
                                               0,
                                               levels,
                                               2,
                                               D3D11_SDK_VERSION,
                                               &sd,
                                               &swapChain,
                                               &device,
                                               &featureLevel,
                                               &context);
    if (FAILED(hr)) {
        DestroyWindow(hwnd);
        LogError("D3D11CreateDeviceAndSwapChain dummy creation failed.");
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(swapChain);
    void* present = vtable[8];
    void* resizeBuffers = vtable[13];

    if (!g_hooks.Initialize()) {
        LogError("MinHook initialize failed.");
        swapChain->Release(); context->Release(); device->Release();
        DestroyWindow(hwnd); UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return false;
    }
    bool presentReady = false;
    bool resizeReady = false;
    if (g_hooks.Create(present, &HookPresent, reinterpret_cast<void**>(&g_originalPresent)) != MH_OK) {
        LogError("MinHook CreateHook Present failed.");
    } else if (g_hooks.Enable(present) != MH_OK) {
        LogError("MinHook EnableHook Present failed.");
    } else {
        presentReady = true;
        Log("DX11 Present hook queued.");
    }

    if (g_hooks.Create(resizeBuffers, &HookResizeBuffers, reinterpret_cast<void**>(&g_originalResizeBuffers)) != MH_OK) {
        LogWarn("MinHook CreateHook ResizeBuffers failed.");
    } else if (g_hooks.Enable(resizeBuffers) != MH_OK) {
        LogWarn("MinHook EnableHook ResizeBuffers failed.");
    } else {
        resizeReady = true;
        Log("DX11 ResizeBuffers hook queued.");
    }

    swapChain->Release();
    context->Release();
    device->Release();
    DestroyWindow(hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
    return presentReady && resizeReady;
}
