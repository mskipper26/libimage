/* bmp.h */

#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push, 1)       // tell the compile to pack struct members with a 1-byte alignment

/* Bitmap File Header */
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

/* DIB Header */
typedef struct {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  XPixelsPerMeter;
    int32_t  YPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t importantColors;
} DIBHeader;
#pragma pack(pop)

void make_bmp_headers(BMPHeader *bmpHeader, DIBHeader *dibHeader, int width, int height, int bitsPerPixel, int compression, int dpiHorizontal, int dpiVertical, int colors, int important);
int save_as_bmp(const char *filename, uint8_t *pixelData, uint8_t *colorMap, int width, int height, int bitsPerPixel, int compression, int dpiHorizontal, int dpiVertical, int colors, int important);
int get_bmp_headers(const char *filename, BMPHeader *bmpHeader, DIBHeader *dibHeader);
int get_bmp_pixeldata(const char *filename, BMPHeader *bmpHeader, DIBHeader *dibHeader, uint8_t *pixelData, uint8_t *colorMap);

#endif