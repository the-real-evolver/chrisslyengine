#------------------------------------------------------------------------------
#  makefile for android
#  (C) 2012 Christian Bleicher
#------------------------------------------------------------------------------
CE := jni

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := chrisslyengine_android

LOCAL_CFLAGS := -Werror -D__ANDROID__

LOCAL_C_INCLUDES += $(CE)/core $(CE)/graphics

LOCAL_SRC_FILES := core/android/androiddebug.cpp core/android/androidfswrapper.cpp core/android/androidmath.cpp \
                   core/vector3.cpp core/matrix3.cpp core/matrix4.cpp core/quaternion.cpp core/chrisslymemory.cpp \
                   graphics/texturebase.cpp graphics/textureunitstate.cpp graphics/texturemanager.cpp \
                   graphics/pass.cpp graphics/material.cpp \
                   graphics/graphicssystem.cpp \
                   graphics/meshmanager.cpp graphics/mesh.cpp graphics/submesh.cpp \
                   graphics/subentity.cpp  graphics/entity.cpp \
                   graphics/renderqueuesortinggrouping.cpp \
                   graphics/light.cpp \
                   graphics/scenemanager.cpp graphics/scenenode.cpp \
                   graphics/camera.cpp graphics/viewport.cpp \
                   graphics/animation.cpp graphics/animationtrack.cpp graphics/keyframe.cpp graphics/animationstate.cpp \
                   graphics/rendertarget.cpp graphics/android/gles2rendertexture.cpp graphics/android/gles2renderwindow.cpp \
                   graphics/android/gles2mappings.cpp graphics/android/gles2texture.cpp graphics/android/gles2rendersystem.cpp \
                   graphics/android/gles2gpuprogram.cpp \
                   main.cpp

LOCAL_LDLIBS := -llog -lGLESv2 -landroid -lEGL

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
