#include "ebfCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
    {
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebfEcho file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image = (Image*)malloc(sizeof(Image));

    char *inputFilename = argv[1];
    char *outputFilename = argv[2];


    // opens the input file in read mode
    image->associatedFile = fopen(inputFilename, "r");

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
    image->magicNumber[0] = getc(image->associatedFile);
    image->magicNumber[1] = getc(image->associatedFile);
    unsigned short *magicNumberValue = (unsigned short *)image->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue, inputFilename,0x6265)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(image->associatedFile, "%d %d", &image->height, &image->width);

    switch(dimensionScan(check, image, inputFilename)){
        case 0:
            fclose(image->associatedFile);
            return BAD_DIM;
        default:
            break;            
    }

    image->imageData = NULL;
    mallocTheArray(image);

    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image) == 0){
            fclose(image->associatedFile);
            printf("ERROR: Image Malloc Failed\n");
            return BAD_MALLOC;
        }

    switch(checkData(image, inputFilename)){ // validates file perms
        case 0:
            free(image->imageData);
            fclose(image->associatedFile);
            return BAD_DATA;
        case 1:
            free(image->imageData);
            fclose(image->associatedFile);
            return BAD_DATA;
        default:
            break;
    }

    // file no longer used 
    fclose(image->associatedFile);

    // opens the output file in write mode
    image->associatedFile = fopen(outputFilename, "w");
    // validate that the file has been opened correctly


    switch(printEBF(image, outputFilename, check)){
        case 0:
            free(image->imageData);
            return BAD_OUTPUT;
            break;
        default:
            break;
    }  

    // frees allocated memory before exit
    free(image->imageData);
    // closes the output file before exit
    fclose(image->associatedFile);

    // prints final success message and return
    printf("ECHOED\n");
    return SUCCESS;
    }