#------------------------------------------------------------------------------
#  makefile for the psp
#  (C) 2011 Christian Bleicher
#------------------------------------------------------------------------------
TARGET = ChrisslyEngine_PSP
# for a custom root directroy use: -DROOT_DIR=\"ms0:/PSP/GAME/YourApplication/\"
CFLAGS = -O2 -G0 -Wall -D__PSP__
CXXFLAGS = $(CFLAGS) -fno-exceptions
ASFLAGS = $(CFLAGS)
PSPSDK = $(shell psp-config --pspsdk-path)
CE = ../

#------------------------------------------------------------------------------
# includes
# PSPSDK = C:/dev/devkitpro/devkitPSP/psp/sdk
#------------------------------------------------------------------------------ 
INCDIR = $(PSPSDK) $(CE)/code/core $(CE)/code/graphics $(CE)/code/thirdparty/stb $(CE)/code/application code/tests

#------------------------------------------------------------------------------
# object files
#------------------------------------------------------------------------------
OBJS = $(CE)/code/core/quaternion.o $(CE)/code/core/vector3.o $(CE)/code/core/matrix3.o $(CE)/code/core/matrix4.o $(CE)/code/core/chrisslymemory.o $(CE)/code/core/chrisslystring.o \
       $(CE)/code/core/psp/pspfswrapper.o $(CE)/code/core/psp/pspdebug.o \
       $(CE)/code/graphics/psp/pspmappings.o $(CE)/code/graphics/psp/psphardwarebuffermanager.o $(CE)/code/graphics/psp/psprendersystem.o \
       $(CE)/code/graphics/graphicssystem.o \
       $(CE)/code/graphics/texturemanager.o $(CE)/code/graphics/texturebase.o $(CE)/code/graphics/psp/psptexture.o \
       $(CE)/code/graphics/materialmanager.o $(CE)/code/graphics/materialparser.o $(CE)/code/graphics/textureunitstate.o $(CE)/code/graphics/pass.o $(CE)/code/graphics/material.o \
       $(CE)/code/graphics/meshmanager.o $(CE)/code/graphics/mesh.o $(CE)/code/graphics/submesh.o \
       $(CE)/code/graphics/subentity.o $(CE)/code/graphics/entity.o \
       $(CE)/code/graphics/renderqueuesortinggrouping.o \
       $(CE)/code/graphics/light.o \
       $(CE)/code/graphics/scenemanager.o $(CE)/code/graphics/scenenode.o \
       $(CE)/code/graphics/camera.o $(CE)/code/graphics/viewport.o \
       $(CE)/code/graphics/animation.o $(CE)/code/graphics/animationtrack.o $(CE)/code/graphics/keyframe.o $(CE)/code/graphics/animationstate.o \
       $(CE)/code/graphics/rendertarget.o $(CE)/code/graphics/psp/psprendertexture.o $(CE)/code/graphics/psp/psprenderwindow.o \
       $(CE)/code/application/statemanager.o \
       code/main.o \
       code/tests/stateanimationtest.o code/tests/statematerialtest.o

#------------------------------------------------------------------------------
# libaries
#------------------------------------------------------------------------------
LIBS = -lpsplibc -lstdc++ -lpspgum -lpspgu -lm -lpspfpu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = ChrisslyEngine

include $(PSPSDK)/lib/build.mak
