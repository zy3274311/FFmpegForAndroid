//
// Created by zhangying62 on 2020/11/26.
//

#ifndef FFMPEGFORANDROID_GLENV_H
#define FFMPEGFORANDROID_GLENV_H
#include <EGL/egl.h>
class glenv {
private:
    ANativeWindow *window;
    EGLDisplay mEGLDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
public:
    glenv(ANativeWindow *window);
    void init();
    void swap();
    void free();
};

#endif //FFMPEGFORANDROID_GLENV_H
