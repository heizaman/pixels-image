/**
 * whodunit.c
 *
 * Computer Science 50
 * Problem Set 4
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./whodunit ClueFile VerdictFile\n");
        return 1;
    }

    // remember filenames
    char* cluefile = argv[1];
    char* verdictfile = argv[2];

    // open clue file 
    FILE* clueptr = fopen(cluefile, "r");
    if (clueptr == NULL)
    {
        printf("Could not open %s.\n", cluefile);
        return 2;
    }

    // open verdict file
    FILE* verptr = fopen(verdictfile, "w");
    if (verptr == NULL)
    {
        fclose(clueptr);
        fprintf(stderr, "Could not create %s.\n", verdictfile);
        return 3;
    }

    // read cluefile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, clueptr);

    // read cluefile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, clueptr);

    // ensure cluefile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(verptr);
        fclose(clueptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write verdictfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, verptr);

    // write verdictfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, verptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over cluefile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from cluefile
            fread(&triple, sizeof(RGBTRIPLE), 1, clueptr);
            
            // edit the RGB triple to reveal the culprit
            if(triple.rgbtRed == 0xff)
                {
                    triple.rgbtBlue = 0xff;
                    triple.rgbtGreen = 0xff;
                }
            
            // write edited RGB triple to verdictfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, verptr);
        }

        // skip over padding, if any
        fseek(clueptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, verptr);
        }
    }

    // close cluefile
    fclose(clueptr);

    // close verdictfile
    fclose(verptr);

    // that's all folks
    return 0;
}
