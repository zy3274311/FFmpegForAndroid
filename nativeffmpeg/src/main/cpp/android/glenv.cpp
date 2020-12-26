//
// Created by zhangying62 on 2020/11/26.
//

#include <memory>
#include "glenv.h"
#include "../ndk_log.h"

void glenv::init() {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint numConfigs;
    EGLConfig config = nullptr;
    EGLint w, h;

    mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(mEGLDisplay, nullptr, nullptr);
    EGLBoolean success = eglChooseConfig(mEGLDisplay, attribs, nullptr, 0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(mEGLDisplay, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    auto i = 0;
    for (; i < numConfigs; i++) {
        auto &cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(mEGLDisplay, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(mEGLDisplay, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(mEGLDisplay, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(mEGLDisplay, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0) {
            LOGE("FFmpeg", "eglQuerySurface r:%d", r);
            LOGE("FFmpeg", "eglQuerySurface g:%d", g);
            LOGE("FFmpeg", "eglQuerySurface b:%d", b);
            LOGE("FFmpeg", "eglQuerySurface d:%d", d);
            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    if (config == nullptr) {
        LOGE("FFmpeg", "Unable to initialize EGLConfig");
    }

    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    eglContext = eglCreateContext(mEGLDisplay, config, nullptr, attrib_list);

    eglSurface = eglCreateWindowSurface(mEGLDisplay, config, window, nullptr);
    if (eglMakeCurrent(mEGLDisplay, eglSurface, eglSurface, eglContext) == EGL_FALSE) {
        LOGE("FFmpeg", "Unable to eglMakeCurrent");
    }

    eglQuerySurface(mEGLDisplay, eglSurface, EGL_WIDTH, &w);
    eglQuerySurface(mEGLDisplay, eglSurface, EGL_HEIGHT, &h);

    LOGE("FFmpeg", "eglQuerySurface w:%d", w);
    LOGE("FFmpeg", "eglQuerySurface h:%d", h);
}

void glenv::swap() {
    eglSwapBuffers(mEGLDisplay, eglSurface);
}

void glenv::free() {
    eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(mEGLDisplay, eglContext);
    eglDestroySurface(mEGLDisplay, eglSurface);
    eglTerminate(mEGLDisplay);

    mEGLDisplay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
}

glenv::glenv(ANativeWindow *window) {
    this->window = window;
}

