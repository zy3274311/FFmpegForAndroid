//
// Created by zhangying62 on 2020/10/30.
//
#ifndef FFMPEGFORANDROID_NDK_LOG_H
#define FFMPEGFORANDROID_NDK_LOG_H

#include <android/log.h>

#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define VLOGE(TAG, fmt, ap) __android_log_vprint(ANDROID_LOG_ERROR, TAG, fmt, ap)



#endif //FFMPEGFORANDROID_NDK_LOG_H
