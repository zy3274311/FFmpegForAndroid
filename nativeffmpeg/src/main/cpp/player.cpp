//
// Created by zhangying62 on 2020/11/14.
//

#include "player.h"
#include "ndk_log.h"
#include "android/glenv.h"
#include "android/glrender.h"
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
        LOGE("FFmpeg", "av_find_best_stream ret:%s", errorStr);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            LOGE("FFmpeg", "Failed to find %s codec", av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            LOGE("FFmpeg", "Failed to allocate the %s codec context",
                 av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            LOGE("FFmpeg", "Failed to copy %s codec parameters to decoder context",
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

static int
decode_packet(AVCodecContext *dec, const AVPacket *pkt, player *p) {
    int ret = 0;
    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        LOGE("FFmpeg", "Error submitting a packet for decoding (%s)", av_err2str(ret));
        return ret;
    }
    // get all the available frames from the decoder
    while (ret >= 0) {
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0) {
            LOGE("FFmpeg", "Error during decoding (%s)", av_err2str(ret));
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                av_frame_free(&frame);
                return 0;
            }
            av_frame_free(&frame);
            return ret;
        }
        LOGE("FFmpeg", "AVFrame pkt_size:%d", frame->pkt_size);
        pthread_mutex_trylock(&p->avframes_demuxer);
        p->vframes.push_back(frame);
        pthread_mutex_trylock(&p->avframes_demuxer);
    }
    return 0;
}

void player::init() {
    LOGE("FFmpeg", "init");
    LOGE("FFmpeg", "init vframes:%d", vframes.size());
    status = INIT;
}

void player::setDataSource(const char *url_) {
    url = url_;
}

void *pthread_run_demuxer(void *arg) {
    auto *p = reinterpret_cast<player *>(arg);
    LOGE("FFmpeg", "pthread_run_demuxer tid_demuxer:%ld", p->tid_demuxer);
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
    fmt_ctx = avformat_alloc_context();
    ret = avformat_open_input(&fmt_ctx, p->url, nullptr, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "avformat_open_input ret:%s", errorStr);
        goto end;
    }
    ret = avformat_find_stream_info(fmt_ctx, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "avformat_find_stream_info ret:%s", errorStr);
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
            LOGE("FFmpeg", "open_codec_context ret:%s", errorStr);
            goto end;
        }
        video_dst_bufsize = ret;
        LOGE("FFmpeg", "pthread_run_demuxer video_dst_bufsize:%d", video_dst_bufsize);
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
    /* dump input information to stderr */
//    av_dump_format(fmt_ctx, 0, p->url, 0);
    if (!audio_stream && !video_stream) {
        LOGE("FFmpeg", "(!audio_stream && !video_stream");
        goto end;
    }
    LOGE("FFmpeg", "video_stream_idx:%d", video_stream_idx);
    LOGE("FFmpeg", "audio_stream_idx:%d", audio_stream_idx);
    av_init_packet(&pkt);
    while (p->status == PlayerStatus::START) {
        ret = av_read_frame(fmt_ctx, &pkt);
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "av_read_frame errorStr:%s", errorStr);
            break;
        }
        if (pkt.stream_index == video_stream_idx) {
            ret = decode_packet(video_dec_ctx, &pkt, p);
        }
        if (pkt.stream_index == audio_stream_idx) {
//            ret = decode_packet(audio_dec_ctx, &pkt, p);
        }
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "decode_packet errorStr:%s", errorStr);
            break;
        }
        pthread_cond_signal(&p->cond_t_sdl);
        int frameSize = p->vframes.size();
        if (frameSize >= 30) {
            pthread_cond_wait(&p->cond_t_demuxer, &p->mutex_t_demuxer);
        }
        LOGE("FFmpeg", "av_read_frame size:%d", frameSize);
        av_packet_unref(&pkt);
    }
    LOGE("FFmpeg", "play succuss");
    end:
    //释放音视频解码器
    if (video_dec_ctx) {
        avcodec_free_context(&video_dec_ctx);
    }
    if (audio_dec_ctx) {
        avcodec_free_context(&audio_dec_ctx);
    }
//    AVPacket *packet = &pkt;
//    av_packet_free(&packet);
    avformat_close_input(&fmt_ctx);
//    av_free(video_dst_data);
    LOGE("FFmpeg", "play end");
    return nullptr;
}

void *pthread_run_sdl(void *arg) {
    auto *p = reinterpret_cast<player *>(arg);

    glenv egl = glenv(p->window);
    egl.init();
    glrender render{};
    render.init();
    bool firstFrame = true;
    uint8_t *pointers[4];
    int linesizes[4];

    while (p->status == PlayerStatus::START) {
        int frameSize = p->vframes.size();
        if (frameSize > 0) {
            pthread_mutex_trylock(&p->avframes_demuxer);
            AVFrame *frame = p->vframes.front();
            p->vframes.pop_front();
            LOGE("FFmpeg", "pthread_run_sdl AVFrame format:%d", frame->format);
            pthread_mutex_unlock(&p->avframes_demuxer);
            pthread_cond_signal(&p->cond_t_demuxer);
            if(firstFrame){
                firstFrame = false;
                av_image_alloc(pointers, linesizes, frame->width, frame->height, AV_PIX_FMT_RGB24, 1);
            }
            glClearColor(1.0f,1.0f,0,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            render.render(frame->width, frame->height, frame->data);
            egl.swap();
            av_frame_free(&frame);
        } else {
            pthread_cond_wait(&p->cond_t_sdl, &p->mutex_t_sdl);
        }
    }
    render.free();
    egl.free();
    LOGE("FFmpeg", "pthread_run_sdl end");
    return nullptr;
}

void player::play() {
    LOGE("FFmpeg", "play url:%s", url);
    status = START;
    int ret;
    ret = pthread_create(&tid_demuxer, nullptr, pthread_run_demuxer, this);
    LOGE("FFmpeg", "pthread_create tid_demuxer ret:%d", ret);

    ret = pthread_create(&tid_sdl, nullptr, pthread_run_sdl, this);
    LOGE("FFmpeg", "pthread_create tid_sdl ret:%d", ret);
}

void player::stop() {
    LOGE("FFmpeg", "stop url:%s", url);
    status = STOPED;
    pthread_cond_signal(&cond_t_demuxer);
    pthread_cond_signal(&cond_t_sdl);
}

void player::release() {
    LOGE("FFmpeg", "release url:%s", url);
    LOGE("FFmpeg", "release vframes:%d", vframes.size());
    status = RELEASE;
    pthread_cond_signal(&cond_t_demuxer);
    pthread_cond_signal(&cond_t_sdl);
    pthread_join(tid_demuxer,nullptr);
    pthread_join(tid_sdl,nullptr);
    LOGE("FFmpeg", "release end");
}

player::player() {
    LOGE("FFmpeg", "player()");
}

player::~player() {
    url = nullptr;
    LOGE("FFmpeg", "~player()");
}

