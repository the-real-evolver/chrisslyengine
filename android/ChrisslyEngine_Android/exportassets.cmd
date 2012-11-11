:: Name:        exportassets.bat
:: Autor:       evolver
:: Description: export assets for Android platform
:: History:     26.05.2010 -> creation

cd ..\..\tools

texturetool.exe -format PF_ETC1_RGB8 -width 512 -height 512 -data ..\assets\textures\cerberus_etc1.raw -out ..\android\ChrisslyEngine_Android\assets\cerberus_etc1.tex

meshtool.exe -morphanim -src ..\assets\animations\cerberus\cerberus_walk -dst ..\android\ChrisslyEngine_Android\assets\cerberus_walk.mesh -length 3.0 -numKeys 31

cd ..\android\ChrisslyEngine_Android


