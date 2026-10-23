#pragma once
#include <windows.h>

// Every C++ entry from a game/render/input thread takes this recursive lock.
// WndProc uses try-lock: synchronous window messages must not deadlock Present.
class StateLock {
public:
    StateLock() { InitializeCriticalSection(&section_); }
    ~StateLock() { DeleteCriticalSection(&section_); }
    void lock() { EnterCriticalSection(&section_); }
    void unlock() { LeaveCriticalSection(&section_); }
    bool try_lock() { return TryEnterCriticalSection(&section_) != FALSE; }
private:
    CRITICAL_SECTION section_{};
};

class StateGuard {
public:
    explicit StateGuard(StateLock& lock, bool tryOnly = false) : lock_(lock) {
        if (tryOnly) held_ = lock_.try_lock();
        else { lock_.lock(); held_ = true; }
    }
    ~StateGuard() { unlock(); }
    explicit operator bool() const { return held_; }
    void unlock() { if (held_) { held_ = false; lock_.unlock(); } }
    void lock() { if (!held_) { lock_.lock(); held_ = true; } }
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
private:
    StateLock& lock_;
    bool held_ = false;
};
