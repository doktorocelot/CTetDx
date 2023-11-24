#include "bmp.hpp";

bool bmp_isValid(unsigned char *bmpData, uint32_t size) {
    if (size < 2) return false;
    return bmpData[0] == 'B' && bmpData[1] == 'M';
}
