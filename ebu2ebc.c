#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
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
    switch(readInFile(image, "ebu", argv[1])){
        case 1:
            return BAD_FILE;
        case 2:
            return BAD_MAGIC_NUMBER;
        case 3:
            return BAD_DIM;
        case 4:
            return BAD_MALLOC;
        case 5:
            return BAD_DATA;
        default:
            break;
    }
    

    image->AFilename = argv[2];
    image->associatedFile = fopen(image->AFilename, "wb");
    // validate that the file has been opened correctly
    switch(printEBC(image, argv[2])){
        case 0:
            free(image->imageData);
            return BAD_OUTPUT;
            break;
        default:
            break;
    }  

    // frees allocated memory before exit
    free(image->imageData);
    // prints final success message and return
    printf("CONVERTED\n");
    return SUCCESS;
}