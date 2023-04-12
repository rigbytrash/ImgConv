#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv)
    {
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebu2ebc file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image = (Image*)malloc(sizeof(Image));


    // creates and initialise variables used within code
    char *inputFilename = argv[1];
    char *outputFilename = argv[2];

    // opens the input file in read mode
    image->associatedFile = fopen(inputFilename, "rb");

    // check file opened successfully
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
    switch(checkMagicNumber(magicNumberValue, inputFilename,0x7565)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }

    // scans for the dimensions
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

    switch(checkData(image->associatedFile, image, inputFilename)){
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


    // file no longer in use
    fclose(image->associatedFile);

    // opens the output file in write mode
    image->associatedFile = fopen(outputFilename, "wb");
    
    switch(printEBC(image, outputFilename, check)){ // does file validation
        case 0:
            free(image->imageData);
            return BAD_OUTPUT;
            break;
        case 1:
            fclose(image->associatedFile);
            free(image->imageData);
            return BAD_OUTPUT;
        default:
            break;
    }  
    

    // frees allocated memory before exit
    free(image->imageData);
    // close the output file before exit
    fclose(image->associatedFile);

    // prints final success message and return
    printf("CONVERTED\n");
    return SUCCESS;
    }