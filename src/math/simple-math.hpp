#pragma once
inline float max(float a, float b) {
    return (a > b) ? a : b;
}

inline short clampToShortRange(const int value) {
    if (value > 32767) {
        return 32767;
    }
    if (value < -32768) {
        return -32768;
    }
    return static_cast<short>(value);
}
