#include "ebfCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
    {

    // validates that user has enter 2 arguments (plus the executable name)
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebfComp file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image1 = (Image*)malloc(sizeof(Image));

    char *inputFilename1 = argv[1];

    // opens the input file in read mode
    FILE *inputFile1 = fopen(argv[1], "r");

    // checks file opened successfully
    switch(checkReadFileAccess(inputFilename1)){
        case 0:
            printf("ERROR: Bad File Name (1)\n");
            return BAD_FILE;
        case 1:
            return BAD_FILE;
        default:
            break;
    }

    // gets first 2 characters which should be magic number
    image1->magicNumber[0] = getc(inputFile1);
    image1->magicNumber[1] = getc(inputFile1);
    unsigned short *magicNumberValue1 = (unsigned short *)image1->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue1, inputFilename1,0x6265)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scans for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile1, "%d %d", &image1->height, &image1->width);
    
    switch(dimensionScan(check, image1, inputFilename1)){
        case 0:
            fclose(inputFile1);
            return BAD_DIM;
        default:
            break;            
    }

    image1->imageData = NULL;
    mallocTheArray(image1);

    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image1) == 0){
        fclose(inputFile1);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        }

    switch(checkData(inputFile1, image1, inputFilename1)){
        case 0:
            free(image1->imageData);
            fclose(inputFile1);
            return BAD_DATA;
        case 1:
            free(image1->imageData);
            fclose(inputFile1);
            return BAD_DATA;
        default:
            break;
    }

    // file no longer in use
    fclose(inputFile1);


    // creates and initialise variables used within code
    Image *image2 = (Image*)malloc(sizeof(Image));
    char *inputFilename2 = argv[2];

    // opens the input file in read mode
    FILE *inputFile2 = fopen(argv[2], "r");

    // checks file opened successfully
    switch(checkReadFileAccess(inputFilename2)){
        case 0:
            printf("ERROR: Bad File Name (2)\n");
            return BAD_FILE;
        case 1:
            return BAD_FILE;
        default:
            break;
    }


    // get first 2 characters which should be magic number
    image2->magicNumber[0] = getc(inputFile2);
    image2->magicNumber[1] = getc(inputFile2);
    unsigned short *magicNumberValue2 = (unsigned short *)image2->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue2, inputFilename2,0x6265)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    check = fscanf(inputFile2, "%d %d", &image2->height, &image2->width);
    
    switch(dimensionScan(check, image2, inputFilename2)){
        case 0:
            fclose(inputFile2);
            return BAD_DIM;
        default:
            break;            
    }

    image2->imageData = NULL;
    mallocTheArray(image2);

    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image2) == 0){
            fclose(inputFile2);
            printf("ERROR: Image Malloc Failed\n");
            return BAD_MALLOC;
        }

    switch(checkData(inputFile2, image2, inputFilename2)){
        case 0:
            free(image2->imageData);
            fclose(inputFile1);
            return BAD_DATA;
        case 1:
            free(image2->imageData);
            fclose(inputFile1);
            return BAD_DATA;
        default:
            break;
    }

    // file no longer in use
    fclose(inputFile2);

    // compare the data from the two files:
    
    // start with magic number values
    if (*magicNumberValue1 != *magicNumberValue2){
        free(image1->imageData);
        free(image2->imageData);
        printf("DIFFERENT\n");
        return SUCCESS;
        }

    // check dimensions
    if ((image1->height != image2->height) || (image1->width != image2->width)){
            free(image1->imageData);
            free(image2->imageData);
            printf("DIFFERENT\n");
            return SUCCESS;
        }

    // and check the pixel values
     for (int currentRow = 0; currentRow < image1->height; currentRow++){
        for (int currentCol = 0; currentCol < image1->width; currentCol++){
            if (image1->imageData[currentRow][currentCol] != image2->imageData[currentRow][currentCol]){
                    free(image1->imageData);
                    free(image2->imageData);
                    printf("DIFFERENT\n");
                    return SUCCESS;
            }
        }
     }
    // free allocated memory before exit
    free(image1->imageData);
    free(image2->imageData);

    // if we have not exited on different data, must be identical
    printf("IDENTICAL\n");
    return SUCCESS;
    }