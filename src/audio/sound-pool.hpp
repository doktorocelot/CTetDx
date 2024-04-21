#pragma once
#include "wav.hpp"

#define MAX_SOUNDS 256
struct SoundSample {
    float left, right;
};
struct Sound {
    double accumulator;
    PcmF32Buffer *pcmBuffer;
};
struct SoundPool {
    Sound sounds[MAX_SOUNDS];
    size_t len;
    unsigned int killList[MAX_SOUNDS];
    size_t killListLen;
};
void soundPool_add(SoundPool *pool, PcmF32Buffer *buffer);
void soundPool_mix(SoundPool *pool, float *dest, size_t frames, double speed);
