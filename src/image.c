/* image.c */

#include "image.h"
#include "bmp.h"
#include "jpeg.h"

/**
 * This function converts a given BMP image file to
 * a new JPEG file.
 *
 *      @param source       - This is the path to a BMP file to be converted
 *      @param dest         - This is the path to the new JPEG file
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int bmp_to_jpeg(const char *source, const char *dest) {
    BMPHeader *bmpHeader = (BMPHeader *)malloc(sizeof(BMPHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for bmpHeader\n");
        return -1;
    }

    DIBHeader *dibHeader = (DIBHeader *)malloc(sizeof(DIBHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for dibHeader\n");
        free(bmpHeader);
        return -1;
    }

    if (get_bmp_headers(source, bmpHeader, dibHeader) == -1) {
        fprintf(stderr, "Failed get headers from file.\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    int width           = dibHeader->width;
    int height          = dibHeader->height;
    int bitsPerPixel    = dibHeader->bitsPerPixel;
    int rowSize         = ((bitsPerPixel * width + 31) / 32) * 4;
    int in_color_space;
    if (bitsPerPixel / 8 == 3) {
        in_color_space = JCS_RGB;
    } else if (bitsPerPixel / 8 == 1) {
        in_color_space = JCS_GRAYSCALE;
    } else {
        fprintf(stderr, "Unsupported color space\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    uint8_t *pixelData  = (uint8_t *)malloc(rowSize * height);
    if (!pixelData) {
        fprintf(stderr, "Failed to allocate memory for pixel data array\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    if (get_bmp_pixeldata(source, bmpHeader, dibHeader, pixelData, NULL) == -1) {
        fprintf(stderr, "Failed to get pixel data from file.\n");
        free(bmpHeader);
        free(dibHeader);
        free(pixelData);
        return -1;
    }
    
    unsigned char *image_buffer = malloc(height * width * (bitsPerPixel / 8));
    if (!image_buffer) {
        fprintf(stderr, "Failed to allocate image buffer.\n");
        free(bmpHeader);
        free(dibHeader);
        free(pixelData);
        return -1;
    }
    
    for (int y = 0; y < height; y++) {
        int bmp_y = height - 1 - y; // file row order bottom to top for BMP - read top to bottom

        for (int x = 0; x < width; x++) {
            /* Calculate pixelData offsets for given pixel*/
            int rowOff      = bmp_y * rowSize;
            int pixelOff    = x * (bitsPerPixel / 8);

            // Calculate image buffer index for given pixel
            int index = (y * width + x) * (bitsPerPixel / 8);

            if (bitsPerPixel / 8 == 3) {
                /* Get color values --- BMP stores in BGR order */
                uint8_t blue    = pixelData[rowOff + pixelOff];
                uint8_t green   = pixelData[rowOff + pixelOff + 1];
                uint8_t red     = pixelData[rowOff + pixelOff + 2];

                /* Assign color values --- JPEG stores in RGB order */
                image_buffer[index]     = (unsigned char)red;
                image_buffer[index + 1] = (unsigned char)green;
                image_buffer[index + 2] = (unsigned char)blue;
            } else {
                uint8_t gray = pixelData[rowOff + pixelOff];
                image_buffer[index] = (unsigned char)gray;
            }
        }
    }

    
    /* Compress image buffer into JPEG file */
    if (compress_jpeg(dest, image_buffer, width, height, (bitsPerPixel / 8), in_color_space) == -1) {
        fprintf(stderr, "Failed to compress jpeg file\n");
        free(bmpHeader);
        free(dibHeader);
        free(pixelData);
        free(image_buffer);
        return -1;
    }

    free(bmpHeader);
    free(dibHeader);
    free(pixelData);
    free(image_buffer);
    return 0;
}

/**
 * This function converts a given JPEG image file to
 * a new BMP file.
 *
 *      @param source       - This is the path to a JPEG file to be converted
 *      @param dest         - This is the path to the new BMP file
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int jpeg_to_bmp(const char *source, const char *dest) {
    unsigned char *image_buffer = NULL;
    int *width      = calloc(1, sizeof(int));
    int *height     = calloc(1, sizeof(int));
    int *components = calloc(1, sizeof(int));

    if (decompress_jpeg(source, &image_buffer, width, height, components) == -1) {
        fprintf(stderr, "Failed to decompress JPEG image.\n");
        free(width);
        free(height);
        free(components);
        if (image_buffer) free(image_buffer);
        return -1;
    }
    if (!image_buffer) {
        fprintf(stderr, "Failed to decompress JPEG image - image_buffer NULL.\n");
        free(width);
        free(height);
        free(components);
        return -1;
    }

    int bitsPerPixel    = (*components) * 8;
    int rowSize         = ((bitsPerPixel * (*width) + 31) / 32) * 4;
    if (bitsPerPixel / 8 != 3 && bitsPerPixel / 8 != 1) {
        fprintf(stderr, "Unsupported color space\n");
        free(image_buffer);
        return -1;
    }

    uint8_t *pixelData  = (uint8_t *)malloc(rowSize * (*height));
    if (!pixelData) {
        fprintf(stderr, "Failed to allocate memory for pixel data array\n");
        free(width);
        free(height);
        free(components);
        free(image_buffer);
        return -1;
    }
    
    for (int y = 0; y < (*height); y++) {
        int bmp_y = (*height) - 1 - y;      // file row order bottom to top for BMP - read top to bottom

        for (int x = 0; x < (*width); x++) {
            // Calculate image buffer index for given pixel
            int index = (y * (*width) + x) * (*components);

            /* Calculate pixelData offsets for given pixel*/
                int rowOff      = bmp_y * rowSize;
                int pixelOff    = x * (bitsPerPixel / 8);
            
            if (bitsPerPixel / 8 == 3) {                // RGB
                /* Get color values --- JPEG stores in RGB order */
                uint8_t red     = (uint8_t)image_buffer[index];
                uint8_t green   = (uint8_t)image_buffer[index + 1];
                uint8_t blue    = (uint8_t)image_buffer[index + 2];
                
                /* Assign color values --- BMP stores in BGR order */
                pixelData[rowOff + pixelOff]     = blue;
                pixelData[rowOff + pixelOff + 1] = green;
                pixelData[rowOff + pixelOff + 2] = red;
            } else {         // Grayscale
                uint8_t gray = (uint8_t)image_buffer[index];
                pixelData[rowOff + pixelOff] = gray;
            } 
        }
    }

    int colors = 1<<(bitsPerPixel / 8);

    if (save_as_bmp(dest, pixelData, NULL, (*width), (*height), bitsPerPixel, 0, 96, 96, colors, 0) == -1) {
        fprintf(stderr, "Failed to save BMP file\n");
        free(width);
        free(height);
        free(components);
        free(image_buffer);
        free(pixelData);
        return -1;
    }

    free(width);
    free(height);
    free(components);
    free(image_buffer);
    free(pixelData);

    return 0;
}

/**
 *  This function will take two BMP file names and copy the contents
 *  of source to a new file called dest.
 *      @param source   - name of BMP file to duplicate data fromm
 *      @param dest     - name of BMP file to duplicate data to
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int duplicate_bmp_file(const char *source, const char *dest) {
    BMPHeader *bmpHeader = (BMPHeader *)malloc(sizeof(BMPHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for bmpHeader\n");
        return -1;
    }

    DIBHeader *dibHeader = (DIBHeader *)malloc(sizeof(DIBHeader));
    if (!bmpHeader) {
        fprintf(stderr, "Failed to allocate memory for dibHeader\n");
        free(bmpHeader);
        return -1;
    }

    if (get_bmp_headers(source, bmpHeader, dibHeader) == -1) {
        fprintf(stderr, "Failed get headers from file.\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    int width           = dibHeader->width;
    int height          = dibHeader->height;
    int bitsPerPixel    = dibHeader->bitsPerPixel;
    int rowSize         = ((bitsPerPixel * width + 31) / 32) * 4;
    uint8_t *pixelData  = (uint8_t *)malloc(rowSize * height);
    if (!pixelData) {
        fprintf(stderr, "Failed to allocate memory for pixel data array\n");
        free(bmpHeader);
        free(dibHeader);
        return -1;
    }

    /* If a grayscale image, must get color map */
    int colorMapSize = (dibHeader->colorsUsed ? dibHeader->colorsUsed : 256) * 4;
    uint8_t *colorMap = NULL;
    if (bitsPerPixel / 8 == 1) {
        colorMap = (uint8_t *)malloc(colorMapSize);
    }

    if (get_bmp_pixeldata(source, bmpHeader, dibHeader, pixelData, colorMap) == -1) {
        fprintf(stderr, "Failed to get pixel data from file.\n");
        free(bmpHeader);
        free(dibHeader);
        free(pixelData);
        return -1;
    }

    save_as_bmp(dest, pixelData, colorMap, width, height, bitsPerPixel, 
                dibHeader->compression, (int)(dibHeader->XPixelsPerMeter/39.3701), (int)(dibHeader->YPixelsPerMeter/39.3701),
                dibHeader->colorsUsed, dibHeader->importantColors);

    free(bmpHeader);
    free(dibHeader);
    free(pixelData);
    return 0;
}


/**
 *  This function will take two JPEG file names and copy the contents
 *  of source to a new file called dest.
 *      @param source   - name of JPEG file to duplicate data fromm
 *      @param dest     - name of JPEG file to duplicate data to
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int duplicate_jpeg_file(const char *source, const char *dest) {
    /* Initialize metadata for decompression */
    unsigned char *image_buffer = NULL;
    int *width      = calloc(1, sizeof(int));
    int *height     = calloc(1, sizeof(int));
    int *components = calloc(1, sizeof(int));

    /* Decompress souce JPEG file */
    if (decompress_jpeg(source, &image_buffer, width, height, components) == -1) {
        fprintf(stderr, "Failed to decompress JPEG image.\n");
        free(width);
        free(height);
        free(components);
        if (image_buffer) free(image_buffer);
        return -1;
    }
    if (!image_buffer) {
        fprintf(stderr, "Failed to decompress JPEG image - image_buffer NULL.\n");
        free(width);
        free(height);
        free(components);
        return -1;
    }

    /* Compress image buffer into JPEG file */
    if (compress_jpeg(dest, image_buffer, (*width), (*height), (*components), JCS_RGB) == -1) {
        fprintf(stderr, "Failed to compress jpeg file\n");
        free(width);
        free(height);
        free(components);
        free(image_buffer);
        return -1;
    }

    free(width);
    free(height);
    free(components);
    free(image_buffer);
    return 0;
}