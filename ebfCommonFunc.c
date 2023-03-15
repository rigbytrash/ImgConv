#include "ebfCommonFunc.h"
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


int checkData(FILE *inputFile, int numBytes, unsigned int **imageData, char *inputFilename, int rows, int cols){
    for (int currentRow = 0; currentRow < rows; currentRow++){
        for (int currentCol = 0; currentCol < cols; currentCol++){
        
        int check2 = fscanf(inputFile, "%u", &imageData[currentRow][currentCol]);
        if (check2 !=1 || imageData[currentRow][currentCol] > MAX_DATA || imageData[currentRow][currentCol] < MIN_DATA)
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