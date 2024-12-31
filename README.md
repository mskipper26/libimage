libimage is a shared C library for operating on image files to perform
such functions as duplicating a file, converting a file to another type,
lowering the resolution of a file, altering image pixel data.

The purpose of this project is as an exercise and to improve my skills
in building C libraries. This library is by no means exhaustive and will
only support a limited number of use cases. It is a work in progress, so
this is not the finished state of the library.

Author:  Michael Skipper

Version: 1.0

Date:    December 31, 2024

Features:

    - File types supported: JPEG, BMP
    - Data representation: supports 24-bit RGB and 8-bit grayscale images
    - Conversion: supports compression to and decompression from JPEG
    - Duplication: supports duplication of BMP and JPEG files.
        - NOTE: obviously there are easier methods of duplicating a file,
                but for the sake of the library's completeness, duplication
                is done manually by interpreting data (getting headers, pixel data
                for BMP; decompressing and compressing for JPEG)

Future Implementations:

    - Support for reduction in resolution of images.
    - Support for PNG files, possibly other file types.
    - Modification of pixel data 
        - Inversion of colors
        - increase/decrease value for a color channel
        - Conversion between RGB and grayscale representations

References:

    - For BMP file format: [https://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm]

    - For libjpeg: [https://github.com/winlibs/libjpeg/blob/master/libjpeg.txt]
