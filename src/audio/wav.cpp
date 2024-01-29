#include "wav.hpp"

#include <climits>
#include <cstring>

bool isDataHeader(const char *headerPtr) {
    return headerPtr[0] == 'd' &&
           headerPtr[1] == 'a' &&
           headerPtr[2] == 't' &&
           headerPtr[3] == 'a';
}
inline float normalizeShortToFloat(short value) {
    const float shortMin = static_cast<float>(SHRT_MIN);
    const float shortMax = static_cast<float>(SHRT_MAX);

    // Map the short value to the range of -1.0 to 1.0.
    float normalizedFloat = (static_cast<float>(value) - shortMin) / (shortMax - shortMin) * 2.0f - 1.0f;

    return normalizedFloat;
}

PcmF32Buffer wav_init_f32(unsigned char *wavFileData,
    unsigned char *destBuffer,
    size_t destBufferSize) {
    while (!isDataHeader(reinterpret_cast<char *>(wavFileData))) wavFileData++;
    wavFileData += 8;
    for (int i = 0; i < destBufferSize / 8; i++) {
        auto outputLoc = reinterpret_cast<float *>(destBuffer + i * 8);
        auto wavePtr = reinterpret_cast<short *>(wavFileData);
        *outputLoc = normalizeShortToFloat(*wavePtr);
        *(outputLoc + 1) = normalizeShortToFloat(*(wavePtr + 1));
        wavFileData += 4;
    }
    return {
        .data = reinterpret_cast<float *>(destBuffer),
        .len = destBufferSize / sizeof(float)
    };
}

size_t wav_getAudioSize_f32(unsigned char *wavFileData) {
    while (!isDataHeader(reinterpret_cast<char *>(wavFileData))) wavFileData++;
    return (*reinterpret_cast<int *>(wavFileData + 4)) * 2;
}
