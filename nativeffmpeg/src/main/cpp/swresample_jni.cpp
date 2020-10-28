#include <jni.h>
#include <string>



extern "C" {
//因为ffmpeg是一个纯C语言编写的库，使用C++调用C代码时候，需要使用“extern "C"”包围头文件
#include <libswresample/swresample.h>

JNIEXPORT jlong JNICALL
Java_io_github_zy3274311_nativeffmpeg_Swresample_initSwr(JNIEnv *env, jobject thiz) {
    // TODO: implement initSwr()
    SwrContext *swrContext = swr_alloc();

    swr_alloc_set_opts(swrContext,  // we're allocating a new context
            AV_CH_LAYOUT_STEREO,  // out_ch_layout
            AV_SAMPLE_FMT_S16,    // out_sample_fmt
            44100,                // out_sample_rate
            AV_CH_LAYOUT_5POINT1, // in_ch_layout
            AV_SAMPLE_FMT_FLTP,   // in_sample_fmt
            48000,                // in_sample_rate
            0,                    // log_offset
            NULL);                // log_ctx
    swr_init(swrContext);
    return reinterpret_cast<jlong>(swrContext);
}

JNIEXPORT void JNICALL
Java_io_github_zy3274311_nativeffmpeg_Swresample_free(JNIEnv *env, jobject thiz, jlong pr) {
    // TODO: implement free()
    auto *swrContext = reinterpret_cast<SwrContext *>(pr);
    swr_free(&swrContext);
}

}

