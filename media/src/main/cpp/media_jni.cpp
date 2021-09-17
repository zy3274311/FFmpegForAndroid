//
// Created by zhangying62 on 2021/6/8.
//
#include <jni.h>

extern "C" {
#include "extractor.h"

int Load_Media(JNIEnv *pEnv);

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    int rc = Load_Media(env);
    if (rc != JNI_OK) return rc;
    return JNI_VERSION_1_6;
}

int Load_Media(JNIEnv *env) {
    jclass c = env->FindClass("com/example/media/FFmpegMediaExtractor");
    if (c == nullptr) return JNI_ERR;

    using namespace extractor;
    // Register your class' native methods.
    static const JNINativeMethod methods[] = {
            {"native_setup",          "()V",                    (void *) (_setup)},
            {"native_setDataSource", "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V", (void *) (_setDataSource)},
            {"native_release",       "()V",                   (void *) (_release)}
    };

    int rc = env->RegisterNatives(c, methods, sizeof(methods) / sizeof(JNINativeMethod));
    return rc;
}

JNIEXPORT jint JNI_OnUnLoad(JavaVM *vm, void *reserved) {
    return JNI_VERSION_1_6;
}

}

