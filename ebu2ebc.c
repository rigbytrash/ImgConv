#include "allCommonFunc.h"
#include "ebuCommonFunc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv)
    { // main
    switch(checkargs(argc)){
        case 0:
            printf("Usage: ebu2ebc file1 file2\n");
            return SUCCESS;
        case 1:
            return BAD_ARGS;
        default:
            break;
    }

    Image *image = (Image*)malloc(sizeof(Image));


    // create and initialise variables used within code
    char *inputFilename = argv[1];
    char *outputFilename = argv[2];

    // open the input file in read mode
    FILE *inputFile = fopen(inputFilename, "rb");

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
    image->magicNumber[0] = getc(inputFile);
    image->magicNumber[1] = getc(inputFile);
    unsigned short *magicNumberValue = (unsigned short *)image->magicNumber;

    // checking against the casted value due to endienness.
    switch(checkMagicNumber(magicNumberValue, inputFilename)){
        case 0:
            return BAD_MAGIC_NUMBER;
        default:
            break;
    }

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &image->height, &image->width);

    switch(dimensionScan(check, image, inputFilename)){
        case 0:
            fclose(inputFile);
            return BAD_DIM;
        default:
            break;            
    }

    // caclulate total size and allocate memory for array
    image->imageData = NULL;
    mallocTheArray(image);



    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(image) == 0)
        { // check malloc
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    switch(checkData(inputFile, image, inputFilename)){
        case 0:
            free(image->imageData);
            fclose(inputFile);
            return BAD_DATA;
        case 1:
            free(image->imageData);
            fclose(inputFile);
            return BAD_DATA;
        default:
            break;
    }


    // now we have finished using the inputFile we should close it
    fclose(inputFile);

    // open the output file in write mode
    FILE *outputFile = fopen(outputFilename, "wb");
    // validate that the file has been opened correctly
    
    if (access(outputFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",outputFilename);
        free(image->imageData);
        return BAD_OUTPUT;
    }


    // write the header data in one block
    check = fprintf(outputFile, "ec\n%d %d\n", image->height, image->width);
    // and use the return from fprintf to check that we wrote.
    if (check == 0) 
        { // check write
        fclose(outputFile);
        free(image->imageData);
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
        } // check write

        int counter = 0;
        unsigned char mask1 = (unsigned char) 0x80;
        uint8_t toOutput;
        uint8_t toSaveToOutput;
        uint8_t current;

        for (int currentRow = 0; currentRow < image->height; currentRow++){
            for (int currentCol = 0; currentCol < image->width; currentCol++){
                current = image->imageData[currentRow][currentCol]<< 3; //shifts the 3 zeros to the end

                for (int i = 0; i < 5; i ++){
                    toSaveToOutput = current & mask1; //applys the mask to only get the top bit
                    toSaveToOutput = (toSaveToOutput >> counter); //moves the bit to the correct place to be pushed
                    toOutput |= toSaveToOutput; //places the bit in the output buffer
                    counter = counter + 1;

                    if (counter == 8){ // writes the output to the file when the buffer is full, then resets buffer
                        counter = 0;
                        check = fwrite(&toOutput,sizeof(unsigned char),1,outputFile); 
                        if (check == 0)
                        { // check write
                            fclose(outputFile);
                            free(image->imageData);
                            printf("ERROR: Bad Output\n");
                            return BAD_OUTPUT;
                        }
                        toOutput = 0;
                    }
                    current <<= 1;
                }
            }
        }
    

    // free allocated memory before exit
    free(image->imageData);
    // close the output file before exit
    fclose(outputFile);

    // print final success message and return
    printf("CONVERTED\n");
    return SUCCESS;
    } // main()