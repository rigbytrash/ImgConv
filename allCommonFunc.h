#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int checkargs(int argCount);

int checkReadFileAccess(char *filename);

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename, int MAGIC_NUMBER);

int dimensionScan(int check, int height, int width, char *inputFilename, int MIN_DIMENSION, int MAX_DIMENSION);