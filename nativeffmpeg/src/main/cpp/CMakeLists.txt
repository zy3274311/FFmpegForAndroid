# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("ffmpegforandroid")

# configure import libs
#set(IMPORTED_TYPE SHARED)
#set(IMPORTED_TYPE_PATH shared)
#set(FILE_SUFFIX so)

# configure lib dir
set(DISTRIBUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../distribution)

# ffmpeg
set(FFMPEG_DIR ${DISTRIBUTION_DIR}/ffmpeg/${ANDROID_ABI})

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libavutil.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libswscale.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libavformat.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libavfilter.so)

add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libavdevice.so)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libavcodec.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_DIR}/lib/libswresample.so)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ffmpeg

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        register_jni.cpp
        swresample_jni.cpp
        avutil_jni.cpp
        avformat_jni.cpp
        player_jni.cpp
        player.cpp
        android/glenv.cpp
        android/glrender.cpp)

# add ffmpeg include
target_include_directories(ffmpeg PUBLIC ${FFMPEG_DIR}/include)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ffmpeg

        #增加ffmpeg库，注意顺序（具体看lib/pkgconfig的依赖关系）
        avdevice
        avfilter
        avformat
        avcodec
        swresample
        swscale
        avutil
        #增加ffmpeg所需要的依赖库
        -lm -lz

        EGL
        GLESv2

        android
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )

#set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../output)
#export(TARGETS ffmpeg FILE ${OUTPUT_DIR}/cmake/Findffmpeg.cmake)
