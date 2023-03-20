#include "ebfCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAGIC_NUMBER 0x6265


int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename){
    if (*magicNumberValue != MAGIC_NUMBER){
            printf("ERROR: Bad Magic Number (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int dimensionScan(int check, Image *img, char *inputFilename){
    if (check != 2 || img->height < MIN_DIMENSION || img->width < MIN_DIMENSION || img->height > MAX_DIMENSION || img->width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}


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

void mallocTheArray(Image *img){
    img->imageData = (uint8_t **)malloc(img->height * sizeof(uint8_t*));
    img->dataBlock = (uint8_t*)malloc(img->height*img->width*sizeof(uint8_t));
    for (int i = 0; i < img->height; i = i + 1){
        img->imageData[i] = img->dataBlock + i * img->width;
    }
}

int isBadMalloc(Image *img){
    if (img->dataBlock == NULL){
        return 0;
    }
    return 1;
}