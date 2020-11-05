package io.github.zy3274311.nativeffmpeg.player;

/**
 * Created by zhangying62 on 2020/11/5.
 */
public class FFMediaPlayer {

    public native void setDataSource();

    public native void play(String url);

    public native void stop();
}
