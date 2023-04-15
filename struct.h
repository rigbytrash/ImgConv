#include <stdint.h>
#include <stdio.h>

typedef struct _Image {
    unsigned char magicNumber[2];
    int width;
    int height;
    int check;
    char *AFilename;
    uint8_t ** imageData;
    uint8_t * dataBlock;
    long long numBytes;
    FILE *associatedFile;
    unsigned short * magicNumberValue;
} Image;
