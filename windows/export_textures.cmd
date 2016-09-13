:: Name:        export_textures.cmd
:: Autor:       evolver
:: Description: add header to raw texture data from intermediate folder

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\textures\windows
set DstFolder=%ProjectFolder%\windows\ChrisslyEngine_Windows
set ToolChainFolder=%ProjectFolder%\tools

cd %ToolChainFolder%

:: add header to raw texture data from intermediate folder
texturetool.exe -format PF_R8G8B8A8 -width 256 -height 256 -data %SrcFolder%\gothic_solid.raw -out %DstFolder%\gothic_solid.tex
texturetool.exe -format PF_R8G8B8A8 -width 256 -height 256 -data %SrcFolder%\gothic_alpha.raw -out %DstFolder%\gothic_alpha.tex

cd %ProjectFolder%\windows