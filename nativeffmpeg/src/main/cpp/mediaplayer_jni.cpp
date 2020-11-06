//
// Created by zhangying62 on 2020/11/5.
//

#include <jni.h>
#include "ndk_log.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/log.h>
#include "mediaplayer_jni.h"

static void av_log(void *ptr, int level, const char *fmt, va_list vl) {
//    VLOGE("FFmpeg", fmt, vl);
}

bool stop;

JNIEXPORT jlong JNICALL native_init(JNIEnv *env, jobject thiz) {
    LOGE("FFmpeg", "FFMediaPlayer native_init");
    auto *mediaPlayer = (MediaPlayer *) malloc(1);
    mediaPlayer->fmt_ctx = nullptr;
    mediaPlayer->decoder_ctx = nullptr;
    return (jlong) &mediaPlayer;
}


JNIEXPORT void JNICALL native_setDataSource(JNIEnv *env, jobject thiz, jlong ptr, jstring url_) {
    LOGE("FFmpeg", "FFMediaPlayer native_setDataSource");
    auto *player = (MediaPlayer *) ptr;
    LOGE("FFmpeg", "FFMediaPlayer native_setDataSource 2");
    jboolean isCopy = JNI_TRUE;
    player->url = env->GetStringUTFChars(url_, &isCopy);
    LOGE("FFmpeg", "FFMediaPlayer native_setDataSource url:%s", player->url);
}

JNIEXPORT void JNICALL native_play(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "FFMediaPlayer native_play");
    auto *player =(MediaPlayer *)ptr;

    av_log_set_callback(av_log);//set av_log callback
    // TODO: implement configuration()s
    stop = false;
    const char *url = player->url;
    int ret;
    AVPacket dec_pkt;
    AVFormatContext *fmt_ctx = player->fmt_ctx;
    AVCodecContext *decoder_ctx = player->decoder_ctx;
    AVCodec *decoder_ret;
    int video_track_index;
    int audio_track_index;

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

    video_track_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder_ret,0);
    if (video_track_index < 0) {
        char *errorStr = av_err2str(video_track_index);
        LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream video_track_index:%s", errorStr);
        goto end;
    }
    LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream video_track_index:%d", video_track_index);

    audio_track_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder_ret,0);
    if (audio_track_index < 0) {
        char *errorStr = av_err2str(audio_track_index);
        LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream audio_track_index:%s", errorStr);
        goto end;
    }
    LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream audio_track_index:%d", audio_track_index);

    while (!stop) {
        ret = av_read_frame(fmt_ctx, &dec_pkt);
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "FFMediaPlayer av_read_frame ret:%s", errorStr);
            break;
        }

        if(video_track_index==dec_pkt.stream_index) {
            //TODO 处理视频Packet
        }
        if(audio_track_index==dec_pkt.stream_index) {
            //TODO 处理音频Packet
        }

        LOGE("FFmpeg", "FFMediaPlayer stream_index:%d", dec_pkt.stream_index);

        av_packet_unref(&dec_pkt);
    }

    end:
    avformat_close_input(&fmt_ctx);

    LOGE("FFmpeg", "FFMediaPlayer native_loadUrl url:%s", url);
}


JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "FFMediaPlayer native_stop");
    auto *player =(MediaPlayer *)ptr;
    stop = true;
}


JNIEXPORT void JNICALL native_release(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "FFMediaPlayer native_release");
}

}