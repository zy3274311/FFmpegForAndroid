//
// Created by zhangying62 on 2020/11/5.
//

#include <jni.h>
#include "ndk_log.h"
#include "player.h"

extern "C" {
#include <libavutil/log.h>

void ff_av_log(void *ptr, int level, const char *fmt, va_list vl) {
//    VLOGE("FFmpeg", fmt, vl);
}

JNIEXPORT jlong JNICALL native_init(JNIEnv *env, jobject thiz) {
    av_log_set_callback(ff_av_log);
    auto *p = new player();
    p->init();
    long ptr = reinterpret_cast<long>(p);
    jlong a = ptr;
    LOGE("FFmpeg", "native_init ptr:%ld", ptr);
    return a;
}

JNIEXPORT void JNICALL native_setDataSource(JNIEnv *env, jobject thiz, jlong ptr, jstring url_) {
    LOGE("FFmpeg", "native_setDataSource ptr:%ld", ptr);
    jboolean isCopy = JNI_TRUE;
    const char *url = env->GetStringUTFChars(url_, &isCopy);
    auto *p = reinterpret_cast<player *>(ptr);
    p->setDataSource(url);
    LOGE("FFmpeg", "native_setDataSource url:%s", url);
}


JNIEXPORT void JNICALL native_play(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "native_play ptr:%ld", ptr);
    auto *p = reinterpret_cast<player *>(ptr);
    p->play();
}


JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "native_stop ptr:%ld", ptr);
    auto *p = reinterpret_cast<player *>(ptr);
    p->stop();
}


JNIEXPORT void JNICALL native_release(JNIEnv *env, jobject thiz, jlong ptr) {
    LOGE("FFmpeg", "native_release ptr:%ld", ptr);
    auto *p = reinterpret_cast<player *>(ptr);
    p->release();
    delete p;
    av_log_set_callback(nullptr);
}

}