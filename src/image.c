/*  Copyright (c) 2008 Jens Georg.
 
    This file is part of tn.

    tn is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    tn is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with tn.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>

#ifdef HAVE_JPEG
#   include <jpeglib.h>
#endif

#include "image.h"
#include "util.h"

#ifdef HAVE_JPEG
static int 
save_image_jpeg(const char* filename, uint8_t* rgb_buffer, int width, int height)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* outfile;
    JSAMPROW row_pointer[1];
    int row_stride = width * 3;

    outfile = fopen (filename, "wb");
    return_if(NULL == outfile, -1);

    /* use default error handle */
    cinfo.err = jpeg_std_error(&jerr);

    /* set jpeg parameters */
    jpeg_create_compress(&cinfo);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE);

    /* set output file */
    jpeg_stdio_dest(&cinfo, outfile);

    /* start encoding & writing to disk */
    jpeg_start_compress(&cinfo, TRUE);
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &rgb_buffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    jpeg_destroy_compress(&cinfo);

    return 0;
}
#endif

static int
save_image_ppm(const char* filename, uint8_t* rgb_buffer, int width, int height)
{
    FILE* file;
    int line;
    
    file = fopen(filename, "wb");
    return_if(NULL == file, -1);

    /* write file header */
    fprintf(file, "P6\n%d %d\n255\n", width, height);

    /* dump rgb data */
    for (line = 0; line < height; ++line)
    {
        fwrite(rgb_buffer + line * width * 3, 1, width * 3, file);
    }

    fclose(file);
    return 0;
}

int
image_save(const char* filename, uint8_t* rgb_buffer, int width, int height, enum ImageFormat format)
{
    return_if(NULL == rgb_buffer, -1);
    return_if(NULL == filename, -1);

    switch (format)
    {
        case IMAGE_FORMAT_PPM:
            return save_image_ppm(filename, rgb_buffer, width, height);
#ifdef HAVE_JPEG
        case IMAGE_FORMAT_JPEG:
            return save_image_jpeg(filename, rgb_buffer, width, height);
#endif
        default:
            fprintf(stderr, "Unsupported image format %d\n", format);
            return -1;
    }

    return 0;
}

char *
image_get_suffix(enum ImageFormat image_format)
{
    switch (image_format)
    {
        case IMAGE_FORMAT_PPM:
            return "ppm";
#ifdef HAVE_JPEG
        case IMAGE_FORMAT_JPEG:
            return "jpg";
#endif
        default:
            return "bin";
    }
}

enum ImageFormat
image_get_format(const char* format)
{
    if (!strcmp(format, "ppm"))
    {
        return IMAGE_FORMAT_PPM;
    }
#ifdef HAVE_JPEG
    else if (!strcmp(format, "jpg"))
    {
        return IMAGE_FORMAT_JPEG;
    }
#endif

    return -1;
}

char *
image_get_supported_string()
{
#ifdef HAVE_JPEG
    return "ppm|jpg";
#else
    return "ppm";
#endif

}
