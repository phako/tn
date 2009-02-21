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

#ifndef __VIDEO_H
#define __VIDEO_H

#ifdef AVCODEC_NEW_INCLUDE
#   include <libavcodec/avcodec.h>
#   include <libavformat/avformat.h>
#   include <libswscale/swscale.h>
#else
#   include <avcodec.h>
#   include <avformat.h>
#   include <swscale.h>
#endif

#include "image.h"
#include "histogram.h"

struct VideoFile
{
    AVFormatContext* format_ctx;
    AVCodecContext* codec_ctx;
    AVCodec* codec;
    AVStream *video_stream;
    struct SwsContext* scale_ctx;
    int video_stream_idx;
    int width;
    int height;
    int64_t pts;
    AVFrame *frame;
    AVFrame *frame_rgb;
    uint8_t* rgb_buffer;
    struct Histogram histogram;
};

struct VideoFile* 
video_file_open(const char* filename);

int 
video_file_close(struct VideoFile* video_file);

int
video_file_decode_frame(struct VideoFile* video_file);

int
video_file_decode_until_non_black(struct VideoFile* video_file);

int
video_file_save_frame(struct VideoFile* video_file, 
        const char* file_name, enum ImageFormat image_format);

int
video_file_seek_frame(struct VideoFile* video_file, uint64_t frame, int slow_seek);
#endif /* __VIDEO_H */
