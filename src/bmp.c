/* bmp.c */
#include "bmp.h"

/**
 *  This function will take an array of BMP pixel data and
 *  return the headers that reflect that data.
 *      @param pixelData        - contains BMP-formatted pixel data
 *      @param bmpHeader        - BMPHeader struct to populate based on pixel data and input
 *      @param dibHeader        - DIBHeader struct to populate based on pixel data and input
 *      @param width            - width in pixels of image
 *      @param height           - height in pixels of image
 *      @param bitsPerPixel     - number of bits per pixel for image
 *      @param compression      - compression value for image
 *      @param dpiHorizontal    - Horizontal resolution in dots per inch for image
 *      @param dpiVertical      - Vertical resolution in dots per inch for image
 *      @param colors           - Number of colors used in image
 *      @param important        - Number of important colors in image
 */
void make_bmp_headers(BMPHeader *bmpHeader, DIBHeader *dibHeader, int width, int height, int bitsPerPixel, int compression, int dpiHorizontal, int dpiVertical, int colors, int important) {
    /* Populate bmpHeader with data */
    bmpHeader->bfType       = 0x4D42;   // set signature to 'BM'
    bmpHeader->bfSize       = sizeof(BMPHeader) + sizeof(DIBHeader) + (height * width * (bitsPerPixel / 8));     // set FileSize
    bmpHeader->bfReserved1  = 0;        // unused
    bmpHeader->bfReserved2  = 0;        // unused
    bmpHeader->bfOffBits    = sizeof(BMPHeader) + sizeof(DIBHeader);    // set offset to start of bitmap data

    /* Populate dibHeader with data */
    dibHeader->size             = sizeof(DIBHeader);
    dibHeader->width            = width;
    dibHeader->height           = height;
    dibHeader->planes           = 1;
    dibHeader->bitsPerPixel     = bitsPerPixel;
    if (bitsPerPixel / 8 == 1) {
        bmpHeader->bfOffBits    = sizeof(BMPHeader) + sizeof(DIBHeader) + 1024;    // account for color map
        bmpHeader->bfSize       = sizeof(BMPHeader) + sizeof(DIBHeader) + 1024 + (height * width * (bitsPerPixel / 8));
    }

    dibHeader->compression      = compression;
    if (compression == 0) {
        dibHeader->imageSize    = 0;
    } else {
        dibHeader->imageSize    = ((bitsPerPixel * width + 31) / 32) * 4 * height; // if uncompressed 24-bit BMP, set to BI_RGB
    }
    dibHeader->XPixelsPerMeter  = (uint32_t)(dpiHorizontal * 39.3701);
    dibHeader->XPixelsPerMeter  = (uint32_t)(dpiVertical * 39.3701);
    dibHeader->colorsUsed       = colors;
    dibHeader->importantColors  = important;
}

/**
 *  This function will take BMP pixel data and save to 
 *  a new file of specified name.
 *      @param filename     - name of file to copy data to
 *      @param pixelData    - contains BMP-formatted pixel data
 *      @param width            - width in pixels of image
 *      @param height           - height in pixels of image
 *      @param bitsPerPixel     - number of bits per pixel for image
 *      @param compression      - compression value for image
 *      @param dpiHorizontal    - Horizontal resolution in dots per inch for image
 *      @param dpiVertical      - Vertical resolution in dots per inch for image
 *      @param colors           - Number of colors used in image
 *      @param important        - Number of important colors in image
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int save_as_bmp(const char *filename, uint8_t *pixelData, uint8_t *colorMap, int width, int height, int bitsPerPixel, int compression, int dpiHorizontal, int dpiVertical, int colors, int important){
    BMPHeader *bmpHeader = (BMPHeader *)calloc(1, sizeof(BMPHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for bmpHeader\n");
        return -1;
    }

    DIBHeader *dibHeader = (DIBHeader *)calloc(1, sizeof(DIBHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for dibHeader\n");
        free(bmpHeader);
        return -1;
    }

    make_bmp_headers(bmpHeader, dibHeader, width, height, bitsPerPixel, compression, dpiHorizontal, dpiVertical, colors, important);

    /* Open the output file to write */
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open output file.\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    /* Write the headers to file */
    fwrite(bmpHeader, 1, sizeof(BMPHeader), file);
    fwrite(dibHeader, 1, sizeof(DIBHeader), file);

    /* Write colormap to file */
    int colorMapSize = (colors ? dibHeader->colorsUsed : 256) * 4;
    if (colorMap) {
        fwrite(colorMap, colorMapSize, 1, file);
    }

    /* Write pixel data to file by individual row */
    int rowSize = ((bitsPerPixel * width + 31) / 32) * 4;
    int bytesPerPixel = bitsPerPixel / 8;
    int padding = rowSize - (width * bytesPerPixel);

    uint8_t paddingBytes[3] = {0};  // Up to 3 padding bytes to write after a row of pixels
    for (int row = 0; row < height; row++) {
        fwrite(pixelData + row * width * bytesPerPixel, bytesPerPixel * width, 1, file);
        
        if (padding > 0) {
            fwrite(paddingBytes, padding, 1, file);
        }
    }

    fclose(file);
    free(bmpHeader);
    free(dibHeader);
    return 0;
}

/**
 *  This function will take a file name and open as a BMP file
 *  Then, it will read the headers into structs.
 *  It will return the headers via pointer argument.
 *      @param filename     - name of BMP file to read headers from
 *      @param bmpHeader    - BMPHeader struct to populate from file
 *      @param dibHeader    - DIBHeader struct to populate from file
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int get_bmp_headers(const char *filename, BMPHeader *bmpHeader, DIBHeader *dibHeader) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    /* Read header data from file */
    fread(bmpHeader, sizeof(BMPHeader), 1, file);
    fread(dibHeader, sizeof(DIBHeader), 1, file);

    /* 'BM' in little-endian notation --- Check if file is valid */
    if (bmpHeader->bfType != 0x4D42) {
        fprintf(stderr, "File is not a valid BMP file\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/**
 *  This function will take a file name and open as a BMP file
 *  Then, it will read the pixel data into an array based on headers.
 *  It will return the pixel data array via pointer argument.
 *      @param filename     - name of BMP file to read pixel data from
 *      @param bmpHeader    - BMPHeader struct to inform reading pixel data
 *      @param dibHeader    - DIBHeader struct to inform reading pixel data
 *      @param pixelData    - array of pixel data to populate from file
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int get_bmp_pixeldata(const char *filename, BMPHeader *bmpHeader, DIBHeader *dibHeader, uint8_t *pixelData, uint8_t *colorMap) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    // move to offset (start of pixel data)
    fseek(file, bmpHeader->bfOffBits, SEEK_SET);

    // image properties
    int width           = dibHeader->width;
    int height          = dibHeader->height;
    int bitsPerPixel    = dibHeader->bitsPerPixel;
    int colorsUsed      = dibHeader->colorsUsed;
    int bytesPerPixel   = bitsPerPixel / 8;
    int rowSize         = ((bitsPerPixel * width + 31) / 32) * 4;
    int padding         = rowSize - (width * bytesPerPixel);

    /* Check if pixelData array is allocated */
    if (!pixelData) {
        fprintf(stderr, "pixelData array was not allocated.\n");
        fclose(file);
        return -1;
    }

    /* Read from file into array */
    for (int row = 0; row < height; row++) {
        fread(pixelData + row * width * bytesPerPixel, bytesPerPixel * width, 1, file);

        if (padding > 0) {
            fseek(file, padding, SEEK_CUR);
        }
    }

    // if color map is not NULL, read from file starting 1024 bytes before pixel data
    int colorMapSize = (colorsUsed ? colorsUsed : 256) * 4;
    if (colorMap) {
        fseek(file, bmpHeader->bfOffBits - colorMapSize, SEEK_SET);
        fread(colorMap, colorMapSize, 1, file);
    }

    fclose(file);
    return 0;
}