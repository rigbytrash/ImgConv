#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "struct.h"
#include "constants.h"

int checkargs(int argCount, char *name);

int checkReadFileAccess(Image *img);

int checkMagicNumber(Image *img, int MAGIC_NUMBER);

int dimensionScan(Image *img);

int printEBU(Image *img, char *nFilename);

int printEBF(Image *img, char *nFilename);

int printEBC(Image *img, char *nFilename);

void mallocTheArray(Image *img);

int isBadMalloc(Image *img);

void setFileR(Image *img, char *name);

void setFileW(Image *img, char *name);

int readInFile(Image *img, char *type, char *nFilename);

int checkEbfData(Image *img);

int compare(Image *image1, Image *image2);

Image *newImage();

void disposeImage(Image *img);