#------------------------------------------------------------------------------
#  makefile for android
#  (C) 2012 Christian Bleicher
#------------------------------------------------------------------------------
CE_INCLUDES := ../../code

CE_SRC_FILES := ../../../code

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := chrisslyengine_android

LOCAL_CFLAGS := -Werror -D__ANDROID__ -D__DEBUG__

LOCAL_C_INCLUDES += $(CE_INCLUDES) $(CE_INCLUDES)/core $(CE_INCLUDES)/graphics $(CE_INCLUDES)/audio $(CE_INCLUDES)/thirdparty/stb

LOCAL_SRC_FILES := $(CE_SRC_FILES)/core/android/androiddebug.cpp $(CE_SRC_FILES)/core/android/androidfswrapper.cpp \
                   $(CE_SRC_FILES)/core/vector3.cpp $(CE_SRC_FILES)/core/matrix3.cpp $(CE_SRC_FILES)/core/matrix4.cpp $(CE_SRC_FILES)/core/quaternion.cpp $(CE_SRC_FILES)/core/plane.cpp \
                   $(CE_SRC_FILES)/core/chrisslymemory.cpp $(CE_SRC_FILES)/core/chrisslystring.cpp $(CE_SRC_FILES)/core/timer.cpp \
                   $(CE_SRC_FILES)/graphics/texturebase.cpp $(CE_SRC_FILES)/graphics/textureunitstate.cpp $(CE_SRC_FILES)/graphics/texturemanager.cpp \
                   $(CE_SRC_FILES)/graphics/pass.cpp $(CE_SRC_FILES)/graphics/material.cpp $(CE_SRC_FILES)/graphics/materialmanager.cpp $(CE_SRC_FILES)/graphics/materialparser.cpp \
                   $(CE_SRC_FILES)/graphics/graphicssystem.cpp \
                   $(CE_SRC_FILES)/graphics/hardwarevertexbufferbase.cpp \
                   $(CE_SRC_FILES)/graphics/meshmanager.cpp $(CE_SRC_FILES)/graphics/mesh.cpp $(CE_SRC_FILES)/graphics/submesh.cpp \
                   $(CE_SRC_FILES)/graphics/subentity.cpp  $(CE_SRC_FILES)/graphics/entity.cpp \
                   $(CE_SRC_FILES)/graphics/renderqueuesortinggrouping.cpp \
                   $(CE_SRC_FILES)/graphics/light.cpp \
                   $(CE_SRC_FILES)/graphics/scenemanager.cpp $(CE_SRC_FILES)/graphics/scenenode.cpp \
                   $(CE_SRC_FILES)/graphics/camera.cpp $(CE_SRC_FILES)/graphics/viewport.cpp \
                   $(CE_SRC_FILES)/graphics/animation.cpp $(CE_SRC_FILES)/graphics/animationtrack.cpp $(CE_SRC_FILES)/graphics/keyframe.cpp $(CE_SRC_FILES)/graphics/animationstate.cpp \
                   $(CE_SRC_FILES)/graphics/rendertarget.cpp $(CE_SRC_FILES)/graphics/gles2/gles2rendertexture.cpp $(CE_SRC_FILES)/graphics/egl/eglrenderwindow.cpp \
                   $(CE_SRC_FILES)/graphics/gles2/gles2mappings.cpp $(CE_SRC_FILES)/graphics/gles2/gles2texture.cpp $(CE_SRC_FILES)/graphics/gles2/gles2rendersystem.cpp \
                   $(CE_SRC_FILES)/graphics/gles2/gles2gpuprogram.cpp $(CE_SRC_FILES)/graphics/gpuprogramparams.cpp \
                   $(CE_SRC_FILES)/graphics/gles2/gles2hardwarevertexbuffer.cpp \
                   $(CE_SRC_FILES)/graphics/gles2/gles2debug.cpp \
                   $(CE_SRC_FILES)/thirdparty/stb/stb_vorbis.c \
                   $(CE_SRC_FILES)/audio/wavcodec.cpp $(CE_SRC_FILES)/audio/vorbiscodec.cpp $(CE_SRC_FILES)/audio/soundbase.cpp $(CE_SRC_FILES)/audio/audiosystem.cpp $(CE_SRC_FILES)/audio/channelbase.cpp \
                   $(CE_SRC_FILES)/audio/dsp/mixer.cpp \
                   $(CE_SRC_FILES)/audio/sles/slesaudiorenderer.cpp $(CE_SRC_FILES)/audio/sles/slessound.cpp $(CE_SRC_FILES)/audio/sles/sleschannel.cpp \
                   main.cpp

LOCAL_LDLIBS := -llog -lGLESv2 -landroid -lEGL -lOpenSLES

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module, android/native_app_glue)
