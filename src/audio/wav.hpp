#pragma once


struct PcmF32Buffer {
    float *data;
    size_t len;
};

PcmF32Buffer wav_init_f32(unsigned char *wavFileData, unsigned char *destBuffer, size_t destBufferSize);

size_t wav_getAudioSize_f32(unsigned char *wavFileData);