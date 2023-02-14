#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char **argv)
    { // main

    // validate that user has enter 2 arguments (plus the executable name)
    if (argc != 3)
        { // check arg count
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
        } // check arg count

    // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber[2];
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    // create and initialise variables used within code
    int width = 0, height = 0;
    unsigned int *imageData;
    long numBytes;

    // open the input file in read mode
    FILE *inputFile = fopen(argv[1], "r");
    // check file opened successfully
    if (!inputFile)
        { // check file pointer
        printf("ERROR: Bad File Name\n");
        return BAD_FILE;
        } // check file pointer

    // get first 2 characters which should be magic number
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);

    // checking against the casted value due to endienness.
    if (*magicNumberValue != MAGIC_NUMBER)
        { // check magic number
        printf("ERROR: Bad Magic Number\n");
        return BAD_FILE;
        } //check magic number

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &height, &width);
    if (check != 2 || height < MIN_DIMENSION || width < MIN_DIMENSION || height > MAX_DIMENSION || width > MAX_DIMENSION)
        { // check dimensions
        // close the file as soon as an error is found
        fclose(inputFile);
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions\n");
        return BAD_DIM;
        } // check dimensions

    // caclulate total size and allocate memory for array
    numBytes = height * width;
    imageData = (unsigned int *)malloc(numBytes * sizeof(unsigned int));

    // if malloc is unsuccessful, it will return a null pointer
    if (imageData == NULL)
        { // check malloc
        fclose(inputFile);
        printf("ERROR: Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    // read in each grey value from the file
    for (int current = 0; current < numBytes; current++)
        { // reading in
        check = fscanf(inputFile, "%u", &imageData[current]);
        // validate that we have captured 1 pixel value
        if (check != 1)
            { // check inputted data
            // ensure that allocated data is freed before exit.
            free(imageData);
            fclose(inputFile);
            printf("ERROR: Bad Data\n");
            return BAD_DATA;
            } // check inputted data
        } // reading in

    // now we have finished using the inputFile we should close it
    fclose(inputFile);

    // open the output file in write mode
    FILE *outputFile = fopen(argv[2], "w");
    // validate that the file has been opened correctly
    if (outputFile == NULL)
        { // validate output file
        free(imageData);
        printf("ERROR: Bad File Name\n");
        return BAD_FILE;
        } // validate output file

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