#------------------------------------------------------------------------------
#  cmakefile for android
#  (C) 2023 Christian Bleicher
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.4.1)

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_definitions(-D__CE_DEBUG__ -D__CE_ANDROID__ -D__CE_GLES2__ -D__CE_SLES__)

set(CE_SRC_FILES ${CMAKE_SOURCE_DIR}/../../../../../code)

add_library(chrisslyengine_android SHARED
    ${CE_SRC_FILES}/core/android/androiddebug.cpp
    ${CE_SRC_FILES}/core/android/androidfswrapper.cpp
    ${CE_SRC_FILES}/core/chrisslymemory.cpp
    ${CE_SRC_FILES}/core/chrisslystring.cpp
    ${CE_SRC_FILES}/core/damping.cpp
    ${CE_SRC_FILES}/core/intersection.cpp
    ${CE_SRC_FILES}/core/matrix3.cpp
    ${CE_SRC_FILES}/core/matrix4.cpp
    ${CE_SRC_FILES}/core/plane.cpp
    ${CE_SRC_FILES}/core/quaternion.cpp
    ${CE_SRC_FILES}/core/timer.cpp
    ${CE_SRC_FILES}/core/vector3.cpp
    ${CE_SRC_FILES}/graphics/texturebase.cpp
    ${CE_SRC_FILES}/graphics/textureunitstate.cpp
    ${CE_SRC_FILES}/graphics/texturemanager.cpp
    ${CE_SRC_FILES}/graphics/pass.cpp
    ${CE_SRC_FILES}/graphics/material.cpp
    ${CE_SRC_FILES}/graphics/materialmanager.cpp
    ${CE_SRC_FILES}/graphics/materialparser.cpp
    ${CE_SRC_FILES}/graphics/graphicssystem.cpp
    ${CE_SRC_FILES}/graphics/hardwarevertexbufferbase.cpp
    ${CE_SRC_FILES}/graphics/gpuprogrammanager.cpp
    ${CE_SRC_FILES}/graphics/meshmanager.cpp
    ${CE_SRC_FILES}/graphics/mesh.cpp
    ${CE_SRC_FILES}/graphics/submesh.cpp
    ${CE_SRC_FILES}/graphics/skeleton.cpp
    ${CE_SRC_FILES}/graphics/skeletonserialiser.cpp
    ${CE_SRC_FILES}/graphics/subentity.cpp
    ${CE_SRC_FILES}/graphics/entity.cpp
    ${CE_SRC_FILES}/graphics/renderqueue.cpp
    ${CE_SRC_FILES}/graphics/light.cpp
    ${CE_SRC_FILES}/graphics/scenemanager.cpp
    ${CE_SRC_FILES}/graphics/scenenode.cpp
    ${CE_SRC_FILES}/graphics/camera.cpp
    ${CE_SRC_FILES}/graphics/viewport.cpp
    ${CE_SRC_FILES}/graphics/animation.cpp
    ${CE_SRC_FILES}/graphics/animationtrack.cpp
    ${CE_SRC_FILES}/graphics/keyframe.cpp
    ${CE_SRC_FILES}/graphics/animationstate.cpp
    ${CE_SRC_FILES}/graphics/rendertarget.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2rendertexture.cpp
    ${CE_SRC_FILES}/graphics/egl/eglrenderwindow.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2mappings.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2texture.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2rendersystem.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2gpuprogram.cpp
    ${CE_SRC_FILES}/graphics/gpuprogramparams.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2hardwarevertexbuffer.cpp
    ${CE_SRC_FILES}/graphics/gles2/gles2debug.cpp
    ${CE_SRC_FILES}/thirdparty/stb/stb_vorbis.c
    ${CE_SRC_FILES}/audio/wavcodec.cpp
    ${CE_SRC_FILES}/audio/vorbiscodec.cpp
    ${CE_SRC_FILES}/audio/soundbase.cpp
    ${CE_SRC_FILES}/audio/dsp.cpp
    ${CE_SRC_FILES}/audio/audiosystem.cpp
    ${CE_SRC_FILES}/audio/channelbase.cpp
    ${CE_SRC_FILES}/audio/dsp/mixer.cpp
    ${CE_SRC_FILES}/audio/dsp/filters.cpp
    ${CE_SRC_FILES}/audio/sles/slesaudiorenderer.cpp
    ${CE_SRC_FILES}/audio/sles/slessound.cpp
    ${CE_SRC_FILES}/audio/sles/sleschannel.cpp
    main.cpp)

target_include_directories(chrisslyengine_android PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${CE_SRC_FILES}
    ${CE_SRC_FILES}/core
    ${CE_SRC_FILES}/core/standard
    ${CE_SRC_FILES}/code/core/posix
    ${CE_SRC_FILES}/core/android
    ${CE_SRC_FILES}/graphics
    ${CE_SRC_FILES}/graphics/egl
    ${CE_SRC_FILES}/graphics/gles2
    ${CE_SRC_FILES}/audio
    ${CE_SRC_FILES}/audio/dsp
    ${CE_SRC_FILES}/audio/sles
    ${CE_SRC_FILES}/thirdparty/stb)

# add lib dependencies
target_link_libraries(
    chrisslyengine_android
    android
    native_app_glue
    EGL
    GLESv2
    log
    OpenSLES)
