#------------------------------------------------------------------------------
#  makefile for the psp
#  (C) 2011 Christian Bleicher
#------------------------------------------------------------------------------
TARGET = ChrisslyEngine_PSP
CFLAGS = -O2 -G0 -Wall -D__PSP__
CXXFLAGS = $(CFLAGS) -fno-exceptions -fpermissive
ASFLAGS = $(CFLAGS)
PSPSDK = $(shell psp-config --pspsdk-path)
CE = ../

#------------------------------------------------------------------------------
# includes
# PSPSDK = C:/dev/devkitpro/devkitPSP/psp/sdk
#------------------------------------------------------------------------------ 
INCDIR = $(PSPSDK) $(CE)/code/core $(CE)/code/graphics code/application

#------------------------------------------------------------------------------
# object files
#------------------------------------------------------------------------------
OBJS = $(CE)/code/core/quaternion.o $(CE)/code/core/vector3.o $(CE)/code/core/matrix3.o $(CE)/code/core/matrix4.o \
       $(CE)/code/core/psp/pspmath.o $(CE)/code/core/psp/pspfswrapper.o $(CE)/code/core/psp/pspdebug.o \
       $(CE)/code/graphics/psp/pspmappings.o $(CE)/code/graphics/psp/psphardwarebuffermanager.o $(CE)/code/graphics/psp/psprendersystem.o \
       $(CE)/code/graphics/graphicssystem.o \
       $(CE)/code/graphics/texturemanager.o $(CE)/code/graphics/texturebase.o \
       $(CE)/code/graphics/textureunitstate.o $(CE)/code/graphics/pass.o $(CE)/code/graphics/material.o \
       $(CE)/code/graphics/meshmanager.o $(CE)/code/graphics/mesh.o $(CE)/code/graphics/submesh.o \
       $(CE)/code/graphics/subentity.o $(CE)/code/graphics/entity.o \
       $(CE)/code/graphics/scenemanager.o $(CE)/code/graphics/scenenode.o \
       $(CE)/code/graphics/camera.o $(CE)/code/graphics/viewport.o \
       $(CE)/code/graphics/rendertarget.o $(CE)/code/graphics/psp/psprendertexture.o $(CE)/code/graphics/psp/psprenderwindow.o \
       code/main.o \
       code/application/stateanimationtest.o code/application/statematerialtest.o code/application/statemanager.o

#------------------------------------------------------------------------------
# libaries
#------------------------------------------------------------------------------
LIBS = -lpsplibc -lstdc++ -lpspgum -lpspgu -lm -lpspfpu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = ChrisslyEngine

include $(PSPSDK)/lib/build.mak
