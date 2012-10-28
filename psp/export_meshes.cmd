:: Name:        export_meshes.bat
:: Autor:       evolver
:: Description: export animations
:: History:     30.10.2010 -> creation

cd ..\tools

:: convert obj to vertexbuffer

meshtool.exe -src ..\assets\meshes\gothic_woman.obj -dst ..\psp\export\gothic_woman.mesh
meshtool.exe -src ..\assets\meshes\cube.obj -dst ..\psp\export\cube.mesh
meshtool.exe -src ..\assets\meshes\cone.obj -dst ..\psp\export\cone.mesh

cd ..\psp
