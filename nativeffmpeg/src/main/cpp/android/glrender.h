//
// Created by zhangying62 on 2020/11/26.
//

#ifndef FFMPEGFORANDROID_GLRENDER_H
#define FFMPEGFORANDROID_GLRENDER_H
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
class glrender {
private:
    GLuint program;
    int aPosition;
    int aTextureCoord;
    int uMVPMatrix;
    int uTexMatrix;
    int mTextureTarget;
public:
    void init();

    void render(int width, int height, const void *pixels);

    void free();
};

#endif //FFMPEGFORANDROID_GLRENDER_H
