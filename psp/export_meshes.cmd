:: Name:        export_meshes.cmd
:: Autor:       evolver
:: Description: export meshes

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\meshes
set DstFolder=%ProjectFolder%\psp\export
set ToolChainFolder=%ProjectFolder%\tools

cd %ToolChainFolder%

:: convert obj to mesh
meshtool.exe -src %SrcFolder%\gothic_woman.obj -dst %DstFolder%\gothic_woman.mesh
meshtool.exe -src %SrcFolder%\cube.obj -dst %DstFolder%\cube.mesh
meshtool.exe -src %SrcFolder%\cone.obj -dst %DstFolder%\cone.mesh

cd %ProjectFolder%\psp