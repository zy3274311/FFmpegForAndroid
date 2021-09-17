//
// Created by zhangying62 on 2021/6/9.
//
#include <jni.h>
extern "C" {
namespace extractor {
    JNIEXPORT void JNICALL _setup(JNIEnv *env, jobject thiz);

    JNIEXPORT void JNICALL
    _setDataSource(JNIEnv *env, jobject thiz, jstring _path, jarray _keys, jarray _values);

    JNIEXPORT void JNICALL _release(JNIEnv *env, jobject thiz);
}
}