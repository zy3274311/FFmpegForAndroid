//
// Created by zhangying62 on 2020/11/14.
//

#ifndef FFMPEGFORANDROID_PLAYER_H
#define FFMPEGFORANDROID_PLAYER_H
#include <queue>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
enum PlayerStatus {
    INIT, PREPARE, START, RESUME, PAUSE, STOPED, RELEASE
};

class player {
public:
    std::deque<AVFrame> avframes;
    PlayerStatus status;
    const char *url = nullptr;
    pthread_t* tid_demuxer;
    pthread_mutex_t mutex_t_demuxer;
    pthread_cond_t cond_t_demuxer;

    pthread_t* tid_sdl;
    pthread_mutex_t mutex_t_sdl;
    pthread_cond_t cond_t_sdl;

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
