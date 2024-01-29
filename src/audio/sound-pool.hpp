#pragma once
#include "wav.hpp"

#define MAX_SOUNDS 256
struct Sound {
    size_t accumulator;
    PcmF32Buffer *pcmBuffer;
};
struct SoundPool {
    Sound sounds[MAX_SOUNDS];
    size_t len;
    size_t iter;
};
void soundPool_add(SoundPool *pool, PcmF32Buffer *buffer);
void soundPool_startIter(SoundPool *pool);
const float *soundPool_next(SoundPool *pool);