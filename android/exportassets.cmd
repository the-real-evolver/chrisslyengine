:: Name:        exportassets.cmd
:: Author:      evolver
:: Description: export assets for Android platform

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets
set DstFolder=%ProjectFolder%\android\app\src\main\assets
set ToolChainFolder=%ProjectFolder%\tools\bin\win32

cd %ToolChainFolder%

texturetool.exe -pkm -data %SrcFolder%\textures\cerberus_etc1.pkm -out %DstFolder%\cerberus.tex

meshtool.exe -morphanim -length 3.0 -numKeys 31 -src %SrcFolder%\animations\cerberus\cerberus_walk -dst %DstFolder%\cerberus_walk.mesh

xcopy /i /y %SrcFolder%\materials\materials.material %DstFolder%\materials.material*

xcopy /i /y %SrcFolder%\audio\intro.ogg %DstFolder%\intro.ogg*

cd %ProjectFolder%\android