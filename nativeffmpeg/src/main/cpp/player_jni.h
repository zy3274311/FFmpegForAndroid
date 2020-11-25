//
// Created by zhangying62 on 2020/11/5.
//

#ifndef FFMPEGFORANDROID_PLAYER_JNI_H
#define FFMPEGFORANDROID_PLAYER_JNI_H

JNIEXPORT jlong JNICALL native_init(JNIEnv *env, jobject thiz);
JNIEXPORT void JNICALL native_setSurface(JNIEnv *env, jobject thiz, jlong ptr, jobject surface);
JNIEXPORT void JNICALL native_setDataSource(JNIEnv *env, jobject thiz, jlong ptr, jstring url_);
JNIEXPORT void JNICALL native_play(JNIEnv *env, jobject thiz, jlong ptr);
JNIEXPORT void JNICALL native_stop(JNIEnv *env, jobject thiz, jlong ptr);
JNIEXPORT void JNICALL native_release(JNIEnv *env, jobject thiz, jlong ptr);

#endif //FFMPEGFORANDROID_PLAYER_JNI_H
