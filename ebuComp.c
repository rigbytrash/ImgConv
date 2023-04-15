#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
    {
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebuComp file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image1 = (Image*)malloc(sizeof(Image));

    switch(readInFile(image1, "ebu", argv[1])){
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

    Image *image2 = (Image*)malloc(sizeof(Image));

    switch(readInFile(image2, "ebu", argv[2])){
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

    switch(compare(image1, image2)){
        case 0:
            return SUCCESS;
        default:
            break;
    }
    // free allocated memory before exit
    free(image1->imageData);
    free(image2->imageData);

    // if we have not exited on different data, must be identical
    printf("IDENTICAL\n");
    return SUCCESS;
    }