package com.example.media;

final public class FFmpegMediaExtractor {

    public FFmpegMediaExtractor(){
        native_setup();
    }

    public void setDataSource(String path, String[] keys, String[] values){
        native_setDataSource(path, keys, values);
    }

    public void release(){
        native_release();
    }

    private native void native_setup();
    private native void native_setDataSource(String path, String[] keys, String[] values);
    private native void native_release();

}
