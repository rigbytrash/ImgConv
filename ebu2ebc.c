#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    int err = checkargs(argc, argv[0]); // checks to see if the args exist + are correct
    if (err != 99){
        return err;
    }

    Image *image = newImage(); // create struct to store data
    image->err = readInFile(image, "ebu", argv[1]); // read in the data + perform checks; 0 idicates success
    if (image->err != 0){
        return image->err;
    }

    image->err = printEBC(image, argv[2]); // prints the data in the specified format
    if (image->err != 0){
        return image->err;
    }

    disposeImage(image); // free the memory
    printf("CONVERTED\n");
    return SUCCESS;
}