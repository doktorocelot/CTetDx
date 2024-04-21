#include "sound-pool.hpp"

#include <cmath>

void soundPool_add(SoundPool *pool, PcmF32Buffer *buffer) {
    if (pool->len == MAX_SOUNDS)
        pool->len--;
    pool->sounds[pool->len] = {.accumulator = 0, .pcmBuffer = buffer};
    pool->len++;
}

void writeAudio(SoundPool *pool, float *dest, size_t frames, double speed) {
    for (int f = 0; f < frames; f++) {
        dest[0] = 0;
        dest[1] = 0;
        for (int i = 0; i < pool->len; i++) {
            if (
                const auto sound = &pool->sounds[i];
                sound->accumulator < sound->pcmBuffer->lenFrames
            ) {
                const float *sampleLow = &sound->pcmBuffer->data[static_cast<int>(floor(sound->accumulator) * 2)];
                const float *sampleHi = &sound->pcmBuffer->data[static_cast<int>(ceil(sound->accumulator) * 2)];
                double discard;
                const double weightLow = modf(sound->accumulator, &discard);
                const double weightHi = 1 - weightLow;

                dest[0] += *sampleLow * weightLow + *sampleHi * weightHi;
                dest[1] += *(sampleLow + 1) * weightLow + *(sampleHi + 1) * weightHi;
                sound->accumulator += speed;
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

void writeZeroes(float *dest, size_t frames) {
    for (int f = 0; f < frames; f++) {
        dest[0] = 0;
        dest[1] = 0;
    }
}

void soundPool_mix(SoundPool *pool, float *dest, size_t frames, double speed) {
    if (pool->len <= 0) writeZeroes(dest, frames);
    else writeAudio(pool, dest, frames, speed);
}