#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _ebfImage {
    unsigned char magicNumber[2];
    int width;
    int height;
    unsigned int ** imageData;
    long long numBytes;
} ebfImage;

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename);
int dimensionScan(int check, ebfImage *img, char *inputFIlename);
int checkData(FILE *inputFile, ebfImage *img, char *inputFilename);
void mallocTheArray(ebfImage *img);
int isBadMalloc(ebfImage *img);