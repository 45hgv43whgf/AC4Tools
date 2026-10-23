#pragma once
#include "MinHook.h"

// MinHook is linked privately into this ASI. Never disable another owner's
// target and never enable a partially initialized set of callbacks.
class HookManager {
public:
    bool Initialize() {
        if (initialized_) return true;
        const MH_STATUS status = MH_Initialize();
        initialized_ = status == MH_OK || status == MH_ERROR_ALREADY_INITIALIZED;
        return initialized_;
    }
    MH_STATUS Create(void* target, void* detour, void** original) {
        if (!initialized_) return MH_ERROR_NOT_INITIALIZED;
        if (count_ == _countof(targets_)) return MH_ERROR_MEMORY_ALLOC;
        const MH_STATUS status = MH_CreateHook(target, detour, original);
        if (status == MH_OK) targets_[count_++] = target;
        return status;
    }
    MH_STATUS Enable(void* target) { return MH_QueueEnableHook(target); }
    bool EnableAll() { return MH_ApplyQueued() == MH_OK; }
    bool DisableAll() {
        if (!initialized_) return true;
        for (std::size_t i = 0; i < count_; ++i) {
            if (MH_QueueDisableHook(targets_[i]) != MH_OK) return false;
        }
        return MH_ApplyQueued() == MH_OK;
    }
    void ReleaseIfUnused() {
        if (initialized_ && count_ == 0) {
            MH_Uninitialize();
            initialized_ = false;
        }
    }
    // Enabled trampolines are deliberately retained after DisableAll: an
    // in-flight callback may still be returning through one. The ASI is pinned.
private:
    void* targets_[32]{};
    std::size_t count_ = 0;
    bool initialized_ = false;
};
