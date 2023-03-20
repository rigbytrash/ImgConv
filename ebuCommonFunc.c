#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define MAGIC_NUMBER 0x7565

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
    fread(&img->imageData[0][0], sizeof(uint8_t), 1, inputFile); // skips a line
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            int check2 = fread(&img->imageData[currentRow][currentCol], sizeof(uint8_t), 1, inputFile);
            if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA){ // check inputted data
                    printf("ERROR: Bad Data (%s)\n", inputFilename);
                    return 0;
                }
        }
    }

    unsigned char *tmp; // checking too much
        int check2 = fread(&tmp, sizeof(uint8_t) - 1, 1, inputFile);
        if (check2 != 0){
            printf("ERROR: Bad Data(%s)\n", inputFilename);
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