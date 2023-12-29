#pragma once
#include <cstdint>

struct BmpImage {
    unsigned char *imageData;
    int width;
    int height;
};

bool bmp_isValid(unsigned char *bmpData, uint32_t size);

BmpImage bmp_init(unsigned char *bmpData, unsigned char *buffer);

size_t bmp_getBufferSize(unsigned char *bmpData);