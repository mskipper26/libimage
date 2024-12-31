/* jpeg.c */

#include "jpeg.h"

/**
 *  This function will take a jpeg file and decompress it into pixel data.
 *
 *      @param filename         - name of source file of compressed data
 *      @param image_buffer     - raw pixel data from decompression --- returned via pointer (must be allocated in function and freed by caller)
 *      @param width            - width in pixels of decompressed image --- returned via pointer
 *      @param height           - height in pixels of decompressed image --- returned via pointer
 *      @param colors           - number of colors used in image --- returned via pointer
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int decompress_jpeg(const char *filename, unsigned char **image_buffer, int *width, int *height, int *components) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        fprintf(stderr, "Failed to open JPEG file for reading.\n");
        return -1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);         // set source as file
    jpeg_read_header(&cinfo, TRUE);         // read header markers

    jpeg_start_decompress(&cinfo);

    *width      = cinfo.output_width;
    *height     = cinfo.output_height;
    *components = cinfo.out_color_components;   // 3 for RGB, 1 for Greyscale

    /* Allocate memory to image_buffer */
    *image_buffer = calloc(1, (*width) * (*height) * (*components));
    if (!*image_buffer) {
        fprintf(stderr, "Failed to allocate memory for image buffer.\n");
        fclose(infile);
        return -1;
    }
    
    unsigned char *row_pointer[1];
    while(cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &(*image_buffer)[cinfo.output_scanline * (*width) * (*components)];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);

    return 0;
}

/**
 *  This function will take pixel data and compress into jpeg file.
 *
 *      @param filename         - name of destination file of compressed data
 *      @param image_buffer     - raw pixel data to be compressed
 *      @param width            - width in pixels of image
 *      @param height           - height in pixels of image
 *      @param input_components - Number of color channels (3 for RGB, 1 for grayscale)
 *      @param in_color_space   - color space of source image (typically JCS_RGB or JCS_GRAYSCALE)
 *
 *      @return success of operation: -1 -> failure, 0 -> success
 */
int compress_jpeg(const char *filename, unsigned char *image_buffer, int width, int height, int input_components, int in_color_space) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *outfile = fopen(filename, "wb");
    if (!outfile) {
        fprintf(stderr, "Failed to open JPEG file for writing.\n");
        return -1;
    }
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile); // set destination to file

    // set cinfo parameters
    cinfo.image_width   = width;
    cinfo.image_height  = height;
    cinfo.input_components = input_components;
    cinfo.in_color_space = in_color_space;
    jpeg_set_defaults(&cinfo);              // set default parameters for compression object
    
    jpeg_start_compress(&cinfo, TRUE);      // begin compression cycle
                                            // "TRUE" ensures a complete JPEG interchange datastream will be written.
    
    /* Read data into buffer */
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image_buffer[cinfo.next_scanline * width * input_components];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);           // complete compression cycle
    jpeg_destroy_compress(&cinfo);          // release compression object

    fclose(outfile);

    return 0;
}
