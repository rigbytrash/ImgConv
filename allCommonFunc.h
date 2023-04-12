#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "struct.h"
#include "constants.h"

int checkargs(int argCount);

int checkReadFileAccess(char *filename);

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename, int MAGIC_NUMBER);

int dimensionScan(int check, Image *img, char *inputFIlename);

int printEBU(Image *img, FILE *outputFile, char* outputFilename, int check);

int printEBF(Image *img, FILE *outputFile, char* outputFilename, int check);

int printEBC(Image *img, FILE *outputFile, char* outputFilename, int check);

void mallocTheArray(Image *img);

int isBadMalloc(Image *img);