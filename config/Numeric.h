#pragma once
#include <cerrno>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdlib>

inline float ParseFiniteFloat(const char* text, float fallback) {
    if (!text) return fallback;
    char* end = nullptr;
    errno = 0;
    const float value = strtof(text, &end);
    if (end == text || errno == ERANGE || !std::isfinite(value)) return fallback;
    while (*end && std::isspace(static_cast<unsigned char>(*end))) ++end;
    return *end ? fallback : value;
}

inline int SaturatingAdd(int value, int delta) {
    if (delta > 0 && value > INT_MAX - delta) return INT_MAX;
    if (delta < 0 && value < INT_MIN - delta) return INT_MIN;
    return value + delta;
}
