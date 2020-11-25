package io.github.zy3274311.nativeffmpeg.player;

import android.view.Surface;

/**
 * Created by zhangying62 on 2020/11/5.
 */
public class FFMediaPlayer {
    private final long ptr;

    public FFMediaPlayer() {
        ptr = _init();
    }

    public void setSurface(Surface surface) throws Exception {
        if(surface==null) {
            throw new Exception("Surface is null");
        }
        _setSurface(ptr, surface);
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

    private native void _setSurface(long ptr, Surface surface);

    private native void _setDataSource(long ptr, String url);

    private native void _play(long ptr);

    private native void _stop(long ptr);

    private native void _release(long ptr);
}
