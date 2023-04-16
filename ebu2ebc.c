#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    switch(checkargs(argc)){
        case 0: // when no args are given
            printf("Usage: ebu2ebc file1 file2\n");
            return SUCCESS;
        case 1: // when args are given but are incorrect
            return BAD_ARGS;
        default:
            break;
    }

    Image *image = newImage(); // create struct to store data
    image->err = readInFile(image, "ebu", argv[1]); // read in the data + perform checks; 0 idicates success
    if (image->err != 0){
        return image->err;
    }

    image->AFilename = argv[2];
    image->associatedFile = fopen(image->AFilename, "wb"); // ebc requires binary manipulation
    image->err = printEBC(image, argv[2]); // prints the data in the specified format
    if (image->err != 0){
        return image->err;
    }

    disposeImage(image); // free the memory
    printf("CONVERTED\n");
    return SUCCESS;
}