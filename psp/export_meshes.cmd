:: Name:        export_meshes.cmd
:: Autor:       evolver
:: Description: export meshes

cd ..\tools

:: convert obj to mesh

meshtool.exe -src ..\assets\meshes\gothic_woman.obj -dst ..\psp\export\gothic_woman.mesh
meshtool.exe -src ..\assets\meshes\cube.obj -dst ..\psp\export\cube.mesh
meshtool.exe -src ..\assets\meshes\cone.obj -dst ..\psp\export\cone.mesh

cd ..\psp
