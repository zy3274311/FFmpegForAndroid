//
// Created by zhangying62 on 2020/11/2.
//

#ifndef FFMPEGFORANDROID_AVUTIL_JNI_H
#define FFMPEGFORANDROID_AVUTIL_JNI_H

JNIEXPORT jint JNICALL native_getChannelLayout(JNIEnv *env, jclass thiz, jint channelCount);

JNIEXPORT jint JNICALL
native_getSampleFormat(JNIEnv *env, jclass clazz, jint sizeByBit);

#endif //FFMPEGFORANDROID_AVUTIL_JNI_H
