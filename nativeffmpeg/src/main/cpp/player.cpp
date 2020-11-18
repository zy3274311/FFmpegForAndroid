//
// Created by zhangying62 on 2020/11/14.
//

#include "player.h"
#include "ndk_log.h"
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <pthread.h>

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
            LOGE("FFmpeg", "FFMediaPlayer Failed to find %s codec", av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            LOGE("FFmpeg", "FFMediaPlayer Failed to allocate the %s codec context",
                 av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            LOGE("FFmpeg", "FFMediaPlayer Failed to copy %s codec parameters to decoder context",
                 av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            LOGE("FFmpeg", "Failed to open %s codec", av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}

static int decode_packet(AVCodecContext *dec, const AVPacket *pkt, AVFrame *frame) {
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        LOGE("FFmpeg", "Error submitting a packet for decoding (%s)", av_err2str(ret));
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame);
        LOGE("FFmpeg", "FFMediaPlayer avcodec_receive_frame ret:%d", ret);
        if (ret < 0) {
            LOGE("FFmpeg", "Error during decoding (%s)", av_err2str(ret));
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;
            return ret;
        }
        LOGE("FFmpeg", "FFMediaPlayer AVFrame pkt_size:%d", frame->pkt_size);
        av_frame_unref(frame);
    }

    return 0;
}

void player::init() {
    LOGE("FFmpeg", "FFMediaPlayer init");
    status = INIT;
}

void player::setDataSource(const char *url_) {
    url = url_;
}

void *pthread_run(void *arg) {
    auto *p = reinterpret_cast<player *>(arg);
    LOGE("FFmpeg", "FFMediaPlayer pthread_run play_tid:%ld", p->play_tid);
    AVFrame *frame = nullptr;
    AVPacket pkt;
    uint8_t *video_dst_data[4];
    int video_dst_linesize[4];
    int video_stream_idx;
    int audio_stream_idx;
    int ret;
    int video_dst_bufsize;

    AVCodecContext *video_dec_ctx = nullptr;
    AVCodecContext *audio_dec_ctx = nullptr;
    AVStream *video_stream = nullptr;
    AVStream *audio_stream = nullptr;
    AVFormatContext *fmt_ctx = nullptr;

    time_t t;
    time(&t);
    LOGE("FFmpeg", "FFMediaPlayer pthread_run time_t1:%ld", t);
    fmt_ctx = avformat_alloc_context();
    ret = avformat_open_input(&fmt_ctx, p->url, nullptr, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_open_input ret:%s", errorStr);
        goto end;
    }

    time(&t);
    LOGE("FFmpeg", "FFMediaPlayer avformat_open_input time_t2:%ld", t);
    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_find_stream_info ret:%s", errorStr);
        goto end;
    }
    time(&t);
    LOGE("FFmpeg", "FFMediaPlayer avformat_find_stream_info time_t3:%ld", t);
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
            LOGE("FFmpeg", "FFMediaPlayer open_codec_context ret:%s", errorStr);
            goto end;
        }
        video_dst_bufsize = ret;
        LOGE("FFmpeg", "FFMediaPlayer pthread_run video_dst_bufsize:%d", video_dst_bufsize);
    } else {
        //TODO open error
        goto end;
    }
    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
    } else {
        //TODO open error
        goto end;
    }
    time(&t);
    LOGE("FFmpeg", "FFMediaPlayer open_codec_context time_t4:%ld", t);
    /* dump input information to stderr */
//    av_dump_format(fmt_ctx, 0, p->url, 0);

    if (!audio_stream && !video_stream) {
        LOGE("FFmpeg", "FFMediaPlayer (!audio_stream && !video_stream");
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        LOGE("FFmpeg", "Could not allocate frame");
        goto end;
    }
    LOGE("FFmpeg", "FFMediaPlayer video_stream_idx:%d", video_stream_idx);
    LOGE("FFmpeg", "FFMediaPlayer audio_stream_idx:%d", audio_stream_idx);

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    while (p->status == PlayerStatus::START) {
        ret = av_read_frame(fmt_ctx, &pkt);
        LOGE("FFmpeg", "FFMediaPlayer av_read_frame ret:%d", ret);
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "FFMediaPlayer av_read_frame errorStr:%s", errorStr);
            break;
        }
        if (pkt.stream_index == video_stream_idx) {
            ret = decode_packet(video_dec_ctx, &pkt, frame);
        }
        if (pkt.stream_index == audio_stream_idx) {
            ret = decode_packet(audio_dec_ctx, &pkt, frame);
        }
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "FFMediaPlayer decode_packet errorStr:%s", errorStr);
            break;
        }

        av_packet_unref(&pkt);
    }


    LOGE("FFmpeg", "FFMediaPlayer play succuss");
    end:
    //释放音视频解码器
    if (video_dec_ctx) {
        avcodec_free_context(&video_dec_ctx);
    }
    if (audio_dec_ctx) {
        avcodec_free_context(&audio_dec_ctx);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    avformat_close_input(&fmt_ctx);
//    av_free(video_dst_data);
    LOGE("FFmpeg", "FFMediaPlayer play end");
    return nullptr;
}

void player::play() {
    LOGE("FFmpeg", "FFMediaPlayer play url:%s", url);
    status = START;
    int ret;
    ret = pthread_create(&play_tid, nullptr, pthread_run, this);
    LOGE("FFmpeg", "FFMediaPlayer native_play ret:%d", ret);
}

void player::stop() {
    LOGE("FFmpeg", "FFMediaPlayer stop url:%s", url);
    status = STOPED;
}

void player::release() {
    LOGE("FFmpeg", "FFMediaPlayer release url:%s", url);
    status = RELEASE;
}

player::player() {
    LOGE("FFmpeg", "FFMediaPlayer player()");
}

player::~player() {
    url = nullptr;
    LOGE("FFmpeg", "FFMediaPlayer ~player()");
}
