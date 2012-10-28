:: Name:        exportassets.bat
:: Autor:       evolver
:: Description: export assets for Android platform
:: History:     26.05.2010 -> creation

cd ..\..\tools

texturetool.exe -format PF_ETC1_RGB8 -width 256 -height 256 -data ..\assets\textures\mage_etc1.raw -out ..\android\ChrisslyEngine_Android\assets\mage_etc1.tex

cd ..\android\ChrisslyEngine_Android

copy ..\..\assets\meshes\mage.mesh assets

