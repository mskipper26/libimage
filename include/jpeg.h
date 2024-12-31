/* jpeg.h */

#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>


int decompress_jpeg(const char *filename, unsigned char **image_buffer, int *width, int *height, int *components);
int compress_jpeg(const char *filename, unsigned char *image_buffer, int width, int height, int input_components, int in_color_space);

#endif