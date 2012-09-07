:: Name:        export_meshes.bat
:: Autor:       evolver
:: Description: export animations
:: History:     30.10.2010 -> creation

cd tools

:: convert obj to vertexbuffer

OBJ_TO_PSPVB_Converter.exe ..\assets\meshes\gothic_woman.obj ..\export\gothic_woman.mesh
OBJ_TO_PSPVB_Converter.exe ..\assets\meshes\cube.obj ..\export\cube.mesh
OBJ_TO_PSPVB_Converter.exe ..\assets\meshes\cone.obj ..\export\cone.mesh

cd..
