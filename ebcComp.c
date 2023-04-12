#include "ebcCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
    {

    // validates that user has enter 2 arguments (plus the executable name)
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebcComp file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image1 = (Image*)malloc(sizeof(Image));

    char *inputFilename1 = argv[1];

    // opens the input file in read mode
    image1->associatedFile = fopen(argv[1], "rb");

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
    image1->magicNumber[0] = getc(image1->associatedFile);
    image1->magicNumber[1] = getc(image1->associatedFile);
    unsigned short *magicNumberValue1 = (unsigned short *)image1->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue1, inputFilename1,0x6365)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scans for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(image1->associatedFile, "%d %d", &image1->height, &image1->width);
    
    switch(dimensionScan(check, image1, inputFilename1)){
        case 0:
            fclose(image1->associatedFile);
            return BAD_DIM;
        default:
            break;            
    }

    image1->imageData = NULL;
    mallocTheArray(image1);


    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image1) == 0){
        fclose(image1->associatedFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        }

    switch(checkData(image1->associatedFile, image1, inputFilename1)){
        case 0:
            free(image1->imageData);
            fclose(image1->associatedFile);
            return BAD_DATA;
        case 1:
            free(image1->imageData);
            fclose(image1->associatedFile);
            return BAD_DATA;
        default:
            break;
    }

    // now we have finished using the image1->associatedFile we should close it
    fclose(image1->associatedFile);

     // create and initialise variables used within code
    Image *image2 = (Image*)malloc(sizeof(Image));
    char *inputFilename2 = argv[2];

    // open the input file in read mode
    image2->associatedFile = fopen(argv[2], "r");

    // check file opened successfully
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
    image2->magicNumber[0] = getc(image2->associatedFile);
    image2->magicNumber[1] = getc(image2->associatedFile);
    unsigned short *magicNumberValue2 = (unsigned short *)image2->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue2, inputFilename2,0x6365)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }
    
    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    check = fscanf(image2->associatedFile, "%d %d", &image2->height, &image2->width);
    
    switch(dimensionScan(check, image2, inputFilename2)){
        case 0:
            fclose(image2->associatedFile);
            return BAD_DIM;
        default:
            break;            
    }

    // caclulate total size and allocate memory for array
    image2->imageData = NULL;
    mallocTheArray(image2);

    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image2) == 0){
        fclose(image2->associatedFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        }

    switch(checkData(image2->associatedFile, image2, inputFilename2)){
        case 0:
            free(image2->imageData);
            fclose(image1->associatedFile);
            return BAD_DATA;
        case 1:
            free(image2->imageData);
            fclose(image1->associatedFile);
            return BAD_DATA;
        default:
            break;
    }

    // now we have finished using the image2->associatedFile we should close it
    fclose(image2->associatedFile);

    // compare the data from the two files:
    
    // start with magic number values
    if (*magicNumberValue1 != *magicNumberValue2){
            free(image1->imageData);
            free(image2->imageData);
            printf("DIFFERENT\n");
            return SUCCESS;
        }

    // check dimensions
    if ((image1->height != image2->height) || (image1->width != image2->width))
        { // free and exit
        free(image1->imageData);
        free(image2->imageData);
        printf("DIFFERENT\n");
        return SUCCESS;
        } // free and exit

    // and check the pixel values
     for (int currentRow = 0; currentRow < image1->height; currentRow++){
        for (int currentCol = 0; currentCol < image1->width; currentCol++){
            if (image1->imageData[currentRow][currentCol] != image2->imageData[currentRow][currentCol]){
                // free and exit
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