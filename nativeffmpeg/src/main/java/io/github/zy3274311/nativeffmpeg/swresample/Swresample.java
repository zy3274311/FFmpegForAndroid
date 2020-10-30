package io.github.zy3274311.nativeffmpeg.swresample;

import android.util.Log;

import io.github.zy3274311.nativeffmpeg.avutil.ChannelLayout;
import io.github.zy3274311.nativeffmpeg.avutil.SampleFmt;

/**
 * Created by zhangying62 on 2020/10/27.
 */
public class Swresample {
    private static final String TAG = Swresample.class.getSimpleName();
    private long pr;

    public void init(int out_channelConfig,
                     int out_sampleFormat,
                     int out_sample_rate,
                     int in_channelConfig,
                     int in_sampleFormat,
                     int in_sample_rate) {
        //TODO Converts the parameter to a type supported by libswresample
        int out_ch_layout = ChannelLayout.convertChannelLayout(out_channelConfig);
        int in_ch_layout = ChannelLayout.convertChannelLayout(in_channelConfig);
        int out_sample_fmt = SampleFmt.convertSampleFormat(out_sampleFormat);
        int in_sample_fmt = SampleFmt.convertSampleFormat(in_sampleFormat);
        Log.e(TAG, "out_ch_layout: "+out_ch_layout);
        Log.e(TAG, "in_ch_layout: "+in_ch_layout);
        Log.e(TAG, "out_sample_fmt: "+out_sample_fmt);
        Log.e(TAG, "in_sample_fmt: "+in_sample_fmt);
        pr = initSwr(
                out_ch_layout,
                out_sample_fmt,
                out_sample_rate,
                in_ch_layout,
                in_sample_fmt,
                in_sample_rate);
    }

    public void release() {
        free(pr);
    }

    /**
     * * @param out_ch_layout   output channel layout (AV_CH_LAYOUT_*)
     * * @param out_sample_fmt  output sample format (AV_SAMPLE_FMT_*).
     * * @param out_sample_rate output sample rate (frequency in Hz)
     * * @param in_ch_layout    input channel layout (AV_CH_LAYOUT_*)
     * * @param in_sample_fmt   input sample format (AV_SAMPLE_FMT_*).
     * * @param in_sample_rate  input sample rate (frequency in Hz)
     *
     * @return
     */
    private native long initSwr(int out_ch_layout,
                                int out_sample_fmt,
                                int out_sample_rate,
                                int in_ch_layout,
                                int in_sample_fmt,
                                int in_sample_rate);

    private native void free(long pr);
}
