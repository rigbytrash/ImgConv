#include "ebfCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAGIC_NUMBER 0x6265

int checkData(FILE *inputFile, Image *img, char *inputFilename){
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
        int check2 = fscanf(inputFile, "%hhu", &img->imageData[currentRow][currentCol]);
        if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA)
            { // check inputted data
            printf("ERROR: Bad Data (%s)\n", inputFilename);
            return 0;
            }
        }
    }

    uint8_t tmp; // checking too much
    int check2 = fscanf(inputFile, "%hhu", &tmp);
    if (check2 != 0 && check2 != -1){
        printf("ERROR: Bad Data (%s)\n", inputFilename);
        return 1;
    }
    return 2;
}