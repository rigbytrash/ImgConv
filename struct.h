#include <stdint.h>

typedef struct _Image {
    unsigned char magicNumber[2];
    int width;
    int height;
    uint8_t ** imageData;
    uint8_t * dataBlock;
    long long numBytes;
} Image;

typedef struct _ebfImage {
    unsigned char magicNumber[2];
    int width;
    int height;
    unsigned int ** imageData;
    unsigned int * dataBlock;
    long long numBytes;
} ebfImage;
