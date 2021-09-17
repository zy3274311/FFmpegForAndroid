package io.github.zy3274311.ffmpegforandroid;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.example.media.FFmpegMediaExtractor;

import io.github.zy3274311.nativeffmpeg.player.FFMediaPlayer;

public class FFMediaPlayerActivity extends AppCompatActivity {
    private static final String URL = "http://wliveplay.58cdn.com.cn/live/l6DI1438765034745376768.flv";
    private FFMediaPlayer ffMediaPlayer;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmediaplayer);
        SurfaceView player_surface = findViewById(R.id.player_surface);
        player_surface.getHolder().addCallback(new SurfaceHolder.Callback2() {
            @Override
            public void surfaceRedrawNeeded(@NonNull SurfaceHolder holder) {

            }

            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                try {
                    ffMediaPlayer.setSurface(holder.getSurface(), width, height);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });
        TextView url_tv = findViewById(R.id.url_tv);
        url_tv.setText(URL);
        ffMediaPlayer = new FFMediaPlayer();
        ffMediaPlayer.setDataSource(URL);

        FFmpegMediaExtractor extractor = new FFmpegMediaExtractor();
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