#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


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

int checkReadFileAccess(Image *img){
    if (access(img->AFilename,F_OK) == -1){
        return 0;
    }
    if (access(img->AFilename,R_OK) == -1){
        printf("ERROR: Bad File Name (%s)\n", img->AFilename);
        return 1;
    }

    return 2;
}

int checkMagicNumber(Image *img, int MAGIC_NUMBER){
    // gets first 2 characters which should be magic number
    img->magicNumber[0] = getc(img->associatedFile);
    img->magicNumber[1] = getc(img->associatedFile); 
    img->magicNumberValue  = (unsigned short *)img->magicNumber;
    if (*img->magicNumberValue != MAGIC_NUMBER){
        // printf("%u + %u ",img->magicNumber[0],img->magicNumber[1]);
        // printf("%u vs %d ",*img->magicNumberValue, MAGIC_NUMBER);
        printf("ERROR: Bad Magic Number (%s)\n", img->AFilename);
        return 0;
    }

    return 1;
}

int dimensionScan(Image *img){
    img->check = fscanf(img->associatedFile, "%d %d", &img->height, &img->width);
    if (img->check != 2 || img->height < MIN_DIMENSION || img->width < MIN_DIMENSION || img->height > MAX_DIMENSION || img->width > MAX_DIMENSION)
    { 
        printf("ERROR: Bad Dimensions (%s)\n", img->AFilename);
        return 0;
    }

    return 1;
}

int printEBU(Image *img, char *nFilename){
    setFileW(img, nFilename);
// writes EBU files, prior to this, the data should already be read in and converted
    if (access(img->AFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        return 0;
    }

    // write the header data in one block
    img->check = fprintf(img->associatedFile, "eu\n%d %d\n", img->height, img->width);

    if (img->check == 0){
            printf("ERROR: Bad Output\n");
            return 0;
        }

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            img->check = fwrite(&img->imageData[currentRow][currentCol],sizeof(unsigned char),1,img->associatedFile); 

            if (img->check == 0)
            { 
                printf("ERROR: Bad Output\n");
                return 0;
            }
        }
    }    
    fclose(img->associatedFile);
    return 2;
}

int printEBF(Image *img, char *nFilename){
// writes EBF files, prior to this, the data should already be read in and converted
    setFileW(img, nFilename);

    if (access(img->AFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        return 0;
    }

    // write the header data in one block
    img->check = fprintf(img->associatedFile, "eb\n%d %d\n", img->height, img->width);

    if (img->check == 0) 
        { // img->check write
            printf("ERROR: Bad Output\n");
            return 0;
        } // img->check write

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            img->check = fprintf(img->associatedFile, "%u%c", img->imageData[currentRow][currentCol], (currentCol != img->width - 1) ? ' ' : '\n');

            if (img->check == 0)
            { // img->check write
                printf("ERROR: Bad Output\n");
                return 0;
            }
        }
    }  
    fclose(img->associatedFile);
    return 2;
}

int printEBC(Image *img, char *nFilename){
   if (access(img->AFilename,W_OK) == -1){
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        return 0;
    }


    // write the header data in one block
    img->check = fprintf(img->associatedFile, "ec\n%d %d\n", img->height, img->width);

    if (img->check == 0){
        printf("ERROR: Bad Output\n");
        return 1;
        }

        int counter = 0;
        unsigned char mask1 = (unsigned char) 0x80;
        uint8_t toOutput;
        uint8_t toSaveToOutput;
        uint8_t current;

        for (int currentRow = 0; currentRow < img->height; currentRow++){
            for (int currentCol = 0; currentCol < img->width; currentCol++){
                current = img->imageData[currentRow][currentCol]<< 3; //shifts the 3 zeros to the end

                for (int i = 0; i < 5; i ++){
                    toSaveToOutput = current & mask1; //applys the mask to only get the top bit
                    toSaveToOutput = (toSaveToOutput >> counter); //moves the bit to the correct place to be pushed
                    toOutput |= toSaveToOutput; //places the bit in the output buffer
                    counter = counter + 1;

                    if (counter == 8){ // writes the output to the file when the buffer is full, then resets buffer
                        counter = 0;
                        img->check = fwrite(&toOutput,sizeof(unsigned char),1,img->associatedFile); 
                        if (img->check == 0){
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
    img->imageData = NULL;
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

void setFileR(Image *img, char *name){
    img->AFilename = name;
    img->associatedFile = fopen(img->AFilename, "r");
}

void setFileW(Image *img, char *name){
    img->AFilename = name;
    img->associatedFile = fopen(img->AFilename, "w");
}

int checkEbfData(Image *img){
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
        int check2 = fscanf(img->associatedFile, "%hhu", &img->imageData[currentRow][currentCol]);
        if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA)
            { // check inputted data
            printf("ERROR: Bad Data (%s)\n", img->AFilename);
            return 0;
            }
        }
    }

    uint8_t tmp; // checking too much
    int check2 = fscanf(img->associatedFile, "%hhu", &tmp);
    if (check2 != 0 && check2 != -1){
        printf("ERROR: Bad Data (%s)\n", img->AFilename);
        return 1;
    }
    
    fclose(img->associatedFile);
    return 2;
}

int checkEbuData(Image *img){
    fread(&img->imageData[0][0], sizeof(uint8_t), 1, img->associatedFile); // skips a line
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            int check2 = fread(&img->imageData[currentRow][currentCol], sizeof(uint8_t), 1, img->associatedFile);
            if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA){ // check inputted data
                    printf("ERROR: Bad Data (%s)\n", img->AFilename);
                    return 0;
                }
        }
    }

    unsigned char *tmp; // checking too much
        int check2 = fread(&tmp, sizeof(uint8_t) - 1, 1, img->associatedFile);
        if (check2 != 0){
            printf("ERROR: Bad Data(%s)\n", img->AFilename);
            return 1;
        }

    return 2;
}

int checkEbcData(Image *img){
    uint8_t ** imageDataForEbc; // creates a new 2d array that is the correct size for the compressed data
    uint8_t * dataBlockForEbc;
    imageDataForEbc = (uint8_t **)malloc(img->height *(5.0f/8.0f)* sizeof(uint8_t*)); // REMEMBER TO FREE!
    dataBlockForEbc = (uint8_t*)malloc(img->height*img->width*sizeof(uint8_t));
    for (int i = 0; i < img->height*(5.0f/8.0f); i = i + 1){
        imageDataForEbc[i] = dataBlockForEbc + i * img->width;
    }

    fread(&imageDataForEbc[0][0], sizeof(uint8_t), 1, img->associatedFile); // skip a line
    for (int currentRow = 0; currentRow < (img->height*(5.0f/8.0f)); currentRow++){ // read in the compressed data to the new array
        for (int currentCol = 0; currentCol < (img->width); currentCol++){
            int check2 = fread(&imageDataForEbc[currentRow][currentCol], sizeof(uint8_t), 1, img->associatedFile);
            if (check2 != 1){ // check inputted data
                printf("ERROR: Bad Data (%s)\n", img->AFilename);
                return 0;
            }
        }
    }

        unsigned char *tmp; // checking too much
        int check2 = fread(&tmp, sizeof(uint8_t), 1, img->associatedFile);
        if (check2 != 0){
            printf("ERROR: Bad Data (%s)\n", img->AFilename);
            return 1;
        }

    // convert from 5 bit to 8 bit and write to the img->imageData
    int currentWriteRow = 0;
    int currentWriteCol = 0;
    int counter = 0;
    unsigned char mask1 = (unsigned char) 0x80;
    uint8_t toOutput;
    uint8_t writeToOutput;
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
                        printf("ERROR: Bad Data (%s)\n", img->AFilename);
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



int readInFile(Image *img, char *type, char *nFilename){
    // checks file opened successfully
    setFileR(img, nFilename);
    switch(checkReadFileAccess(img)){
        case 0:
            printf("ERROR: Bad File Name (1)\n");
            return 1;
        case 1:
            return 1;
        default:
            break;
    }

    if (strcmp(type, "ebf") == 0){
        switch(checkMagicNumber(img, 0x6265)){
            case 0:
                return 2;
            default:
                break;
        }
    }

    if (strcmp(type, "ebu") == 0){
        switch(checkMagicNumber(img, 0x7565)){
            case 0:
                return 2;
            default:
                break;
        }
    }

    if (strcmp(type, "ebc") == 0){
        switch(checkMagicNumber(img, 0x6365)){
            case 0:
                return 2;
            default:
                break;
        }
    }

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    switch(dimensionScan(img)){
        case 0:
            fclose(img->associatedFile);
            return 3;
        default:
            break;            
    }

    mallocTheArray(img);
    // if malloc is unsuccessful, it will return a null pointer
    if (isBadMalloc(img) == 0){
            fclose(img->associatedFile);
            printf("ERROR: Image Malloc Failed\n");
            return 4;
        }

    if (strcmp(type, "ebf") == 0){
        switch(checkEbfData(img)){ // validates file perms
            case 0:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            case 1:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            default:
                return 0;
            }
    }

    if (strcmp(type, "ebu") == 0){
        switch(checkEbuData(img)){ // validates file perms
            case 0:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            case 1:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            default:
                return 0;
            }
    }

    if (strcmp(type, "ebc") == 0){
        switch(checkEbcData(img)){ // validates file perms
            case 0:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            case 1:
                free(img->imageData);
                fclose(img->associatedFile);
                return 5;
            default:
                return 0;
            }
    }

    
    return 0;
}

int compare(Image *image1, Image *image2){
        // compare the data from the two files:
    // start with magic number values
    if (*image1->magicNumberValue != *image2->magicNumberValue){
        free(image1->imageData);
        free(image2->imageData);
        printf("DIFFERENT\n");
        return 0;
        }

    // check dimensions
    if ((image1->height != image2->height) || (image1->width != image2->width)){
            free(image1->imageData);
            free(image2->imageData);
            printf("DIFFERENT\n");
            return 0;
        }

    // and check the pixel values
     for (int currentRow = 0; currentRow < image1->height; currentRow++){
        for (int currentCol = 0; currentCol < image1->width; currentCol++){
            if (image1->imageData[currentRow][currentCol] != image2->imageData[currentRow][currentCol]){
                    free(image1->imageData);
                    free(image2->imageData);
                    printf("DIFFERENT\n");
                    return 0;
            }
        }
     }
    return 1;
}

