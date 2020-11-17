//
// Created by zhangying62 on 2020/11/14.
//

#include "player.h"
#include "ndk_log.h"
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>

static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                              enum AVMediaType type) {
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream ret:%s", errorStr);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

void player::init() {
    LOGE("FFmpeg", "FFMediaPlayer init");
    fmt_ctx = avformat_alloc_context();
    status = INIT;
}

void player::setDataSource(const char *url_) {
    url = url_;
}

void player::play() {
    LOGE("FFmpeg", "FFMediaPlayer play url:%s", url);
    status = START;
    AVFrame *frame = nullptr;
    AVPacket pkt;
    uint8_t *video_dst_data[4];
    int video_dst_linesize[4];
    int video_stream_idx;
    int audio_stream_idx;
    int ret;
    int video_dst_bufsize;

    ret = avformat_open_input(&fmt_ctx, url, nullptr, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_open_input ret:%s", errorStr);
        goto end;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_find_stream_info ret:%s", errorStr);
        goto end;
    }

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(
                video_dst_data,
                video_dst_linesize,
                video_dec_ctx->width,
                video_dec_ctx->height,
                video_dec_ctx->pix_fmt,
                1);
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "FFMediaPlayer avformat_find_stream_info ret:%s", errorStr);
            goto end;
        }
        video_dst_bufsize = ret;
    } else {
        //TODO open error
        goto end;
    }
    if (open_codec_context(&audio_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
    } else {
        //TODO open error
        goto end;
    }

    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, url, 0);

    if (!audio_stream && !video_stream) {
        LOGE("FFmpeg", "FFMediaPlayer (!audio_stream && !video_stream");
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGE("FFmpeg", "Could not allocate frame");
        goto end;
    }

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    while (status == PlayerStatus::START) {
        int byteCount = av_read_frame(fmt_ctx, &pkt);
        LOGE("FFmpeg", "FFMediaPlayer av_read_frame byteCount:%d",byteCount);
        if (byteCount < 0) {
            break;
        }

        av_packet_unref(&pkt);
    }


    LOGE("FFmpeg", "FFMediaPlayer play succuss");
    end:
    if (video_dec_ctx) {
        avcodec_free_context(&video_dec_ctx);
    }
    if (audio_dec_ctx) {
        avcodec_free_context(&audio_dec_ctx);
    }
    if (frame) {
        av_frame_free(&frame);
    }
//    av_free(video_dst_data);
    LOGE("FFmpeg", "FFMediaPlayer play end");


}

void player::stop() {
    LOGE("FFmpeg", "FFMediaPlayer stop url:%s", url);

    status = STOPED;
}

void player::release() {
    LOGE("FFmpeg", "FFMediaPlayer release url:%s", url);
    avformat_close_input(&fmt_ctx);
    status = RELEASE;
}

player::player() {
    LOGE("FFmpeg", "FFMediaPlayer player()");
}

player::~player() {
    url = nullptr;
    LOGE("FFmpeg", "FFMediaPlayer ~player()");
}
