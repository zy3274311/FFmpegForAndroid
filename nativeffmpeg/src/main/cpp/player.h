//
// Created by zhangying62 on 2020/11/14.
//

#ifndef FFMPEGFORANDROID_PLAYER_H
#define FFMPEGFORANDROID_PLAYER_H
#include <queue>
#include <EGL/egl.h>
#include <EGL/eglext.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
enum PlayerStatus {
    INIT, PREPARE, START, RESUME, PAUSE, STOPED, RELEASE
};

class player {
public:
    ANativeWindow *window = nullptr;
    int width{};
    int height{};

    pthread_mutex_t mutex_t_avframes{};
    std::deque<AVFrame*> vframes;
    std::deque<AVFrame*> aframes;
    PlayerStatus status;
    const char *url = nullptr;

    pthread_t tid_demuxer{};
    pthread_mutex_t mutex_t_demuxer{};
    pthread_cond_t cond_t_demuxer{};

    pthread_t tid_sdl{};
    pthread_mutex_t mutex_t_sdl{};
    pthread_cond_t cond_t_sdl{};

    player();

    ~player();

    void setSurface(ANativeWindow *pWindow, int w, int h) {
        this->window = pWindow;
        this->width = w;
        this->height = h;
    }

    void setDataSource(const char *url_) {
        url = url_;
    }

    void play();

    void stop();

    void release();
};

}
#endif //FFMPEGFORANDROID_PLAYER_H
