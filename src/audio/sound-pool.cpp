#include "sound-pool.hpp"
void soundPool_add(SoundPool *pool, PcmF32Buffer *buffer) {
    if (pool->len == MAX_SOUNDS) pool->len--;
    pool->sounds[pool->len] = {.accumulator = 0, .pcmBuffer = buffer};
    pool->len++;
}

void soundPool_startIter(SoundPool *pool) {
    pool->iter = 0;
}

const float *soundPool_next(SoundPool *pool) {
    while (true) {
        if (pool->iter == pool->len) return nullptr;
        if (const auto sound = &pool->sounds[pool->iter];
            sound->accumulator < sound->pcmBuffer->len) {
            const float *sample = &sound->pcmBuffer->data[sound->accumulator];
            sound->accumulator += 2;
            pool->iter++;
            return sample;
        }
        pool->sounds[pool->iter] = pool->sounds[pool->len - 1];
        pool->len--;
    }
}
