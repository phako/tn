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


#ifndef __HISTOGRAM_H
#define __HISTOGRAM_H

#include <stdint.h>

/**
 * Struct which represents a histogram of a video frame.
 */
struct Histogram
{
    /** Y data distribution */
    uint32_t data[256];

    /** Number of pixels in image */
    uint32_t total_pixel;

    /** Peek of distribution */
    uint32_t max;
};

/**
 * Create a histogram from an RGB buffer. The buffer is assumed to contain
 * width * height * 3 elements in order RGB. The data is converted to Luma and
 * categorized afterwards.
 *
 * @param buffer input buffer
 * @param width of picture
 * @param height of picture
 * @param histogram pointer to a #Histogram. Any old data will be erased.
 * @return 0 on success
 * \ingroup analysis
 */
int
histogram_create_from_rgb(uint8_t* buffer, int width, int height, struct Histogram* histogram);

/**
 * Write histogram distribution to disk. This is a raw ascii file allowing
 * processing in other tools such as gnuplot. If the file already exists, it
 * will be overwritten.
 *
 * @param histogram pointer to a #Histogram containing the data
 * @param file_name name of a file to save to
 * @return 0 on success
 * \ingroup analysis
 */
int
histogram_save(struct Histogram* histogram, const char* file_name);

/**
 * Simple heuristics to check if a histogram is from a black or near-black
 * picture. The function assumes that if 50% of all pixels are in 15
 * lowest luma values, the picture is (near-)black
 *
 * @param histogram pointer to a #Histogram containing the data
 * @return 1, if histogram matches assumptions, 0 otherwise
 * \ingroup analysis
 */
int
histogram_heuristically_black(struct Histogram* histogram);

/**
 * Write a distribution image to disk. This is a plot of the histogram. In
 * order to have this working, the 
 * <a href="http://www.cairographics.org">cairo library</a> has to be
 * available. If the file exists, it will be overwritten
 *
 * @param histogram pointer to a #Histogram containing the data
 * @param filename name of a file to save to
 * @return 0 on success
 * \ingroup analysis
 */
int
histogram_render(struct Histogram* histogram, const char* filename);
#endif /* __HISTOGRAM_H */
