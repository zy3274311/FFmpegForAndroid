package io.github.zy3274311.nativeffmpeg.player;

import android.graphics.SurfaceTexture;
import android.view.Surface;
import android.view.SurfaceHolder;

/**
 * Created by zhangying62 on 2020/11/5.
 */
public class FFMediaPlayer {
    private final long ptr;

    public FFMediaPlayer() {
        ptr = _init();
    }

    public void setSurface(Surface surface, int width, int height) throws Exception {
        if(surface==null) {
            throw new Exception("Surface is null");
        }
        _setSurface(ptr, surface, width, height);
    }

    public void setSurfaceHolder(SurfaceHolder holder, int width, int height) throws Exception {
        if(holder==null) {
            throw new Exception("holder is null");
        }
        _setSurface(ptr, holder, width, height);
    }

    public void setSurfaceTexture(SurfaceTexture surfaceTexture, int width, int height) throws Exception {
        if(surfaceTexture==null) {
            throw new Exception("surfaceTexture is null");
        }
        _setSurface(ptr, surfaceTexture, width, height);
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

    private native void _setSurface(long ptr, Object surface, int width, int height);

    private native void _setDataSource(long ptr, String url);

    private native void _play(long ptr);

    private native void _stop(long ptr);

    private native void _release(long ptr);

}
