#include <jni.h>
#include "log.h"

extern "C" {
//因为ffmpeg是一个纯C语言编写的库，使用C++调用C代码时候，需要使用“extern "C"”包围头文件
#include <libswresample/swresample.h>

JNIEXPORT jlong JNICALL
native_initSwr(
        JNIEnv *env, jobject thiz,
        jint out_ch_layout_, jint out_sample_fmt_,
        jint out_sample_rate_, jint in_ch_layout_,
        jint in_sample_fmt_, jint in_sample_rate_) {

    int64_t out_ch_layout = out_ch_layout_;
    auto out_sample_fmt = static_cast<AVSampleFormat>(out_sample_fmt_);
    int64_t in_ch_layout = in_ch_layout_;
    auto in_sample_fmt = static_cast<AVSampleFormat>(in_sample_fmt_);
    SwrContext *swrContext = swr_alloc_set_opts(NULL,  // we're allocating a new context
                       out_ch_layout,  // out_ch_layout
                       out_sample_fmt,    // out_sample_fmt
                       out_sample_rate_,                // out_sample_rate
                       in_ch_layout, // in_ch_layout
                       in_sample_fmt,   // in_sample_fmt
                       in_sample_rate_,                // in_sample_rate
                       0,                    // log_offset
                       NULL);                // log_ctx
    int code = swr_init(swrContext);
    LOGE("ffmpegforandroid", "swr_init:%d", code);

    return reinterpret_cast<jlong>(swrContext);
}

JNIEXPORT void JNICALL
native_freeSwr(
        JNIEnv *env, jobject thiz, jlong pr) {
    // TODO: implement free()
    auto *swrContext = reinterpret_cast<SwrContext *>(pr);
    swr_free(&swrContext);
}

}
