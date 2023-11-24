#pragma once
#include <cstdint>

struct BmpImage {
    unsigned char *imageData;
    int width;
    int height;
};

bool bmp_isValid(unsigned char *bmpData, uint32_t size);

BmpImage bmp_createFromLoadedFileBuffer(unsigned char *bmpData);

void bmp_destroy(BmpImage *bmp);