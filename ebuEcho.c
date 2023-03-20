#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
    {
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebuEcho file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image = (Image*)malloc(sizeof(Image));

    // create sand initialise variables used within code
    char *inputFilename = argv[1];
    char *outputFilename = argv[2];

    // opens the input file in read mode
    FILE *inputFile = fopen(inputFilename, "rb");

    // checks file opened successfully
    switch(checkReadFileAccess(inputFilename)){
        case 0:
            printf("ERROR: Bad File Name (1)\n");
            return BAD_FILE;
        case 1:
            return BAD_FILE;
        default:
            break;
    }

    // gets first 2 characters which should be magic number
    image->magicNumber[0] = getc(inputFile);
    image->magicNumber[1] = getc(inputFile);
    unsigned short *magicNumberValue = (unsigned short *)image->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue, inputFilename)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }

    // scans for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &image->height, &image->width);

    switch(dimensionScan(check, image, inputFilename)){
        case 0:
            fclose(inputFile);
            return BAD_DIM;
        default:
            break;            
    }

    image->imageData = NULL;
    mallocTheArray(image);

    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image) == 0)
        { // check malloc
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    switch(checkData(inputFile, image, inputFilename)){ // validates file perms
        case 0:
            free(image->imageData);
            fclose(inputFile);
            return BAD_DATA;
        case 1:
            free(image->imageData);
            fclose(inputFile);
            return BAD_DATA;
        default:
            break;
    }

    // no longer in use
    fclose(inputFile);

    // opens the output file in write mode
    FILE *outputFile = fopen(outputFilename, "wb");
    // validates that the file has been opened correctly
    
    switch(printEBU(image, outputFile, outputFilename, check)){ // validates file perms too
        case 0:
            free(image->imageData);
            return BAD_OUTPUT;
            break;
        case 1:
            fclose(outputFile);
            free(image->imageData);
            printf("ERROR: Bad Output\n");
            return BAD_OUTPUT;
            break;
        default:
            break;
    }

    // frees allocated memory before exit
    free(image->imageData);
    // closes the output file before exit
    fclose(outputFile);

    // prints final success message and return
    printf("ECHOED\n");
    return SUCCESS;
    }