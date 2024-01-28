#include "wav.hpp"

#include <cstring>

bool isDataHeader(const char *headerPtr) {
    return headerPtr[0] == 'd' &&
           headerPtr[1] == 'a' &&
           headerPtr[2] == 't' &&
           headerPtr[3] == 'a';
}


PcmS16Buffer wav_init(unsigned char *wavFileData,
    unsigned char *destBuffer,
    size_t destBufferSize) {
    while (!isDataHeader(reinterpret_cast<char *>(wavFileData))) wavFileData++;
    wavFileData += 8;
    memcpy(destBuffer, wavFileData, destBufferSize);
    return {
        .data = reinterpret_cast<short *>(destBuffer),
        .len = destBufferSize / sizeof(short)
    };
}

size_t wav_getAudioSize(unsigned char *wavFileData) {
    while (!isDataHeader(reinterpret_cast<char *>(wavFileData))) wavFileData++;
    return *reinterpret_cast<int *>(wavFileData + 4);
}
