package io.github.zy3274311.ffmpegforandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.media.AudioFormat;
import android.os.Bundle;
import android.view.View;

import io.github.zy3274311.nativeffmpeg.swresample.Swresample;

public class SwresampleActivity extends AppCompatActivity {
    private Swresample swresample;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_swresample);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        swresample.release();
    }

    public void onClickInfo(View view) {
        swresample = new Swresample();
        swresample.init(
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT,
                48000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_8BIT,
                44100);
    }
}