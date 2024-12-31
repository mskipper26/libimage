/* main.c */

#include "include/bmp.h"
#include "include/jpeg.h"
#include "include/image.h"

int main() {
    const char *source = "test_images/barbara_gray.bmp";
    const char *source2 = "test_images/grayscale_example.jpg";
    const char *copy_bmp   = "copy.bmp";
    const char *copy2_bmp   = "copy2.bmp";
    const char *copy_jpeg   = "copy.jpeg";
    const char *copy2_jpeg   = "copy2.jpeg";

    if (bmp_to_jpeg(source, copy_jpeg) == -1) {
        fprintf(stderr, "Failed to convert BMP file to JPEG.\n");
        return 1;
    }

    if (duplicate_bmp_file(source, copy_bmp) == -1) {
        fprintf(stderr, "Failed to copy BMP file.\n");
        return 1;
    }

    if (jpeg_to_bmp(source2, copy2_bmp) == -1) {
        fprintf(stderr, "Failed to convert JPEG file to BMP.\n");
        return 1;
    }

    if (duplicate_jpeg_file(source2, copy2_jpeg) == -1) {
        fprintf(stderr, "Failed to copy JPEG file.\n");
        return 1;
    }

    return 0;
}