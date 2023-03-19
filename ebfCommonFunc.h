#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "struct.h"

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename);
int dimensionScan(int check, ebfImage *img, char *inputFIlename);
int checkData(FILE *inputFile, ebfImage *img, char *inputFilename);
void mallocTheArray(ebfImage *img);
int isBadMalloc(ebfImage *img);
