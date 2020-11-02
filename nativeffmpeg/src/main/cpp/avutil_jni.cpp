#include <jni.h>
#include <string>

extern "C" {
//因为ffmpeg是一个纯C语言编写的库，使用C++调用C代码时候，需要使用“extern "C"”包围头文件
#include <libavutil/channel_layout.h>

JNIEXPORT jint JNICALL
native_getChannelLayout(JNIEnv *env,
                        jclass thiz,
                        jint channelCount) {
    // TODO: implement getChannelLayoutByName()
    switch (channelCount) {
        case 1:
            return AV_CH_LAYOUT_MONO;
        case 2:
            return AV_CH_LAYOUT_STEREO;
        default:
            break;
    }
    return -1;
}
}


extern "C" {
#include <libavutil/samplefmt.h>

JNIEXPORT jint JNICALL
native_getSampleFormat(JNIEnv *env, jclass clazz, jint sizeByBit) {
    // TODO: implement getSampleFormat()
    switch (sizeByBit) {
        case 8:
            return AV_SAMPLE_FMT_U8;
        case 16:
            return AV_SAMPLE_FMT_S16;
        case 32:
            return AV_SAMPLE_FMT_S32;
        default:
            return AV_SAMPLE_FMT_NONE;
    }

}
}