#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "struct.h"

int checkargs(int argCount);

int checkReadFileAccess(char *filename);

int printEBU(Image *img, FILE *outputFile, char* outputFilename, int check);

int printEBF(Image *img, FILE *outputFile, char* outputFilename, int check);

int printEBC(Image *img, FILE *outputFile, char* outputFilename, int check);