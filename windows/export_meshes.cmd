:: Name:        export_meshes.cmd
:: Author:      evolver
:: Description: export meshes

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\meshes
set DstFolder=%ProjectFolder%\windows\ChrisslyEngine_Windows
set ToolChainFolder=%ProjectFolder%\tools\bin\win32

cd %ToolChainFolder%

:: convert obj to mesh
meshtool.exe -src %SrcFolder%\gothic_woman.obj -dst %DstFolder%\gothic_woman.mesh

cd %ProjectFolder%\windows
