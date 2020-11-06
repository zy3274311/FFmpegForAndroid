package io.github.zy3274311.nativeffmpeg.player;

/**
 * Created by zhangying62 on 2020/11/5.
 */
public class FFMediaPlayer {
    private final long ptr;

    public FFMediaPlayer() {
        ptr = _init();
    }

    public void setDataSource(String url) {
        _setDataSource(ptr, url);
    }

    public void play() {
        _play(ptr);
    }

    public void stop() {
        _stop(ptr);
    }

    public void release() {
        _release(ptr);
    }

    private native long _init();

    private native void _setDataSource(long ptr, String url);

    private native void _play(long ptr);

    private native void _stop(long ptr);

    private native void _release(long ptr);
}
