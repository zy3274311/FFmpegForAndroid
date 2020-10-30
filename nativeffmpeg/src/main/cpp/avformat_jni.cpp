//
// Created by zhangying62 on 2020/10/30.
//
#include <jni.h>


extern "C" {
#include <libavformat/avformat.h>

JNIEXPORT jint JNICALL native_getAvFormatVersion(JNIEnv *env, jobject thiz) {
    // TODO: implement getAvFormatVersion()
    return avformat_version();
}

JNIEXPORT jstring JNICALL native_configuration(JNIEnv *env, jobject thiz) {
    // TODO: implement configuration()
    const char * conf = avformat_configuration();
    return env->NewStringUTF(conf);
}

JNIEXPORT jstring JNICALL native_license(JNIEnv *env, jobject thiz) {
    // TODO: implement configuration()
    const char * conf = avformat_license();
    return env->NewStringUTF(conf);
}


}