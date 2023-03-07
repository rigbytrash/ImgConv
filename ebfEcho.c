#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commonFunc.h"

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


int main(int argc, char **argv)
    { // main
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebfEcho file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    // create a char array to hold magic number
    // and cast to short (later in code)
    unsigned char magicNumber[2];

    // create and initialise variables used within code
    int width = 0, height = 0;
    unsigned int *imageData;
    long numBytes;
    char *inputFilename = argv[1];
    char *outputFilename = argv[2];

    // open the input file in read mode
    FILE *inputFile = fopen(inputFilename, "r");

    // check file opened successfully
    switch(checkReadFileAccess(inputFilename)){
        case 0:
            printf("ERROR: Bad File Name (1)\n");
            return BAD_FILE;
        case 1:
            return BAD_FILE;
        default:
            break;
    }

    // get first 2 characters which should be magic number
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue, inputFilename)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &height, &width);

    switch(dimensionScan(check,height,width,inputFilename)){
        case 0:
            fclose(inputFile);
            return BAD_DIM;
        default:
            break;            
    }

    // caclulate total size and allocate memory for array
    numBytes = height * width;
    imageData = (unsigned int *)malloc(numBytes * sizeof(unsigned int));


    // if malloc is unsuccessful, it will return a null pointer
    if (imageData == NULL)
        { // check malloc
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    switch(checkData(inputFile,numBytes,imageData, inputFilename)){
        case 0:
            free(imageData);
            fclose(inputFile);
            return BAD_DATA;
        case 1:
            free(imageData);
            fclose(inputFile);
            return BAD_DATA;
        default:
            break;
    }


    // now we have finished using the inputFile we should close it
    fclose(inputFile);

    // open the output file in write mode
    FILE *outputFile = fopen(outputFilename, "w");
    // validate that the file has been opened correctly
    
    if (access(outputFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",outputFilename);
        free(imageData);
        return BAD_OUTPUT;
    }


    // write the header data in one block
    check = fprintf(outputFile, "eb\n%d %d\n", height, width);
    // and use the return from fprintf to check that we wrote.
    if (check == 0) 
        { // check write
        fclose(outputFile);
        free(imageData);
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
        } // check write

    // iterate though the array and print out pixel values
    for (int current = 0; current < numBytes; current++)
        { // writing out
        // if we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
        check = fprintf(inputFile, "%u%c", imageData[current], ((current + 1) % width) ? ' ' : '\n');
        if (check == 0)
            { // check write
            fclose(outputFile);
            free(imageData);
            printf("ERROR: Bad Output\n");
            return BAD_OUTPUT;
            } // check write
        } // writing out

    // free allocated memory before exit
    free(imageData);
    // close the output file before exit
    fclose(outputFile);

    // print final success message and return
    printf("ECHOED\n");
    return SUCCESS;
    } // main()