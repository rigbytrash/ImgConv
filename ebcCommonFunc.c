#include "ebcCommonFunc.h"
#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6365
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

int dimensionScan(int check, Image *img, char *inputFilename){
    if (check != 2 || img->height < MIN_DIMENSION || img->width < MIN_DIMENSION || img->height > MAX_DIMENSION || img->width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", inputFilename);
        return 0;
    }
    return 1;
}

int checkData(FILE *inputFile, Image *img, char *inputFilename){
    uint8_t ** imageDataForEbc; // creates a new 2d array that is the correct size for the compressed data
    uint8_t * dataBlockForEbc;
    imageDataForEbc = (uint8_t **)malloc(img->height *(5.0f/8.0f)* sizeof(uint8_t*)); // REMEMBER TO FREE!
    dataBlockForEbc = (uint8_t*)malloc(img->height*img->width*sizeof(uint8_t));
    for (int i = 0; i < img->height*(5.0f/8.0f); i = i + 1){
        imageDataForEbc[i] = dataBlockForEbc + i * img->width;
    }
    fread(&imageDataForEbc[0][0], sizeof(uint8_t), 1, inputFile); // skip a line
    for (int currentRow = 0; currentRow < (img->height*(5.0f/8.0f)); currentRow++){ // read in the compressed data to the new array
        for (int currentCol = 0; currentCol < (img->width); currentCol++){
            int check2 = fread(&imageDataForEbc[currentRow][currentCol], sizeof(uint8_t), 1, inputFile);
            if (check2 != 1){ // check inputted data
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
                                // convert from 5 bit to 8 bit and write to the img->imageData
    int currentWriteRow = 0;
    int currentWriteCol = 0;
    int counter = 0;
    unsigned char mask1 = (unsigned char) 0x80;
    uint8_t toOutput;
    uint8_t writeToOutput;
    // uint8_t toSaveToOutput;
    uint8_t current;
    for (int currentRow = 0; currentRow < (img->height*(5.0f/8.0f)); currentRow++){ // read in the compressed data to the new array
        for (int currentCol = 0; currentCol < ((img->width)); currentCol++){
            current = imageDataForEbc[currentRow][currentCol];
            for (int i = 0; i < 8; i++){
                // place current[i] in output[counter]
                writeToOutput = 0;
                writeToOutput = (current << i) & mask1;
                toOutput = toOutput | (writeToOutput>>counter);
                counter++;

                //if output is full, print and set counter to 0
                if (counter == 5){
                    counter = 0;
                    img->imageData[currentWriteRow][currentWriteCol] = toOutput >> 3;
                    if (img->imageData[currentWriteRow][currentWriteCol] > 31){
                        printf("%d\n", img->imageData[currentWriteRow][currentWriteCol]);
                    }
                    if (img->imageData[currentWriteRow][currentWriteCol] > MAX_DATA || img->imageData[currentWriteRow][currentWriteCol] < MIN_DATA){ // check inputted data
                        printf("ERROR: Bad Data (%s)\n", inputFilename);
                        return 0;
                    }
                    toOutput = 0;

                    if(currentWriteCol < img->width){
                        currentWriteCol++;
                    }
                    if(currentWriteCol == img->width){
                        currentWriteCol = 0;
                        currentWriteRow++;
                    }
                    
                }
            }
        }
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