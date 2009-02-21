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
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "video.h"

static int
find_video_stream(AVFormatContext* ctx)
{
    int i;

    for (i = 0; i < ctx->nb_streams; ++i)
    {
        if (ctx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            return i;
        }
    }

    return -1;
}

static struct SwsContext*
make_scale_context(AVCodecContext* ctx)
{
    return sws_getContext(
            /* source parameters */
            ctx->width, ctx->height, ctx->pix_fmt,
            /* target parameters */
            ctx->width, ctx->height, PIX_FMT_RGB24,
            /* scale parameters */
            SWS_BICUBIC,
            /* dunno. need to look up */
            NULL, NULL, NULL);
}

static AVCodec*
create_codec(AVCodecContext* ctx)
{
    AVCodec* codec;

    codec = avcodec_find_decoder(ctx->codec_id);
    if (codec)
    {
        if (avcodec_open(ctx, codec) >= 0)
        {
            return codec;
        }
    }

    return NULL;
}

struct VideoFile*
video_file_open(const char* filename)
{
    struct VideoFile* video_file = NULL;

    return_if(NULL == filename, NULL);

    video_file = (struct VideoFile*)malloc(sizeof(struct VideoFile));

    if (video_file)
    {
        memset(video_file, 0, sizeof(struct VideoFile));
        if (av_open_input_file(&(video_file->format_ctx), filename, NULL, 0, NULL) == 0)
        {
            if (av_find_stream_info(video_file->format_ctx) >= 0)
            {   
                int idx;
                /* find video stream */
                idx = video_file->video_stream_idx = 
                    find_video_stream(video_file->format_ctx);

                if (idx >= 0)
                {
                    video_file->video_stream = 
                        video_file->format_ctx->streams[idx];
                    video_file->codec_ctx = 
                        video_file->format_ctx->streams[idx]->codec;

                    video_file->codec = create_codec(video_file->codec_ctx);
                    if (video_file->codec)
                    {
                        /* 
                         * create software scaler context for colorspace
                         * conversion
                         */
                        video_file->scale_ctx =
                            make_scale_context(video_file->codec_ctx);
                        if (video_file->scale_ctx)
                        {
                            video_file->width = 
                                video_file->codec_ctx->width;
                            video_file->height = 
                                video_file->codec_ctx->height;
                            video_file->frame = avcodec_alloc_frame();
                            if (video_file->frame)
                            {
                                video_file->frame_rgb = avcodec_alloc_frame();
                                if (video_file->frame_rgb)
                                {
                                    int bytes = avpicture_get_size(
                                            PIX_FMT_RGB24,
                                            video_file->width,
                                            video_file->height);
                                    video_file->rgb_buffer =
                                        (uint8_t *)av_malloc(bytes * sizeof(uint8_t));
                                    if (video_file->rgb_buffer)
                                    {
                                        avpicture_fill(
                                                (AVPicture *)video_file->frame_rgb, 
                                                video_file->rgb_buffer, 
                                                PIX_FMT_RGB24,
                                                video_file->width, 
                                                video_file->height);
                                        return video_file;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        video_file_close(video_file);
        video_file = NULL;
    }

    return video_file;
}

int 
video_file_close(struct VideoFile* video_file)
{
    return_if (NULL == video_file, -1);

    if (video_file->rgb_buffer != NULL)
    {
        av_free(video_file->rgb_buffer);
    }

    if (video_file->frame_rgb != NULL)
    {
        av_free(video_file->frame_rgb);
    }

    if (video_file->frame != NULL)
    {
        av_free(video_file->frame);
    }

    if (video_file->scale_ctx != NULL)
    {
        sws_freeContext(video_file->scale_ctx);
    }

    if (video_file->codec_ctx != NULL)
    {
        avcodec_close(video_file->codec_ctx);
    }

    if (video_file->format_ctx)
    {
        av_close_input_file(video_file->format_ctx);
    }

    free(video_file);

    return 0;
}

int
video_file_decode_frame(struct VideoFile* video_file)
{
    AVPacket packet;
    int frame_finished;

    return_if(video_file == NULL, -1);

    while (av_read_frame(video_file->format_ctx, &packet) >= 0)
    {
        if (packet.stream_index == video_file->video_stream_idx)
        {
            avcodec_decode_video(
                    video_file->codec_ctx, 
                    video_file->frame,
                    &frame_finished,
                    packet.data, packet.size);
            if (frame_finished)
            {
                video_file->pts = packet.dts;
                /* create rgb frame */
                sws_scale(
                        video_file->scale_ctx,
                        video_file->frame->data, 
                        video_file->frame->linesize, 0,
                        video_file->height, 
                        video_file->frame_rgb->data, 
                        video_file->frame_rgb->linesize);
                histogram_create_from_rgb(
                        video_file->frame_rgb->data[0],
                        video_file->width,
                        video_file->height,
                        &(video_file->histogram));
                av_free_packet(&packet);
                break;
            }
        }
        av_free_packet(&packet);
    }

    return 0;
}

int
video_file_decode_until_non_black(struct VideoFile* video_file)
{
    return_if(video_file == NULL, -1);

    do
    {
        video_file_decode_frame(video_file);
    } while (histogram_heuristically_black(&(video_file->histogram)));

    return 0;
}

int 
video_file_save_frame(struct VideoFile* video_file, 
        const char* filename, enum ImageFormat image_format)
{
    return_if(video_file == NULL, -1);
    return_if(filename == NULL, -1);
    return_if(image_format < 0 || image_format >= IMAGE_FORMAT_COUNT, -1);

    return image_save(filename, video_file->frame_rgb->data[0],
            video_file->width, video_file->height,
            image_format);
}

int
video_file_seek_frame(struct VideoFile* video_file, uint64_t frame, int slow_seek)
{
    return_if(video_file == NULL, -1);

    if (!slow_seek)
    {
        av_seek_frame(video_file->format_ctx, 
                video_file->video_stream_idx,
                frame,
                AVSEEK_FLAG_BACKWARD);
    }
    video_file->codec_ctx->hurry_up = 1;
    do
    {
        video_file_decode_frame(video_file);
        if (video_file->pts >= frame - 1)
        {
            printf("PTS: %lli, frame: %lli\n", video_file->pts, frame);
            break;
        }
    } while (1);
    video_file->codec_ctx->hurry_up = 0;

    return 0;
}
