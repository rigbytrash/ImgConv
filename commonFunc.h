#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAX_DATA 31
#define MIN_DATA 0

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

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename){
    if (*magicNumberValue != MAGIC_NUMBER){
            printf("ERROR: Bad Magic Number (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int dimensionScan(int check, int height, int width, char *inputFilename){
    if (check != 2 || height < MIN_DIMENSION || width < MIN_DIMENSION || height > MAX_DIMENSION || width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int checkData(FILE *inputFile, int numBytes, unsigned int *imageData, char *inputFilename){
    for (int current = 0; current < numBytes; current++)
        { // reading in
        int check2 = fscanf(inputFile, "%u", &imageData[current]);
        // validate that we have captured 1 pixel value and the value falls within the CORRECT range
        if (check2 !=1 || imageData[current] > MAX_DATA || imageData[current] < MIN_DATA)
            { // check inputted data
            // ensure that allocated data is freed before exit.
            printf("ERROR: Bad Data (%s)\n", inputFilename);
            return 0;
            } // check inputted data
        } // reading in
        unsigned int tmp; // checking too much
        int check2 = fscanf(inputFile, "%u", &tmp);
        if (check2 != 0 && check2 != -1){
            printf("ERROR: Bad Data (%s)\n", inputFilename);
            return 1;
        }
        return 2;
}