//
// Created by zhangying62 on 2020/11/5.
//

#ifndef FFMPEGFORANDROID_MEDIAPLAYER_JNI_H
#define FFMPEGFORANDROID_MEDIAPLAYER_JNI_H


#include <libavformat/avformat.h>

JNIEXPORT jlong JNICALL native_init(JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL native_setDataSource(JNIEnv *env, jobject thiz, jlong ptr, jstring url_);
JNIEXPORT void JNICALL native_play(JNIEnv *env, jobject thiz, jlong ptr);
JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject thiz, jlong ptr);
JNIEXPORT void JNICALL native_release(JNIEnv *env, jobject thiz, jlong ptr);

typedef struct MediaPlayer {
    AVFormatContext *fmt_ctx;
    AVCodecContext *decoder_ctx;
    const char *url;
};


#endif //FFMPEGFORANDROID_MEDIAPLAYER_JNI_H
