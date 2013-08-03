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

/*!\mainpage
 * \section sec_intro Introduction
 * Tn is a program for automatically generating a number of screenshots from a
 * video file.
 *
 * Tn is meant to be used on headless displays. Its main features include:
 * \li Seeking for fast snapshot generation
 * \li Black frame detection
 * \li Output to multiple image formats
 * \section sec_install Installation
 */

/**
 * \defgroup video Video handling functions
 * \defgroup image Image handling functions
 * \defgroup analysis Image analysis functions
 */

/**
 * \addtogroup analysis
 *
 * This module contains functions for the creation and analysis of histograms.
 * These Historgrams are used for the feature of automatic black frame
 * recognition.
 *
 */

#ifdef AVCODEC_NEW_INCLUDE
#   include <libavformat/avformat.h>
#else
#   include <avformat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "histogram.h"
#include "util.h"
#include "video.h"

/** Flag to enable black frame detection heuristics; can be changed by
 * commandline parameter <tt>-b</tt>. Default is off */
int skip_black_frames = 0;

/** Flag to write histogram data to disk; can be changed by commandline
 * parameter <tt>-t</tt>. Default is off */
int write_histogram = 0;

/** Flag to use decoding instead of seeking. This is necessary for packed
 * bitstream files, MPEG1 and MPEG2. Can be changed by commandline parameter
 * <tt>-s</tt>. Default is off */
int slow_seek = 0;

/**
 * Target image format for snapshots. can be changed by using commandline
 * parameter <tt>-i</tt>. Default is PPM because it does not depend on
 * additional libraries
 */
enum ImageFormat image_format = IMAGE_FORMAT_PPM;
uint8_t num_pics = 32;
uint32_t offset = 0;

int main(int argc, char *argv[]) {
    int             i;
    int opt;
    uint64_t        step;
    struct VideoFile* video_file;

    /* Register all formats and codecs */
    av_register_all();

    LOG(INFO, "Tn version %s", VERSION);

    while ((opt = getopt(argc, argv, "bthso:i:n:")) != -1)
    {
        switch (opt)
        {
            case 't':
                write_histogram = 1;
                break;
            case 'i':
                image_format = image_get_format (optarg);
                break;
            case 'n':
                num_pics = atoi(optarg);
                if (num_pics == 0)
                {
                    LOG(WARNING, "%s", "Cannot create zero pictures");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'b':
                skip_black_frames = 1;
                break;
            case 'o':
                offset = atol(optarg);
                break;
            case 's':
                LOG(INFO, "%s", "Will use slow decoding mode, please be patient");
                slow_seek = 1;
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s [options] file\n\n", argv[0]);
                fprintf(stderr, "With options:\n");
                fprintf(stderr, "\t-h : This help\n");
                fprintf(stderr, "\t-b : Write histogram data files\n");
                fprintf(stderr, "\t-i %s: Select output image format\n", image_get_supported_string());
                fprintf(stderr, "\t-n <count>: Create count snapshots\n");
                fprintf(stderr, "\t-b Skip very dark frames\n");
                fprintf(stderr, "\t-o <NUM>: Start output file numbering at NUM\n");
                fprintf(stderr, "\t-s : Use slow seeking (for MPEG and packed bitstream files)\n");
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) 
    {
        LOG(ERROR, "%s", "Please provide a movie file");
        exit(EXIT_FAILURE);
    }

    video_file = video_file_open(argv[optind]);
    if (!video_file)
    {
        LOG(ERROR, "Error opening file %s", argv[optind]);
        exit(EXIT_FAILURE);
    }

    step = video_file->video_stream->duration / num_pics;

    /* Dump information about file onto standard error */
    av_dump_format(video_file->format_ctx, 0, argv[optind], 0);

    for (i = 0; i < num_pics; ++i)
    {
        char filename[64];
        sprintf(filename, "frame_%05d.%s", i + offset, image_get_suffix(image_format));

        if (skip_black_frames)
        {
            video_file_decode_until_non_black(video_file);
        }
        else
        {
            video_file_decode_frame(video_file);
        }
        video_file_save_frame(video_file, filename, image_format);

        if (write_histogram)
        {
            sprintf(filename, "histogram_%05d.dat", i);
            histogram_save(&(video_file->histogram), filename);
            sprintf(filename, "histogram_%05d.png", i);
            histogram_render(&(video_file->histogram), filename);
        }

        video_file_seek_frame(video_file, (i + 1) * step, slow_seek);
    }

    video_file_close(video_file);

    return 0;
}

