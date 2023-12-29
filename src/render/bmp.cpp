#include "bmp.hpp";


bool bmp_isValid(unsigned char *bmpData, uint32_t size) {
    if (size < 2) return false;
    return bmpData[0] == 'B' && bmpData[1] == 'M';
}

#define FILE_SIZE_OFFSET 0x02
#define IMAGE_DATA_PTR_OFFSET 0x0a
#define IMG_WIDTH_OFFSET 0x12
#define IMG_HEIGHT_OFFSET 0x16
#define COLOR_DEPTH_OFFSET 0x1c

size_t static inline getBmpImageDataOffset(unsigned char *bmpData) {
    return *reinterpret_cast<uint32_t *>(bmpData + IMAGE_DATA_PTR_OFFSET);;
}

BmpImage bmp_init(unsigned char *bmpData, unsigned char *buffer) {
    BmpImage image {};

    if (*reinterpret_cast<uint16_t *>(bmpData + COLOR_DEPTH_OFFSET) != 24) {
        return {.imageData = nullptr};
    }

    const size_t imageDataOffset = getBmpImageDataOffset(bmpData);
    const size_t bmpBufferSize = bmp_getBufferSize(bmpData);

    image.imageData = buffer;

    
    image.width = *reinterpret_cast<int32_t *>(bmpData + IMG_WIDTH_OFFSET);
    image.height = *reinterpret_cast<int32_t *>(bmpData + IMG_HEIGHT_OFFSET);

    const unsigned long long totalPixels = image.width * image.height;

    const unsigned char *fileDataPtr = bmpData + imageDataOffset;
    unsigned char *imgDataPtr = buffer;
    for (int i = 0; i < totalPixels; i++) {
        imgDataPtr[0] = fileDataPtr[2];
        imgDataPtr[1] = fileDataPtr[1];
        imgDataPtr[2] = fileDataPtr[0];
        imgDataPtr[3] = 0xFF;
        
        fileDataPtr += 3;
        imgDataPtr += 4;
    }
    
    return image;
}

size_t bmp_getBufferSize(unsigned char *bmpData) {
    const size_t fileSize = *reinterpret_cast<uint32_t *>(bmpData + FILE_SIZE_OFFSET);
    const size_t imageDataOffset = getBmpImageDataOffset(bmpData);
    return (fileSize - imageDataOffset) + (fileSize - imageDataOffset) / 3;
}
