#include <jni.h>
#include "log.h"


extern "C" {
#include "swresample_jni.h"
#include "avformat_jni.h"
#include "avutil_jni.h"

int Load_Swresample(JNIEnv *env);
int Load_AvFormat(JNIEnv *env);
int Load_AvUtil(JNIEnv *pEnv);

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    int rc = Load_Swresample(env);
    if (rc != JNI_OK) return rc;

    rc = Load_AvFormat(env);
    if (rc != JNI_OK) return rc;

    rc = Load_AvUtil(env);
    if (rc != JNI_OK) return rc;

    return JNI_VERSION_1_6;
}

int Load_Swresample(JNIEnv *env) {
    LOGE("FFmpeg", "Load_Swresample start");
// Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
    jclass c = env->FindClass("io/github/zy3274311/nativeffmpeg/swresample/Swresample");
    if (c == nullptr) return JNI_ERR;
// Register your class' native methods.
    static const JNINativeMethod methods[] = {
            {"initSwr", "(IIIIII)J", (jlong *) native_initSwr},
            {"free",    "(J)V",      (void *) native_freeSwr},
    };
    int rc = env->RegisterNatives(c, methods, sizeof(methods) / sizeof(JNINativeMethod));
    LOGE("FFmpeg", "Swresample RegisterNatives rc:%d", rc);
    return rc;
}

int Load_AvFormat(JNIEnv *env) {
    LOGE("FFmpeg", "Load_AvFormat start");
    // Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
    jclass c = env->FindClass("io/github/zy3274311/nativeffmpeg/avformat/AVFormat");
    if (c == nullptr) return JNI_ERR;

    // Register your class' native methods.
    static const JNINativeMethod methods[] = {
            {"getAvFormatVersion", "()I",                  reinterpret_cast<jint *>(native_getAvFormatVersion)},
            {"configuration",      "()Ljava/lang/String;", reinterpret_cast<jstring *>(native_configuration)},
            {"license",            "()Ljava/lang/String;", reinterpret_cast<jstring *>(native_license)},
    };

    int rc = env->RegisterNatives(c, methods, sizeof(methods) / sizeof(JNINativeMethod));
    LOGE("FFmpeg", "AVFormat RegisterNatives rc:%d", rc);
    return rc;
}

int Load_AvUtil(JNIEnv *env) {
    LOGE("FFmpeg", "Load_AvUtil start");
    jclass c = env->FindClass("io/github/zy3274311/nativeffmpeg/avutil/ChannelLayout");
    if (c == nullptr) return JNI_ERR;

// Register your class' native methods.
    static const JNINativeMethod methods[] = {
            {"getChannelLayout", "(I)I", reinterpret_cast<jint *>(native_getChannelLayout)},
    };

    int rc = env->RegisterNatives(c, methods, sizeof(methods) / sizeof(JNINativeMethod));
    LOGE("FFmpeg", "ChannelLayout RegisterNatives rc:%d", rc);
    if (rc != 0) {
        return JNI_ERR;
    }

    jclass SampleFmtC = env->FindClass("io/github/zy3274311/nativeffmpeg/avutil/SampleFmt");
    if (SampleFmtC == nullptr) return JNI_ERR;

// Register your class' native methods.
    static const JNINativeMethod SampleFmtMethods[] = {
            {"getSampleFormat", "(I)I", reinterpret_cast<jint *>(native_getSampleFormat)},
    };

    int SampleFmtRc = env->RegisterNatives(SampleFmtC, SampleFmtMethods,
                                           sizeof(SampleFmtMethods) / sizeof(JNINativeMethod));
    LOGE("FFmpeg", "SampleFmt RegisterNatives rc:%d", SampleFmtRc);
    return rc;
}

}


