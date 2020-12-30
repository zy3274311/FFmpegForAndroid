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
    int texture;
    struct SwsContext *sws_ctx;
    uint8_t *pixels[4];
    int pitch[4];

    while (p->status == PlayerStatus::START) {
        int frameSize = p->vframes.size();
        if (frameSize > 0) {
            pthread_mutex_trylock(&p->avframes_demuxer);
            AVFrame *frame = p->vframes.front();
            p->vframes.pop_front();
            LOGE("FFmpeg", "pthread_run_sdl AVFrame width:%d", frame->width);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame height:%d", frame->height);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame AV_PIX_FMT_YUV420P:%d", AV_PIX_FMT_YUV420P);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame format:%d", frame->format);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame linesize[0]:%d", frame->linesize[0]);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame linesize[1]:%d", frame->linesize[1]);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame linesize[2]:%d", frame->linesize[2]);
            LOGE("FFmpeg", "pthread_run_sdl AVFrame linesize[3]:%d", frame->linesize[3]);
            pthread_mutex_unlock(&p->avframes_demuxer);
            pthread_cond_signal(&p->cond_t_demuxer);
            //TODO YUV convert to RGB data
            int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, frame->width, frame->height, 1);
            auto* p_global_bgr_buffer = (uint8_t*) malloc(num_bytes * sizeof(uint8_t));
            uint8_t* bgr_buffer[4] = {p_global_bgr_buffer};
            int linesize[4] = { frame->width*4};
            if(firstFrame){
                firstFrame = false;
                sws_ctx = sws_getContext(
                        frame->width,
                        frame->height,
                        static_cast<AVPixelFormat>(frame->format),
                        frame->width,
                        frame->height,
                        AV_PIX_FMT_RGBA,
                        SWS_BICUBIC, nullptr, nullptr, nullptr);

                int ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, bgr_buffer, linesize);
//                int ret = sws_scale(sws_ctx, (const uint8_t * const *)frame->data, frame->linesize,
//                          0, frame->height, pixels, pitch);
                LOGE("FFmpeg", "sws_scale ret:%d", ret);
                texture = render.createTexture(frame->width, frame->height, bgr_buffer[0]);
            } else {
                int ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, bgr_buffer, linesize);
//                int ret = sws_scale(sws_ctx, (const uint8_t * const *)frame->data, frame->linesize,
//                          0, frame->height, pixels, pitch);
                LOGE("FFmpeg", "sws_scale ret:%d", ret);
                render.updateTexture(texture, frame->width, frame->height, bgr_buffer[0]);
            }
            LOGE("FFmpeg", "pthread_run_sdl texture:%d", texture);
            glViewport(0,0 , frame->width, frame->height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render.render(texture);
            egl.swap();
            av_frame_free(&frame);
        } else {
            pthread_cond_wait(&p->cond_t_sdl, &p->mutex_t_sdl);
        }
    }
    render.free();
    egl.free();
    if(sws_ctx){
        sws_freeContext(sws_ctx);
    }
    //TODO 是否纹理
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

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}


