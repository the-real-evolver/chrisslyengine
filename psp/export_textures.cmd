:: Name:        export_textures.cmd
:: Author:      evolver
:: Description: add header to raw texture data from intermediate folder

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\textures
set DstFolder=%ProjectFolder%\psp\export
set ToolChainFolder=%ProjectFolder%\tools

cd %ToolChainFolder%

:: add header to raw texture data from intermediate folder
texturetool.exe -format PF_R5G6B5 -width 256 -height 256 -swizzled -data %SrcFolder%\floor.raw -out %DstFolder%\floor.tex
texturetool.exe -format PF_R5G6B5 -width 256 -height 256 -swizzled -data %SrcFolder%\water.raw -out %DstFolder%\water.tex

texturetool.exe -format PF_R5G6B5 -width 512 -height 512 -swizzled -data %SrcFolder%\gothic_solid.raw -out %DstFolder%\gothic_solid.tex
texturetool.exe -format PF_A4R4G4B4 -width 512 -height 512 -swizzled -data %SrcFolder%\gothic_alpha.raw -out %DstFolder%\gothic_alpha.tex

texturetool.exe -format PF_R5G6B5 -width 512 -height 512 -swizzled -data %SrcFolder%\cerberus.raw -out %DstFolder%\cerberus.tex

cd %ProjectFolder%\psp