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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_CAIRO
#   include <cairo.h>
#endif

#include "histogram.h"
#include "util.h"

int
histogram_create_from_rgb(uint8_t* buffer, int width, int height, struct Histogram* histogram)
{
    int line; 
    int column;
    uint8_t r,g,b,y;
    int offset;

    return_if(histogram == NULL, -1);

    memset(histogram, 0, sizeof(struct Histogram));

    histogram->total_pixel = width * height;

    for (line = 0; line < height; line++)
    {
        for (column = 0; column < width * 3; column += 3)
        {
            offset = width * 3 * line;
            r = buffer[offset + column];
            g = buffer[offset + column + 1];
            b = buffer[offset + column + 2];

            /* calculating luma value from RGB data */
            y = (uint8_t)(0.299 * r + 0.587 * g + 0.144 * b);
            histogram->data[y]++;
            if (histogram->data[y] > histogram->max)
            {
                histogram->max = histogram->data[y];
            }
        }
    }

    return 0;
}

int
histogram_save(struct Histogram* histogram, const char* file_name)
{
    FILE* f = NULL;
    int i;

    return_if(histogram == NULL, -1);
    return_if(file_name == NULL, -1);

    f = fopen(file_name, "w");
    if (!f)
    {
        fprintf(stderr, "Failed to open file %s:%d\n", file_name, errno);
        return -1;
    }

    for (i = 0; i < 256; i++)
    {
        fprintf(f, "%d\t%d\n", i, histogram->data[i]);
    }

    fclose(f);

    return 0;
}

int
histogram_heuristically_black(struct Histogram* histogram)
{
    int i;
    uint32_t dark_pixel_count = 0;

    return_if (histogram == NULL, 0);

    for (i = 0; i < 15; i++)
    {
        dark_pixel_count += histogram->data[i];
    }

    return dark_pixel_count >= histogram->total_pixel / 2;
}

int
histogram_render(struct Histogram* histogram, const char* filename)
{
#ifdef HAVE_CAIRO
    cairo_surface_t *surface;
    cairo_t *cr;
    int i;
    float line_width = 1.0 / 256.0;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 500, 400);
    cr = cairo_create(surface);
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_scale(cr, 500, 400);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, line_width);

    cairo_move_to(cr, 0.0, 1.0);
    for (i = 0; i < 256; ++i)
    {
        cairo_line_to(cr, i * line_width, 1.0 - 
                (double)histogram->data[i] / (double)histogram->max);
    }
    cairo_line_to(cr, 1.0, 1.0);
    cairo_close_path(cr);
    cairo_fill(cr);

    cairo_surface_write_to_png(surface, 
            filename);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
#endif
    return 0;
}
