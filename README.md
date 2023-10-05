# Image Compress Project

This software will convert image data from one file format to another. It makes use of bit-packing.

This project has been taken from my private university github account, so commit history/comments are not available.

Some more info is below.

| Value  | String | Condition |
| ------------- | ------------- | ------------- |
| 1  | ERROR: Bad Argument Count | Program given wrong # of arguments |
| 2 | ERROR: Bad File Name (fname) | Program fails to open file |
| 3 | ERROR: Bad Magic Number (fname) | File has an illegal magic number |
| 4 | ERROR: Bad Dimensions (fname) | File has illegal dimensions |
| 5 | ERROR: Image Malloc Failed | Malloc failed to allocate memory |
| 6 | ERROR: Bad Data (fname) | Reading in data failed |
| 7 | ERROR: Output Failed (fname) | Writing out data failed |
| 100 | ERROR: Miscellaneous (describe) | Any other error which is detected |


| String | Condition |
| ------------- | ------------- |
| Usage: executablename file1 file2 | Program run with no arguments |
| ECHOED | xEcho file successfully completes |
| IDENTICAL | xComp file successfully completes and files are identical |
| DIFFERENT | xComp file successfully completes and files are not identical |
| CONVERTED | x2y file successfully completes |

Usage messages are also a Unix convention - when an executable is run with no arguments, it is assumed that the user does not know the correct syntax and wishes to see a prompt. As this is not an error state, the return value is 0.

# File Format Details

The three file formats are ebf, ebu and ebc - these are all **new file formats** which have been created for this project.

You have some sample ebf files available in the sample_images folder.

## ebf files

ebf files are a basic greyscale image file. They contain a short header of metadata, followed by pixel values. The basic format is as follows:

```
eb              - this is the magic number - for ebf files this is always eb.
height width    - two space-separated integers which give the height and width of the image.
p1 p2 p3 ...    - the pixel data which is separated by spaces. This is usually (but not always) separated into rows by a newline.
```

ebf files have a maximum pixel value of 31.

## ebu files

ebu (ebf uncompressed) are the binary equivalent of ebf files. They have a very similar header (written in ASCII) followed by the pixel data written out as **binary**. If you open an ebu file in a text editor, you will generally see a random string of data as it is not encoded in a human-readable format.

```
eu
height width
p1 p2 p3 ...
```

ebc files have a maximum pixel value of 31.

## ebc files

ebc (ebf compressed) are compressed binary ebf files. They also have a similar header, but the pixel data is written out in **compressed** binary. As ebf files have a maximum pixel value of 31, each pixel can be represented by **5 bits of binary**. C can only write out blocks of 8 bits (1 byte) at once, so the algorithm packs pixel data into the minimum number of bytes. This should compress an ebu file to just over 0.6 times its original size.

```
ec
height width
packed binary pixel data
```
