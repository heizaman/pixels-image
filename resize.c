/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    int n = atoi(argv[1]);
    
    if(n < 1 || n > 100)
    {
        printf("n must be in [1,100]\n");
        return 5;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // determine padding for scanlines
    int paddingold = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // preserve old variables to be used in iteration
    LONG widthold = bi.biWidth;
    LONG heightold = abs(bi.biHeight);
    
    // edit file and info header to match resized version
    bi.biWidth *= n;
    bi.biHeight *= n;
    int paddingnew = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + paddingnew) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // temporary storage
    RGBTRIPLE triple[widthold];

    // iterate over infile's scanlines
    for (int i = 0; i < heightold; i++)
    {   
        // iterate over pixels in scanline
        for (int j = 0; j < widthold; j++)
        {
            // read RGB triple from infile and store them in an array
            fread(&triple[j], sizeof(RGBTRIPLE), 1, inptr);
        }    
        
        // repeating vertically
        for(int q = 0; q < n; q ++)
        {
            for(int j = 0; j < widthold; j++)
            {
                // repeating horizontally
                for(int p = 0; p < n; p++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple[j], sizeof(RGBTRIPLE), 1, outptr);   
                }
            }
            
            // add new padding to outfile
            for (int k = 0; k < paddingnew; k++)
            {
                fputc(0x00, outptr);
            } 
        }
        
        // skip over padding, if any
        fseek(inptr, paddingold, SEEK_CUR);
    }
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
