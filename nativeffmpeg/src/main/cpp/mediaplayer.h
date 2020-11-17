//
// Created by zhangying62 on 2020/11/12.
//

#ifndef FFMPEGFORANDROID_MEDIAPLAYER_H
#define FFMPEGFORANDROID_MEDIAPLAYER_H

#include <libavformat/avformat.h>

namespace MediaPlayer{
long init();

void setDataSource(long i, const char *string);

void play(long ptr);

void stop(long ptr);

void release(long ptr);
}
#endif //FFMPEGFORANDROID_MEDIAPLAYER_H
