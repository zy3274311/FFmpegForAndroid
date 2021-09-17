package io.github.zy3274311.ffmpegforandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.example.media.NativeMedia;

import java.util.Locale;

import io.github.zy3274311.nativeffmpeg.FFmpeg;
import io.github.zy3274311.nativeffmpeg.avformat.AVFormat;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        FFmpeg.loadLibrary();
        NativeMedia.loadLibrary();
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        AVFormat avFormat = new AVFormat();
        String txt = String.format(Locale.getDefault(),
                "getAvFormatVersion:%d\nconfiguration:%s\nnative_license:%s",
                avFormat.getAvFormatVersion()
                , avFormat.configuration()
                , avFormat.license());
        tv.setText(txt);

    }


    public void onClickAVFormat(View view) {
        startActivity(new Intent(this, FFMediaPlayerActivity.class));
    }

    public void onClickSwresample(View view) {
        startActivity(new Intent(this, SwresampleActivity.class));

    }
}