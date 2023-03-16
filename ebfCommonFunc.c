#include "ebfCommonFunc.h"
#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAX_DATA 31
#define MIN_DATA 0

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename){
    if (*magicNumberValue != MAGIC_NUMBER){
            printf("ERROR: Bad Magic Number (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int dimensionScan(int check, ebfImage *img, char *inputFilename){
    if (check != 2 || img->height < MIN_DIMENSION || img->width < MIN_DIMENSION || img->height > MAX_DIMENSION || img->width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}


int checkData(FILE *inputFile, ebfImage *img, char *inputFilename){
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
        
        int check2 = fscanf(inputFile, "%u", &img->imageData[currentRow][currentCol]);
        if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA)
            { // check inputted data
            printf("ERROR: Bad Data (%s)\n", inputFilename);
            return 0;
            }
        }
    }
    unsigned int tmp; // checking too much
        int check2 = fscanf(inputFile, "%u", &tmp);
        if (check2 != 0 && check2 != -1){
            printf("ERROR: Bad Data (%s)\n", inputFilename);
            return 1;
        }
    return 2;
}

void mallocTheArray(ebfImage *img){
    img->imageData = (unsigned int **)malloc(img->height * sizeof(unsigned int*));
    img->dataBlock = (unsigned int*)malloc(img->height*img->width*sizeof(unsigned int));
    for (int i = 0; i < img->height; i = i + 1){
        img->imageData[i] = img->dataBlock + i * img->width;
    }
}

int isBadMalloc(ebfImage *img){
    if (img->dataBlock == NULL){
        return 0;
    }
    return 1;
}

