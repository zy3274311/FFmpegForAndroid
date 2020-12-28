//
// Created by zhangying62 on 2020/11/26.
//

#ifndef FFMPEGFORANDROID_GLRENDER_H
#define FFMPEGFORANDROID_GLRENDER_H
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
class glrender {
private:
    const int SIZEOF_FLOAT = 4;
    GLuint program;
    int aPosition;
    int aTextureCoord;
    int uMVPMatrix;
    int uTexMatrix;
    int mTextureTarget;

    float mvpMatrix[16];//uMVPMatrix
    float texMatrix[16];//uTexMatrix

    float vertexBuffer[8] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    float texBuffer[8] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,1.0f, 1.0f};
    int coordsPerVertex;
    int vertexStride;
    int texStride;
public:
    void init();

    void render(int texture);

    void free() const;

    int createTexture(int width, int height,const void *pixels) const;

    void updateTexture(int texture,int width, int height, const void *pixels) const;

    static void setIdentityM(float *sm, int smOffset);
};

#endif //FFMPEGFORANDROID_GLRENDER_H
