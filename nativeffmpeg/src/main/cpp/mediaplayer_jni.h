//
// Created by zhangying62 on 2020/11/5.
//

#ifndef FFMPEGFORANDROID_MEDIAPLAYER_JNI_H
#define FFMPEGFORANDROID_MEDIAPLAYER_JNI_H


#include <libavformat/avformat.h>

JNIEXPORT void JNICALL native_play(JNIEnv *env, jobject thiz, jstring url);
JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject thiz);

struct MediaPlayer {
    AVFormatContext *fmt_ctx;
    AVCodecContext *decoder_ctx;
};


#endif //FFMPEGFORANDROID_MEDIAPLAYER_JNI_H
