#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int checkargs(int argCount){
    if (argCount != 3)
        { // check arg count
            if(argCount == 1){
                return 0;
            }
            printf("ERROR: Bad Arguments\n");
            return 1;
        } // check arg count
    return 2;
}

int checkReadFileAccess(char *filename){
    if (access(filename,F_OK) == -1){
        return 0;
    }
    if (access(filename,R_OK) == -1){
        printf("ERROR: Bad File Name (%s)\n", filename);
        return 1;
    }
    return 2;
}

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename, int MAGIC_NUMBER){
    if (*magicNumberValue != MAGIC_NUMBER){
            printf("ERROR: Bad Magic Number (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int dimensionScan(int check, int height, int width, char *inputFilename, int MIN_DIMENSION, int MAX_DIMENSION){
    if (check != 2 || height < MIN_DIMENSION || width < MIN_DIMENSION || height > MAX_DIMENSION || width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}