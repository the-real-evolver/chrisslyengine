:: Name:        exportassets.cmd
:: Autor:       evolver
:: Description: export assets for Android platform
:: History:     26.05.2010 -> creation

cd ..\..\tools

texturetool.exe -data ..\assets\textures\cerberus_etc1.pkm -out ..\android\ChrisslyEngine_Android\assets\cerberus_etc1.tex -pkm

meshtool.exe -morphanim -src ..\assets\animations\cerberus\cerberus_walk -dst ..\android\ChrisslyEngine_Android\assets\cerberus_walk.mesh -length 3.0 -numKeys 31

XCOPY /i /y ..\assets\materials\materials.material ..\android\ChrisslyEngine_Android\assets\materials.material

XCOPY /i /y ..\assets\audio\intro.ogg ..\android\ChrisslyEngine_Android\assets\intro.ogg

cd ..\android\ChrisslyEngine_Android


