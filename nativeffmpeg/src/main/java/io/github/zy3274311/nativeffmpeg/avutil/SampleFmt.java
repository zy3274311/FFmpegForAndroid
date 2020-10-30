package io.github.zy3274311.nativeffmpeg.avutil;

import android.media.AudioFormat;

/**
 * Created by zhangying62 on 2020/10/29.
 */
public class SampleFmt {
    /**
     * @param sampleFormat the format in which the audio data is to be returned.
     *                     See {@link android.media.AudioFormat#ENCODING_PCM_8BIT},
     *                     {@link android.media.AudioFormat#ENCODING_PCM_16BIT},
     *                     and {@link android.media.AudioFormat#ENCODING_PCM_FLOAT}.
     * @return a sample format corresponding to name, or AV_SAMPLE_FMT_NONE on error.
     * see libavutil samplefmt.h
     */
    public static int convertSampleFormat(int sampleFormat) {
        int sample_fmt = -1;
        switch (sampleFormat) {
            case AudioFormat.ENCODING_PCM_8BIT:
                sample_fmt = getSampleFormat(8);
                break;
            case AudioFormat.ENCODING_PCM_16BIT:
                sample_fmt = getSampleFormat(16);
                break;
            case AudioFormat.ENCODING_PCM_FLOAT:
                sample_fmt = getSampleFormat(32);
                break;
            default:
                break;
        }
        return sample_fmt;
    }

    private static native int getSampleFormat(int sizeByBit);
}
