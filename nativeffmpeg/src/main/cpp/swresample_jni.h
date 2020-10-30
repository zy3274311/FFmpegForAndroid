//
// Created by zhangying62 on 2020/10/29.
//

#ifndef FFMPEGFORANDROID_SWRESAMPLE_JNI_H
#define FFMPEGFORANDROID_SWRESAMPLE_JNI_H

JNIEXPORT jlong JNICALL
native_initSwr(
        JNIEnv *env, jobject thiz,
        jint out_ch_layout_, jint out_sample_fmt_,
        jint out_sample_rate_, jint in_ch_layout_,
        jint in_sample_fmt_, jint in_sample_rate_);

JNIEXPORT void JNICALL
native_freeSwr(
        JNIEnv *env, jobject thiz, jlong pr);

#endif //FFMPEGFORANDROID_SWRESAMPLE_JNI_H
