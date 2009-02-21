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

#ifndef __IMAGE_H
#define __IMAGE_H

#include <stdint.h>

enum ImageFormat
{
    /**
     * PPM format; used in #image_save to create a frame dump in PPM format
     * PPM is always available as it does not depend on external libraries
     * text representation is <tt>ppm</tt>
     */
    IMAGE_FORMAT_PPM = 0,
#ifdef HAVE_JPEG
    /**
     * JPEG format; used in #image_save to create a frame dump in JPEG. This
     * is only available if libjpeg was available at build time.
     * text representation is <tt>jpg</tt>
     */
    IMAGE_FORMAT_JPEG,
#endif
    /**
     * Number of supported image formats
     */
    IMAGE_FORMAT_COUNT
};

int
image_save(const char* filename, uint8_t* rgb_buffer, int width, int height, enum ImageFormat format);

/**
 * Map a member of #ImageFormat to a file suffix
 * 
 * @param image_format a member of #ImageFormat
 * @return a file suffix to use without the leading "."
 * \ingroup image
 */
char *
image_get_suffix(enum ImageFormat image_format);

/**
 * Translate a textual image format representation to an
 * member of #ImageFormat
 *
 * @param format textual representation for image format, @see #ImageFormat
 * \ingroup image
 */
enum ImageFormat
image_get_format(const char* format);

/**
 * Get a textual representation of the supported image formats
 *
 * @return pipe-separated list of image formats
 * \ingroup image
 **/
char *
image_get_supported_string();
#endif /* __IMAGE_H */
