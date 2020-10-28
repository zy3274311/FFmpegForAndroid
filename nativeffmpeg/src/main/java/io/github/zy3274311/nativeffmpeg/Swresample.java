package io.github.zy3274311.nativeffmpeg;

/**
 * Created by zhangying62 on 2020/10/27.
 */
public class Swresample {
    static {
        System.loadLibrary("native-lib");
    }

    private long pr;

    public void init() {
        pr = initSwr();
    }

    public void free() {
        free(pr);
    }

    private native long initSwr();

    private native void free(long pr);
}
