#------------------------------------------------------------------------------
#  makefile for the psp
#  (C) 2011 Christian Bleicher
#------------------------------------------------------------------------------
# for a custom root directroy use: -DROOT_DIR=\"ms0:/PSP/GAME/YourApplication/\"
CFLAGS = -O2 -G0 -Wall -D__PSP__ -D__DEBUG__
CXXFLAGS = $(CFLAGS) -fno-exceptions
ASFLAGS = $(CFLAGS)
PSPSDK = $(shell psp-config --pspsdk-path)
CE = ../code

#------------------------------------------------------------------------------
# includes
# PSPSDK = C:/dev/devkitpro/devkitPSP/psp/sdk
#------------------------------------------------------------------------------ 
INCDIR = $(PSPSDK) $(CE)/core $(CE)/graphics $(CE)/audio $(CE)/thirdparty/stb $(CE)/application code/tests

#------------------------------------------------------------------------------
# object files
#------------------------------------------------------------------------------
OBJS = $(CE)/core/quaternion.o $(CE)/core/vector3.o $(CE)/core/matrix3.o $(CE)/core/matrix4.o $(CE)/core/chrisslymemory.o $(CE)/core/chrisslystring.o \
       $(CE)/core/psp/pspfswrapper.o $(CE)/core/psp/pspdebug.o \
       $(CE)/graphics/psp/pspmappings.o $(CE)/graphics/psp/psphardwarebuffermanager.o $(CE)/graphics/psp/psprendersystem.o \
       $(CE)/graphics/psp/psphardwarevertexbuffer.o \
       $(CE)/graphics/graphicssystem.o \
       $(CE)/graphics/texturemanager.o $(CE)/graphics/texturebase.o $(CE)/graphics/psp/psptexture.o \
       $(CE)/graphics/materialmanager.o $(CE)/graphics/materialparser.o $(CE)/graphics/textureunitstate.o $(CE)/graphics/pass.o $(CE)/graphics/material.o \
       $(CE)/graphics/hardwarevertexbufferbase.o \
       $(CE)/graphics/meshmanager.o $(CE)/graphics/mesh.o $(CE)/graphics/submesh.o \
       $(CE)/graphics/subentity.o $(CE)/graphics/entity.o \
       $(CE)/graphics/renderqueuesortinggrouping.o \
       $(CE)/graphics/light.o \
       $(CE)/graphics/scenemanager.o $(CE)/graphics/scenenode.o \
       $(CE)/graphics/camera.o $(CE)/graphics/viewport.o \
       $(CE)/graphics/animation.o $(CE)/graphics/animationtrack.o $(CE)/graphics/keyframe.o $(CE)/graphics/animationstate.o \
       $(CE)/graphics/rendertarget.o $(CE)/graphics/psp/psprendertexture.o $(CE)/graphics/psp/psprenderwindow.o \
       $(CE)/audio/wavcodec.o $(CE)/audio/vorbiscodec.o $(CE)/audio/soundbase.o $(CE)/audio/audiosystem.o $(CE)/audio/channelbase.o \
       $(CE)/audio/psp/pspaudiorenderer.o $(CE)/audio/psp/pspchannel.o \
       $(CE)/application/statemanager.o \
       code/main.o \
       code/tests/stateanimationtest.o code/tests/statematerialtest.o

#------------------------------------------------------------------------------
# libaries
#------------------------------------------------------------------------------
LIBS = -lpsplibc -lstdc++ -lpspgum -lpspgu -lm -lpspfpu -lpspaudio

PSP_EBOOT_TITLE = ChrisslyEngine
TARGET = ChrisslyEngine_PSP
EXTRA_TARGETS = EBOOT.PBP

include $(PSPSDK)/lib/build.mak
