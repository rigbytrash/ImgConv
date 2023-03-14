#include <stdio.h>
#include <stdlib.h>
#include "ebuCommonFunc.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x7565
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAX_DATA 31
#define MIN_DATA 0

int main(int argc, char **argv)
    { // main

    // validate that user has enter 2 arguments (plus the executable name)
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebuComp file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber1[2];

    // create and initialise variables used within code
    int width1 = 0, height1 = 0;
    unsigned char **imageData1 = NULL;
    long long numBytes1;
    char *inputFilename1 = argv[1];

    // open the input file in read mode
    FILE *inputFile1 = fopen(argv[1], "rb");

    // check file opened successfully
    switch(checkReadFileAccess(inputFilename1)){
        case 0:
            printf("ERROR: Bad File Name (1)\n");
            return BAD_FILE;
        case 1:
            return BAD_FILE;
        default:
            break;
    }

    // get first 2 characters which should be magic number
    magicNumber1[0] = getc(inputFile1);
    magicNumber1[1] = getc(inputFile1);
    unsigned short *magicNumberValue1 = (unsigned short *)magicNumber1;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue1, inputFilename1)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile1, "%d %d", &height1, &width1);
    
    switch(dimensionScan(check,height1,width1,inputFilename1)){
        case 0:
            fclose(inputFile1);
            return BAD_DIM;
        default:
            break;            
    }

    // caclulate total size and allocate memory for array
    numBytes1 = (long long) height1 * width1;
        if (numBytes1 <= MAX_DIMENSION && numBytes1 > MIN_DIMENSION){
        imageData1 = (unsigned char **)malloc(numBytes1 * sizeof(unsigned char*));
        for (int i = 0; i < height1; i = i + 1){
            imageData1[i] = (unsigned char*)malloc(numBytes1 * sizeof(unsigned char));
        }
    }

    // if malloc is unsuccessful, it will return a null pointer
    if (imageData1 == NULL)
        { // check malloc
        fclose(inputFile1);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    switch(checkData(inputFile1,numBytes1,imageData1,inputFilename1,height1,width1)){
        case 0:
            free(imageData1);
            fclose(inputFile1);
            return BAD_DATA;
        case 1:
            free(imageData1);
            fclose(inputFile1);
            return BAD_DATA;
        default:
            break;
    }

    // now we have finished using the inputFile1 we should close it
    fclose(inputFile1);

     // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber2[2];

    // create and initialise variables used within code
    int width2 = 0, height2 = 0;
    unsigned char **imageData2 = NULL;
    long long numBytes2;
    char *inputFilename2 = argv[2];

    // open the input file in read mode
    FILE *inputFile2 = fopen(argv[2], "r");

    // check file opened successfully
    switch(checkReadFileAccess(inputFilename1)){
        case 0:
            printf("ERROR: Bad File Name (2)\n");
            return BAD_FILE;
        case 1:
            printf("ERROR: Bad File Name (%s)\n",inputFilename2);
            return BAD_FILE;
        default:
            break;
    }


    // get first 2 characters which should be magic number
    magicNumber2[0] = getc(inputFile2);
    magicNumber2[1] = getc(inputFile2);
    unsigned short *magicNumberValue2 = (unsigned short *)magicNumber2;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue2,inputFilename2)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    check = fscanf(inputFile2, "%d %d", &height2, &width2);
    
    switch(dimensionScan(check,height2,width2,inputFilename2)){
        case 0:
            fclose(inputFile2);
            return BAD_DIM;
        default:
            break;            
    }

    // caclulate total size and allocate memory for array
    numBytes2 = (long long) height2 * width2;
        if (numBytes2 <= MAX_DIMENSION && numBytes2 > MIN_DIMENSION){
        imageData2 = (unsigned char **)malloc(numBytes2 * sizeof(unsigned char*));
        for (int i = 0; i < height2; i = i + 1){
            imageData2[i] = (unsigned char*)malloc(numBytes2 * sizeof(unsigned char));
        }
    }

    // if malloc is unsuccessful, it will return a null pointer
    if (imageData2 == NULL)
        { // check malloc
        fclose(inputFile2);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    switch(checkData(inputFile2,numBytes2,imageData2,inputFilename2,height2,width2)){
        case 0:
            free(imageData2);
            fclose(inputFile2);
            return BAD_DATA;
        case 1:
            free(imageData2);
            fclose(inputFile2);
            return BAD_DATA;
        default:
            break;
    }

    // now we have finished using the inputFile1 we should close it
    fclose(inputFile2);

    // compare the data from the two files:
    
    // start with magic number values
    if (*magicNumberValue1 != *magicNumberValue2)
        { // free and exit
        free(imageData1);
        free(imageData2);
        printf("DIFFERENT\n");
        return SUCCESS;
        } // free and exit

    // check dimensions
    if ((height1 != height2) || (width1 != width2))
        { // free and exit
        free(imageData1);
        free(imageData2);
        printf("DIFFERENT\n");
        return SUCCESS;
        } // free and exit

    // and check the pixel values
     for (int currentRow = 0; currentRow < height1; currentRow++){
        for (int currentCol = 0; currentCol < width1; currentCol++){
            if (imageData1[currentRow][currentCol] != imageData2[currentRow][currentCol]){
                // free and exit
                free(imageData1);
                free(imageData2);
                printf("DIFFERENT\n");
                return SUCCESS;
            }
        }
     }
    // free allocated memory before exit
    free(imageData1);
    free(imageData2);

    // if we have not exited on different data, must be identical
    printf("IDENTICAL\n");
    return SUCCESS;
    } // main()