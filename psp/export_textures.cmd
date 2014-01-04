:: Name:        export_textures.cmd
:: Autor:       evolver
:: Description: add header to raw texture data from intermediate folder
:: History:     30.10.2010 -> creation

cd ..\tools

texturetool.exe -format PF_R5G6B5 -width 256 -height 256 -data ..\assets\textures\floor.raw -out ..\psp\export\floor.tex -swizzled
texturetool.exe -format PF_R5G6B5 -width 256 -height 256 -data ..\assets\textures\water.raw -out ..\psp\export\water.tex -swizzled

texturetool.exe -format PF_R5G6B5 -width 512 -height 512 -data ..\assets\textures\gothic_solid.raw -out ..\psp\export\gothic_solid.tex -swizzled
texturetool.exe -format PF_A4R4G4B4 -width 512 -height 512 -data ..\assets\textures\gothic_alpha.raw -out ..\psp\export\gothic_alpha.tex -swizzled

texturetool.exe -format PF_R5G6B5 -width 512 -height 512 -data ..\assets\textures\cerberus.raw -out ..\psp\export\cerberus.tex -swizzled

cd ..\psp