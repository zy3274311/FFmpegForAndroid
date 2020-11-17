//
// Created by zhangying62 on 2020/11/12.
//
extern "C" {
#include "ndk_log.h"
#include "mediaplayer.h"
#include <libavformat/avformat.h>
#include <libavutil/error.h>

enum PlayerStatus {
    INIT, PREPARE, START, RESUME, PAUSE, STOPED, RELEASE
};

struct mediaplayer {
    const char *url;
    PlayerStatus status;
};

long MediaPlayer::init() {
    mediaplayer player{};
    player.status = PlayerStatus::INIT;
    long ptr = reinterpret_cast<long>(&player);
    LOGE("FFmpeg", "FFMediaPlayer init status:%d", player.status);
    return ptr;
}

void MediaPlayer::setDataSource(long ptr, const char *url) {
    auto *mediaplayer = reinterpret_cast<struct mediaplayer *>(ptr);
    mediaplayer->url = url;
    LOGE("FFmpeg", "FFMediaPlayer setDataSource url:%s", mediaplayer->url);
}

void MediaPlayer::play(long ptr) {
    auto *mediaplayer = reinterpret_cast<struct mediaplayer *>(ptr);
    AVPacket dec_pkt;
    AVCodec *decoder_ret;
    int video_track_index;
    int audio_track_index;
    int ret;
    AVCodecContext *decoder_ctx;
    AVFormatContext *fmt_ctx = avformat_alloc_context();

    LOGE("FFmpeg", "FFMediaPlayer play ptr:%ld", ptr);
    LOGE("FFmpeg", "FFMediaPlayer play url:%s", mediaplayer->url);
    LOGE("FFmpeg", "FFMediaPlayer play status:%d", mediaplayer->status);

    ret = avformat_open_input(&fmt_ctx, mediaplayer->url, nullptr, nullptr);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_open_input ret:%s", errorStr);
        goto end;
    }
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        char *errorStr = av_err2str(ret);
        LOGE("FFmpeg", "FFMediaPlayer avformat_find_stream_info ret:%s", errorStr);
        goto end;
    }

    video_track_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1,
                                            &decoder_ret, 0);
    if (video_track_index < 0) {
        char *errorStr = av_err2str(video_track_index);
        LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream video_track_index:%s", errorStr);
        goto end;
    }

    LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream video_track_index:%s", decoder_ret->name);

    audio_track_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1,
                                            &decoder_ret, 0);
    if (audio_track_index < 0) {
        char *errorStr = av_err2str(audio_track_index);
        LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream audio_track_index:%s", errorStr);
        goto end;
    }
    LOGE("FFmpeg", "FFMediaPlayer av_find_best_stream audio_track_index:%s", decoder_ret->name);

    while (mediaplayer->status != PlayerStatus::START) {
        ret = av_read_frame(fmt_ctx, &dec_pkt);
        if (ret < 0) {
            char *errorStr = av_err2str(ret);
            LOGE("FFmpeg", "FFMediaPlayer av_read_frame ret:%s", errorStr);
            break;
        }

        if (video_track_index == dec_pkt.stream_index) {
            //TODO 处理视频Packet
        }
        if (audio_track_index == dec_pkt.stream_index) {
            //TODO 处理音频Packet
        }

        LOGE("FFmpeg", "FFMediaPlayer stream_index:%d", dec_pkt.stream_index);

        av_packet_unref(&dec_pkt);
    }

    end:
    avformat_close_input(&fmt_ctx);
}

void MediaPlayer::stop(long ptr) {
    auto *player =(mediaplayer *)ptr;
    player->status = PlayerStatus::STOPED;
    LOGE("FFmpeg", "FFMediaPlayer stop url:%s", player->url);
}

void MediaPlayer::release(long ptr) {
    auto *player =(mediaplayer *)ptr;
    player->status = PlayerStatus::RELEASE;
    LOGE("FFmpeg", "FFMediaPlayer release url:%s", player->url);

}

}