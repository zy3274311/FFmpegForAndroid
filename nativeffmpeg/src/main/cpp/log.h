//
// Created by zhangying62 on 2020/10/30.
//
#ifndef FFMPEGFORANDROID_LOG_H
#define FFMPEGFORANDROID_LOG_H

#include <android/log.h>

#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)



#endif //FFMPEGFORANDROID_LOG_H
