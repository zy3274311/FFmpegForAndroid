package io.github.zy3274311.nativeffmpeg.avutil;

import android.media.AudioFormat;

/**
 * Created by zhangying62 on 2020/10/29.
 */
public class ChannelLayout {

    /**
     * @param channelConfig describes the configuration of the audio channels.
     *                      See {@link android.media.AudioFormat#CHANNEL_IN_MONO} and
     *                      {@link android.media.AudioFormat#CHANNEL_IN_STEREO}.
     *                      {@link android.media.AudioFormat#CHANNEL_IN_MONO} is guaranteed
     *                      to work on all devices.
     * @return a channel layout id that matches name, or 0 if no match is found.
     * see libavutil channel_layout.h
     */
    public static int convertChannelLayout(int channelConfig) {
        int ch_layout = -1;
        switch (channelConfig) {
            case AudioFormat.CHANNEL_IN_MONO:
                ch_layout = getChannelLayout(1);
                break;
            case AudioFormat.CHANNEL_IN_STEREO:
                ch_layout = getChannelLayout(2);
                break;
        }
        return ch_layout;
    }


    private static native int getChannelLayout(int channelCount);
}
