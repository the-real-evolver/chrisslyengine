:: Name:        export_animations.bat
:: Autor:       evolver
:: Description: export animations
:: History:     30.10.2010 -> creation

cd tools

:: convert keyframes as obj to vertexbuffer

OBJ_TO_PSPVB_Converter.exe -anim ..\assets\animations\cerberus\cerberus_walk ..\export\cerberus_walk.mesh

cd..
