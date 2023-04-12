#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int checkargs(int argCount){
    if (argCount != 3) // there should be exe name, input filename and output filename
        {
            if(argCount == 1){
                return 0;
            }
            printf("ERROR: Bad Arguments\n");
            return 1;
        }
        
    return 2;
}

int checkReadFileAccess(char *filename){
    if (access(filename,F_OK) == -1){
        return 0;
    }
    if (access(filename,R_OK) == -1){
        printf("ERROR: Bad File Name (%s)\n", filename);
        return 1;
    }

    return 2;
}

int checkMagicNumber(unsigned short *magicNumberValue, char *inputFilename, int MAGIC_NUMBER){
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

int printEBU(Image *img, char* outputFilename, int check){
// writes EBU files, prior to this, the data should already be read in and converted
    if (access(outputFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",outputFilename);
        return 0;
    }

    // write the header data in one block
    check = fprintf(img->associatedFile, "eu\n%d %d\n", img->height, img->width);

    if (check == 0){
            printf("ERROR: Bad Output\n");
            return 1;
        }

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            check = fwrite(&img->imageData[currentRow][currentCol],sizeof(unsigned char),1,img->associatedFile); 

            if (check == 0)
            { 
                printf("ERROR: Bad Output\n");
                return 1;
            }
        }
    }    

    return 2;
}

int printEBF(Image *img, char* outputFilename, int check){
// writes EBF files, prior to this, the data should already be read in and converted

    if (access(outputFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",outputFilename);
        return 0;
    }

    // write the header data in one block
    check = fprintf(img->associatedFile, "eb\n%d %d\n", img->height, img->width);

    if (check == 0) 
        { // check write
            printf("ERROR: Bad Output\n");
            return 0;
        } // check write

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            check = fprintf(img->associatedFile, "%u%c", img->imageData[currentRow][currentCol], (currentCol != img->width - 1) ? ' ' : '\n');

            if (check == 0)
            { // check write
                printf("ERROR: Bad Output\n");
                return 0;
            }
        }
    }  

    return 2;
}

int printEBC(Image *image, char* outputFilename, int check){

   if (access(outputFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",outputFilename);
        return 0;
    }


    // write the header data in one block
    check = fprintf(image->associatedFile, "ec\n%d %d\n", image->height, image->width);

    if (check == 0){
        printf("ERROR: Bad Output\n");
        return 1;
        }

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
                        check = fwrite(&toOutput,sizeof(unsigned char),1,image->associatedFile); 
                        if (check == 0){
                            printf("ERROR: Bad Output\n");
                            return 1;
                        }

                        toOutput = 0;
                    }

                    current <<= 1;
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