#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>


typedef struct _Image {
    unsigned char magicNumber[2];
    int width;
    int height;
    uint8_t ** imageData;
    uint8_t * dataBlock;
    long long numBytes;
} Image;

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename);
int dimensionScan(int check, Image *img, char *inputFilename);
int checkData(FILE *inputFile, Image *img, char *inputFilename);
void mallocTheArray(Image *img);
int isBadMalloc(Image *img);