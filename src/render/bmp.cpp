#include "bmp.hpp";

#include <cstdlib>
#include <cstring>

bool bmp_isValid(unsigned char *bmpData, uint32_t size) {
    if (size < 2) return false;
    return bmpData[0] == 'B' && bmpData[1] == 'M';
}

#define FILE_SIZE_OFFSET 0x02
#define IMAGE_DATA_PTR_OFFSET 0x0a
#define IMG_WIDTH_OFFSET 0x12
#define IMG_HEIGHT_OFFSET 0x16
#define COLOR_DEPTH_OFFSET 0x1c

BmpImage bmp_createFromLoadedFileBuffer(unsigned char *bmpData) {
    BmpImage image {};

    if (*reinterpret_cast<uint16_t *>(bmpData + COLOR_DEPTH_OFFSET) != 24) {
        return {.imageData = nullptr};
    }

    const size_t fileSize = *reinterpret_cast<uint32_t *>(bmpData + FILE_SIZE_OFFSET);
    const size_t imageDataOffset = *reinterpret_cast<uint32_t *>(bmpData + IMAGE_DATA_PTR_OFFSET);
    const size_t bmpBufferSize = fileSize - imageDataOffset;

    image.imageData = static_cast<unsigned char *>(malloc(bmpBufferSize));

    memcpy(image.imageData, bmpData + imageDataOffset, bmpBufferSize);
    
    image.width = *reinterpret_cast<int32_t *>(bmpData + IMG_WIDTH_OFFSET);
    image.height = *reinterpret_cast<int32_t *>(bmpData + IMG_HEIGHT_OFFSET);
    
    return image;
}

void bmp_destroy(BmpImage *bmp) {
    free(bmp->imageData);
}
