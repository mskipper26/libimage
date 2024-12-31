/* image.h */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <jpeglib.h>

int bmp_to_jpeg(const char *source, const char *dest);
int jpeg_to_bmp(const char *source, const char *dest);
int duplicate_bmp_file(const char *source, const char *dest);
int duplicate_jpeg_file(const char *source, const char *dest);

#endif