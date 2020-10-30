package io.github.zy3274311.ffmpegforandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.media.AudioFormat;
import android.os.Bundle;
import android.widget.TextView;

import java.util.Locale;

import io.github.zy3274311.nativeffmpeg.FFmpeg;
import io.github.zy3274311.nativeffmpeg.avformat.AVFormat;
import io.github.zy3274311.nativeffmpeg.swresample.Swresample;


public class MainActivity extends AppCompatActivity {
    private Swresample swresample;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        FFmpeg.loadLibrary();
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        AVFormat avFormat = new AVFormat();
        String txt = String.format(Locale.getDefault(),
                "getAvFormatVersion:%d\nconfiguration:%s\nnative_license:%s",
                avFormat.getAvFormatVersion()
                , avFormat.configuration()
                , avFormat.license());
        tv.setText(txt);
        swresample = new Swresample();
        swresample.init(
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT,
                48000,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_8BIT,
                44100);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        swresample.release();
    }
}