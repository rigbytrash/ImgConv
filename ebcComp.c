#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    int err = checkargs(argc, argv[0]); // checks to see if the args exist + are correct
    if (err != 99){
        return err;
    }

    Image *image1 = newImage(); // create struct to store data
    image1->err = readInFile(image1, "ebc", argv[1]); // read in the data + perform checks; 0 idicates success
    if (image1->err != 0){
        return image1->err;
    }

    Image *image2 = newImage(); // create struct to store data
    image2->err = readInFile(image2, "ebc", argv[2]); // read in the data + perform checks; 0 idicates success
    if (image2->err != 0){
        return image2->err;
    }

    switch(compare(image1, image2)){ // compares the image, gives the answer and frees (inshallah)
        case 0:
            disposeImage(image1);
            disposeImage(image2);
            return SUCCESS;
        default:
            printf("IDENTICAL\n");
            break;
    }
    
    disposeImage(image1);
    disposeImage(image2);
    return SUCCESS;
}