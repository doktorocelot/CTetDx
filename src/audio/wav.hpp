#pragma once

struct PcmS16Buffer {
    short *data;
    size_t len;
};

PcmS16Buffer wav_init(unsigned char *wavFileData, unsigned char *destBuffer, size_t destBufferSize);

size_t wav_getAudioSize(unsigned char *wavFileData);