#include <jni.h>
#include <string>



extern "C" {
//因为ffmpeg是一个纯C语言编写的库，使用C++调用C代码时候，需要使用“extern "C"”包围头文件
#include <libswresample/swresample.h>

JNIEXPORT jlong JNICALL
Java_io_github_zy3274311_nativeffmpeg_Swresample_initSwr(JNIEnv *env, jobject thiz) {
    // TODO: implement initSwr()
    SwrContext *swrContext = swr_alloc();
//    swr_alloc_set_opts(swrContext);
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

