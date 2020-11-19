package io.github.zy3274311.ffmpegforandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import io.github.zy3274311.nativeffmpeg.player.FFMediaPlayer;

public class FFMediaPlayerActivity extends AppCompatActivity {
    private static final String URL = "http://wliveplay.58cdn.com.cn/live/D5kl1329314099867209728.flv";
    private FFMediaPlayer ffMediaPlayer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmediaplayer);
        TextView url_tv = findViewById(R.id.url_tv);
        url_tv.setText(URL);
        ffMediaPlayer = new FFMediaPlayer();
        ffMediaPlayer.setDataSource(URL);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ffMediaPlayer.release();
    }

    public void onClickPlay(View view) {
        ffMediaPlayer.play();

    }

    public void onClickStop(View view) {
        ffMediaPlayer.stop();
    }

}