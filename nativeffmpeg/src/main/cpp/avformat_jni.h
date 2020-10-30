//
// Created by zhangying62 on 2020/10/30.
//
#ifndef FFMPEGFORANDROID_AVFORMAT_JNI_H
#define FFMPEGFORANDROID_AVFORMAT_JNI_H

JNIEXPORT jint JNICALL native_getAvFormatVersion(JNIEnv *env, jobject thiz);

JNIEXPORT jstring JNICALL native_configuration(JNIEnv *env, jobject thiz);

JNIEXPORT jstring JNICALL native_license(JNIEnv *env, jobject thiz);


#endif //FFMPEGFORANDROID_AVFORMAT_JNI_H
