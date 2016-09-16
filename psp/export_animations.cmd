:: Name:        export_animations.cmd
:: Autor:       evolver
:: Description: export animations

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\animations
set DstFolder=%ProjectFolder%\psp\export
set ToolChainFolder=%ProjectFolder%\tools

cd %ToolChainFolder%

:: convert keyframes as obj to mesh
meshtool.exe -morphanim -length 3.0 -numKeys 31 -src %SrcFolder%\cerberus\cerberus_walk -dst %DstFolder%\cerberus_walk.mesh

cd %ProjectFolder%\psp