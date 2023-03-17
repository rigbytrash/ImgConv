#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "struct.h"

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename);
int dimensionScan(int check, Image *img, char *inputFilename);
int checkData(FILE *inputFile, Image *img, char *inputFilename);
void mallocTheArray(Image *img);
int isBadMalloc(Image *img);