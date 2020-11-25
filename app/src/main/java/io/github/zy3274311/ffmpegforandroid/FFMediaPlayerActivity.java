package io.github.zy3274311.ffmpegforandroid;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.media.MediaMuxer;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import io.github.zy3274311.nativeffmpeg.player.FFMediaPlayer;

public class FFMediaPlayerActivity extends AppCompatActivity {
    private static final String URL = "http://wliveplay.58cdn.com.cn/live/VUnA1331477755195342848.flv";
    private FFMediaPlayer ffMediaPlayer;
    private SurfaceView player_surface;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmediaplayer);
        player_surface = findViewById(R.id.player_surface);
        player_surface.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                try {
                    ffMediaPlayer.setSurface(holder.getSurface());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });
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