#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _ebuImage {
    unsigned char magicNumber[2];
    int width;
    int height;
    unsigned char ** imageData;
    long long numBytes;
} ebuImage;

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename);
int dimensionScan(int check, ebuImage *img, char *inputFilename);
int checkData(FILE *inputFile, ebuImage *img, char *inputFilename);
void mallocTheArray(ebuImage *img);