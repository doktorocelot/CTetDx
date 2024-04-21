#include "sound-pool.hpp"

void soundPool_add(SoundPool *pool, PcmF32Buffer *buffer) {
    if (pool->len == MAX_SOUNDS)
        pool->len--;
    pool->sounds[pool->len] = {.accumulator = 0, .pcmBuffer = buffer};
    pool->len++;
}

void soundPool_mix(SoundPool *pool, float *dest, size_t frames) {
    if (pool->len <= 0) return;
    for (int f = 0; f < frames; f++) {
        dest[0] = 0;
        dest[1] = 0;
        for (int i = 0; i < pool->len; i++) {
            if (
                const auto sound = &pool->sounds[i];
                sound->accumulator < sound->pcmBuffer->len
            ) {
                const float *sample = &sound->pcmBuffer->data[sound->accumulator];
                sound->accumulator += 2;
                dest[0] += *sample;
                dest[1] += *(sample + 1);
            } else {
                pool->killList[pool->killListLen] = i;
                pool->killListLen++;
            }
        }
        dest += 2;
        for (int i = pool->killListLen - 1; i >= 0; i--) {
            pool->sounds[pool->killList[i]] = pool->sounds[pool->len - 1];
            pool->len--;
        }
        pool->killListLen = 0;
    }
}