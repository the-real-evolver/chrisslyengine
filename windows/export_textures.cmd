:: Name:        export_textures.cmd
:: Author:      evolver
:: Description: add header to raw texture data from intermediate folder

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\textures
set DstFolder=%ProjectFolder%\windows\export
set ToolChainFolder=%ProjectFolder%\tools\bin\win32

cd %ToolChainFolder%

:: add header to raw texture data from intermediate folder
texturetool.exe -format PF_DXT1 -width 512 -height 512 -data %SrcFolder%\gothic_solid_dxt1.raw -out %DstFolder%\gothic_solid.tex
texturetool.exe -format PF_DXT3 -width 512 -height 512 -data %SrcFolder%\gothic_alpha_dxt3.raw -out %DstFolder%\gothic_alpha.tex

cd %ProjectFolder%\windows