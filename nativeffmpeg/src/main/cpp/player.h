//
// Created by zhangying62 on 2020/11/14.
//

#ifndef FFMPEGFORANDROID_PLAYER_H
#define FFMPEGFORANDROID_PLAYER_H
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
enum PlayerStatus {
    INIT, PREPARE, START, RESUME, PAUSE, STOPED, RELEASE
};

class player {
private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *video_dec_ctx = nullptr;
    AVCodecContext *audio_dec_ctx = nullptr;
    PlayerStatus status;
    AVStream *video_stream = nullptr;
    AVStream *audio_stream = nullptr;
public:
    const char *url = nullptr;

    player();

    ~player();

    void init();

    void setDataSource(const char *url_);

    void play();

    void stop();

    void release();
};

}
#endif //FFMPEGFORANDROID_PLAYER_H
