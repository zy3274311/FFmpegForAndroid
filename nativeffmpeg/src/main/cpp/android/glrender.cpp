//
// Created by zhangying62 on 2020/11/26.
//

#include <string>
#include "glrender.h"
#include "../ndk_log.h"

void glrender::init() {
    GLint param;
    std::string fragmentSource = "precision mediump float;\n"
                               "varying vec2 vTextureCoord;\n"
                               "uniform sampler2D sTexture;\n"
                               "void main() {\n"
                               "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n"
                               "}\n";

    auto *fragment_source = const_cast<GLchar *>(fragmentSource.c_str());
    LOGE("FFmpeg", "glrender fragment_shader :\n%s", fragment_source);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &param);
    if (param != GL_TRUE) {
        LOGE("FFmpeg", "glrender fragment_shader :%d", param);
    }

    std::string vertexSource = "uniform mat4 uMVPMatrix;\n"
                               "uniform mat4 uTexMatrix;\n"
                               "attribute vec4 aPosition;\n"
                               "attribute vec4 aTextureCoord;\n"
                               "varying vec2 vTextureCoord;\n"
                               "void main() {\n"
                               "gl_Position = uMVPMatrix * aPosition;\n"
                               "vTextureCoord = (uTexMatrix * aTextureCoord).xy;}";
    auto *vertex_source= const_cast<GLchar *>(vertexSource.c_str());
    LOGE("FFmpeg", "glrender vertex_source :\n%s", vertex_source);
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &param);
    if (param != GL_TRUE) {
        LOGE("FFmpeg", "glrender vertex_shader :%d", param);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    aPosition = glGetAttribLocation(program, "aPosition");
    aTextureCoord = glGetAttribLocation(program, "aTextureCoord");
    uMVPMatrix = glGetUniformLocation(program, "uMVPMatrix");
    uTexMatrix = glGetUniformLocation(program, "uTexMatrix");
    mTextureTarget = GL_TEXTURE_2D;


//    int uKernel = glGetUniformLocation(program, "uKernel");
//    auto uTexOffset = 0;
//    auto uColorAdjust = 0;
//    if (uKernel < 0) {
//        // no kernel in this one
//        uKernel = -1;
//        uTexOffset = -1;
//        uColorAdjust = -1;
//    } else {
//        uTexOffset = glGetUniformLocation(program, "uTexOffset");
//        uColorAdjust = glGetUniformLocation(program, "uColorAdjust");
//    }

    setIdentityM(mvpMatrix, 0);
    setIdentityM(texMatrix, 0);
    coordsPerVertex = 2;
    vertexStride = coordsPerVertex * SIZEOF_FLOAT;
    texStride = 2 * SIZEOF_FLOAT;
}

void glrender::render(int texture) {
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(mTextureTarget, texture);


    glUniformMatrix4fv(uMVPMatrix, 1, false, mvpMatrix);
    glUniformMatrix4fv(uMVPMatrix, 1, false, texMatrix);
    glEnableVertexAttribArray(aPosition);
    glEnableVertexAttribArray(aTextureCoord);
    glVertexAttribPointer(aPosition, coordsPerVertex,
                          GL_FLOAT, false, vertexStride, vertexBuffer);
    glVertexAttribPointer(aTextureCoord, 2,
                          GL_FLOAT, false, texStride, texBuffer);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTextureCoord);
    glBindTexture(mTextureTarget, 0);
    glUseProgram(0);
}

void glrender::free() const {
    glDeleteProgram(program);
}

int glrender::createTexture(int width, int height, const void *pixels) const {
    GLuint textures[1];
    glGenTextures(1, textures);
    glBindTexture(mTextureTarget, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameterf(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(mTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(mTextureTarget, 0);
    return textures[0];
}

void glrender::updateTexture(int texture, int width, int height, const void *pixels) const {
    glBindTexture(mTextureTarget, texture);
    glTexSubImage2D(mTextureTarget, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(mTextureTarget, 0);
}

void glrender::setIdentityM(float *sm, int smOffset) {
    for (int i=0 ; i<16 ; i++) {
        sm[smOffset + i] = 0;
    }
    for(int i = 0; i < 16; i += 5) {
        sm[smOffset + i] = 1.0f;
    }
}