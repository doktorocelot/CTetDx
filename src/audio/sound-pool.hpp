#pragma once
#include "wav.hpp"

#define MAX_SOUNDS 256
struct Sound {
    size_t accumulator;
    PcmS16Buffer *pcmBuffer;
};
struct SoundPool {
    Sound sounds[MAX_SOUNDS];
    size_t len;
    size_t iter;
};
void soundPool_add(SoundPool *pool, PcmS16Buffer *buffer);
void soundPool_startIter(SoundPool *pool);
const short *soundPool_next(SoundPool *pool);