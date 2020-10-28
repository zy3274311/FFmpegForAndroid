package io.github.zy3274311.ffmpegforandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import io.github.zy3274311.nativeffmpeg.Swresample;


public class MainActivity extends AppCompatActivity {
    private Swresample swresample;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        swresample = new Swresample();
        swresample.init();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        swresample.free();
    }
}