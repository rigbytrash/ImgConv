#include <stdint.h>

typedef struct _Image {
    unsigned char magicNumber[2];
    int width;
    int height;
    uint8_t ** imageData;
    uint8_t * dataBlock;
    long long numBytes;
} Image;
