#include "allCommonFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int checkargs(int argCount, char *name){
    if (argCount != 3){ // expecting exe name, input filename and output filename
            if(argCount == 1){
                char *newStr = name + 2; // shift pointer 2 characters to the right to remove './' from exe name
                printf("Usage: %s file1 file2\n",newStr);
                return SUCCESS;
            }
            printf("ERROR: Bad Arguments\n");
            return BAD_ARGS;
    }

    return 99;  // success
}

int checkReadFileAccess(Image *img){
    if (access(img->AFilename,F_OK) == -1){ // if the file does not exist at all
        printf("ERROR: Bad File Name (1)\n");
        return 0;
    }
    if (access(img->AFilename,R_OK) == -1){
        printf("ERROR: Bad File Name (%s)\n", img->AFilename);
        disposeImage(img);
        return 0;
    }

    return 2;   // success
}

int checkMagicNumber(Image *img, int MAGIC_NUMBER){
    img->magicNumber[0] = getc(img->associatedFile);     // gets first 2 characters which should be magic number
    img->magicNumber[1] = getc(img->associatedFile); 
    img->magicNumberValue  = (unsigned short *)img->magicNumber; // casts to comaprable type
    if (*img->magicNumberValue != MAGIC_NUMBER){
        printf("ERROR: Bad Magic Number (%s)\n", img->AFilename);
        disposeImage(img);
        return 0;
    }

    return 1;   // success
}

int dimensionScan(Image *img){ // checks that the provided dims are within allowed range
    img->check = fscanf(img->associatedFile, "%d %d", &img->height, &img->width);
    if (img->check != 2 || img->height < MIN_DIMENSION || img->width < MIN_DIMENSION || img->height > MAX_DIMENSION || img->width > MAX_DIMENSION){ 
        printf("ERROR: Bad Dimensions (%s)\n", img->AFilename);
        disposeImage(img);
        return 0;
    }

    return 1;   // success
}

int printEBU(Image *img, char *nFilename){ // writes image data to EBU files; prior to this, the data should already be read in and converted
    setFileW(img, nFilename); // sets the fileapath
    if (access(img->AFilename,W_OK) == -1){ // checks writing perms
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        disposeImage(img);
        return BAD_OUTPUT;
    }

    img->check = fprintf(img->associatedFile, "eu\n%d %d\n", img->height, img->width);  // write the header data in one block
    if (img->check == 0){ // if writing failed
            printf("ERROR: Bad Output\n");
            disposeImage(img);
            return BAD_OUTPUT;
        }

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            img->check = fwrite(&img->imageData[currentRow][currentCol],sizeof(unsigned char),1,img->associatedFile); // prints sizeof(unsigned char) 1 time 
            if (img->check == 0){ // if write fails
                printf("ERROR: Bad Output\n");
                disposeImage(img);
                return BAD_OUTPUT;
            }
        }
    }

    fclose(img->associatedFile);
    return 0;   // success
}

int printEBF(Image *img, char *nFilename){  // writes EBF files; prior to this, the data should already be read in and converted
    setFileW(img, nFilename);   // sets the fileapath
    if (access(img->AFilename,W_OK) == -1){ //checks writing perms
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        disposeImage(img);
        return BAD_OUTPUT;
    }

    img->check = fprintf(img->associatedFile, "eb\n%d %d\n", img->height, img->width);  // write the header data in one block
    if (img->check == 0){  // if write fails
            printf("ERROR: Bad Output\n");
            disposeImage(img);
            return BAD_OUTPUT;
    }

    // iterates though the array and print out pixel values
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            // print data; if at the end of the line, print new line, otherwise print space
            img->check = fprintf(img->associatedFile, "%u%c", img->imageData[currentRow][currentCol], (currentCol != img->width - 1) ? ' ' : '\n');

            if (img->check == 0){
                printf("ERROR: Bad Output\n");
                disposeImage(img);
                return BAD_OUTPUT;
            }
        }
    }  

    fclose(img->associatedFile);
    return 0;   // success
}

int printEBC(Image *img, char *nFilename){
   if (access(img->AFilename,W_OK) == -1){  // if access fails
        printf("ERROR: Bad Output(%s)\n",img->AFilename);
        disposeImage(img);
        return BAD_OUTPUT;
    }

    img->check = fprintf(img->associatedFile, "ec\n%d %d\n", img->height, img->width);  // write the header data in one block
    if (img->check == 0){ // if wrte fails
        printf("ERROR: Bad Output\n");
        disposeImage(img);
        return BAD_OUTPUT;
    }

    int counter = 0;
    unsigned char mask1 = (unsigned char) 0x80; // bitmask of 10000000, to grab 1st bit of 8-bit seq.
    uint8_t toOutput;   // stores the final value to write
    uint8_t toSaveToOutput; // stores the values to be added to output (buffer)
    uint8_t current;    // stores the current value being read from Image struct

    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            current = img->imageData[currentRow][currentCol]<< 3;   // shifts the 3 zeros to the end

            for (int i = 0; i < 5; i ++){
                toSaveToOutput = current & mask1;   // applys the mask to only get the top bit
                toSaveToOutput = (toSaveToOutput >> counter);   // moves the bit to the correct place to be pushed to output buffer
                toOutput |= toSaveToOutput; // places the bit in the output buffer
                counter = counter + 1;

                if (counter == 8){  // writes the output to the file when the buffer is full, then resets buffer
                    counter = 0;
                    img->check = fwrite(&toOutput,sizeof(unsigned char),1,img->associatedFile); 
                    if (img->check == 0){   // if write fails
                        printf("ERROR: Bad Output\n");
                        disposeImage(img);
                        return BAD_OUTPUT;
                    }

                    toOutput = 0;   // sets the output back to 00000000; needed as OR is used in line "toOutput |= toSaveToOutput;"
                }

                current <<= 1;  // shifts the current data left ready to be masked
            }
        }
    }

    return 0;   // success
}

void mallocTheArray(Image *img){    // creates space for image stuct
    img->imageData = NULL;
    img->imageData = (uint8_t **)malloc(img->height * sizeof(uint8_t*));
    img->dataBlock = (uint8_t*)malloc(img->height*img->width*sizeof(uint8_t));
    for (int i = 0; i < img->height; i = i + 1){
        img->imageData[i] = img->dataBlock + i * img->width;
    }
}

int isBadMalloc(Image *img){
    if (img->dataBlock == NULL){
        printf("ERROR: Image Malloc Failed\n");
        disposeImage(img);
        return 0;
    }
    
    return 1;   // no malloc issues / success
}

void setFileR(Image *img, char *name){  // sets the filepath in read mode
    img->AFilename = name;
    img->associatedFile = fopen(img->AFilename, "r");
}

void setFileW(Image *img, char *name){  // sets the filepath in write mode
    img->AFilename = name;
    img->associatedFile = fopen(img->AFilename, "w");
}

int checkEbfData(Image *img){   // reads in ebf data; stores to Image struct
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            int check2 = fscanf(img->associatedFile, "%hhu", &img->imageData[currentRow][currentCol]);  // reads in data, stores to 2d array
            if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA){ // check inputted data vs range allowed
                printf("ERROR: Bad Data (%s)\n", img->AFilename);
                fclose(img->associatedFile);
                disposeImage(img);
                return 0;
            }
        }
    }

    uint8_t tmp; // checking for too much data
    int check2 = fscanf(img->associatedFile, "%hhu", &tmp);
    if (check2 != 0 && check2 != -1){
        printf("ERROR: Bad Data (%s)\n", img->AFilename);
        fclose(img->associatedFile);
        disposeImage(img);
        return 0;
    }
    
    fclose(img->associatedFile);
    return 2;   // success
}

int checkEbuData(Image *img){   // reads in ebu data; stores to Image struct
    fread(&img->imageData[0][0], sizeof(uint8_t), 1, img->associatedFile); // skips a line
    for (int currentRow = 0; currentRow < img->height; currentRow++){
        for (int currentCol = 0; currentCol < img->width; currentCol++){
            int check2 = fread(&img->imageData[currentRow][currentCol], sizeof(uint8_t), 1, img->associatedFile);   // reads in data, stores to 2d array
            if (check2 !=1 || img->imageData[currentRow][currentCol] > MAX_DATA || img->imageData[currentRow][currentCol] < MIN_DATA){  // check inputted data vs range allowed
                printf("ERROR: Bad Data (%s)\n", img->AFilename);
                fclose(img->associatedFile);
                disposeImage(img);
                return 0;
            }
        }
    }

    unsigned char *tmp; // checking for too much data
    int check2 = fread(&tmp, sizeof(uint8_t) - 1, 1, img->associatedFile);
    if (check2 != 0){
        printf("ERROR: Bad Data(%s)\n", img->AFilename);
        fclose(img->associatedFile);
        disposeImage(img);
        return 0;
    }

    return 2;   // success
}

int checkEbcData(Image *img){   // reads in ebc data; stores to Image struct
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
                fclose(img->associatedFile);
                disposeImage(img);
                return 0;
            }
        }
    }

        unsigned char *tmp; // checking too much
        int check2 = fread(&tmp, sizeof(uint8_t), 1, img->associatedFile);
        if (check2 != 0){
            printf("ERROR: Bad Data (%s)\n", img->AFilename);
            fclose(img->associatedFile);
            disposeImage(img);
            return 0;
        }

    // convert from 5 bit to 8 bit representaion and store to img->imageData (adds 3 redundant 0s at front)
    int currentWriteRow = 0;    // counters show where to store the data in the 2d array
    int currentWriteCol = 0;
    int counter = 0;
    unsigned char mask1 = (unsigned char) 0x80; // mask of 1000000 to grab 1st bit of 8
    uint8_t toOutput;   // output to be stored to 2d array in stuct
    uint8_t writeToOutput;  // output buffer
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

                // if output is full, store output and set counter to 0
                if (counter == 5){
                    counter = 0;
                    img->imageData[currentWriteRow][currentWriteCol] = toOutput >> 3;

                    if (img->imageData[currentWriteRow][currentWriteCol] > 31){
                        printf("%d\n", img->imageData[currentWriteRow][currentWriteCol]);
                    }
                    if (img->imageData[currentWriteRow][currentWriteCol] > MAX_DATA || img->imageData[currentWriteRow][currentWriteCol] < MIN_DATA){ // check inputted data
                        printf("ERROR: Bad Data (%s)\n", img->AFilename);
                        fclose(img->associatedFile);
                        disposeImage(img);
                        return 0;
                    }

                    toOutput = 0;   // 00000000
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
    
    return 2;   // success
}



int readInFile(Image *img, char *type, char *nFilename){    // reads in any type of file (ebf,u,c) and validates the information
    setFileR(img, nFilename);   // sets the filepath
    switch(checkReadFileAccess(img)){
        case 0:
            return BAD_FILE;
        default:
            break;
    }

    if (strcmp(type, "ebf") == 0){  // the type is checked here so the magic number is correct
        switch(checkMagicNumber(img, 0x6265)){
            case 0:
                return BAD_MAGIC_NUMBER;
            default:
                break;
        }
    }

    if (strcmp(type, "ebu") == 0){
        switch(checkMagicNumber(img, 0x7565)){
            case 0:
                return BAD_MAGIC_NUMBER;
            default:
                break;
        }
    }

    if (strcmp(type, "ebc") == 0){
        switch(checkMagicNumber(img, 0x6365)){
            case 0:
                return BAD_MAGIC_NUMBER;
            default:
                break;
        }
    }

    switch(dimensionScan(img)){ // scan for the dimensions
        case 0:
            return BAD_DIM;
        default:
            break;            
    }

    mallocTheArray(img);
    if (isBadMalloc(img) == 0){
            return BAD_MALLOC;
        }

    if (strcmp(type, "ebf") == 0){
        switch(checkEbfData(img)){ // validates file perms
            case 0:
                return BAD_DATA;
            default:
                return 0;
            }
    }

    if (strcmp(type, "ebu") == 0){
        switch(checkEbuData(img)){ // validates file perms
            case 0:
                return BAD_DATA;
            default:
                return 0;
            }
    }

    if (strcmp(type, "ebc") == 0){
        switch(checkEbcData(img)){ // validates file perms
            case 0:
                return BAD_DATA;
            default:
                return 0;
            }
    }
    
    return 0;   // success
}

int compare(Image *image1, Image *image2){  // compare the data from the two files
    if (*image1->magicNumberValue != *image2->magicNumberValue){ // compares magic values
        printf("DIFFERENT\n");
        return 0;   // success
    }

    if ((image1->height != image2->height) || (image1->width != image2->width)){    // compares values
        printf("DIFFERENT\n");
        return 0;   // success
    }

     for (int currentRow = 0; currentRow < image1->height; currentRow++){   // checks data values
        for (int currentCol = 0; currentCol < image1->width; currentCol++){
            if (image1->imageData[currentRow][currentCol] != image2->imageData[currentRow][currentCol]){
                    printf("DIFFERENT\n");
                    return 0;   // success
            }
        }
     }

    return 1;   // NON success
}

Image* newImage(){  // initialises the custom Image stuct
    Image *image = (Image*)malloc(sizeof(Image));
    return image;
}

void disposeImage(Image *img){  // frees the memory reserved for the Image struct
    free(img->dataBlock);
    free(img->imageData);
    return;
}