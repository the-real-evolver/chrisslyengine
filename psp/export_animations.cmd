:: Name:        export_animations.cmd
:: Autor:       evolver
:: Description: export animations
:: History:     30.10.2010 -> creation

cd ..\tools

:: convert keyframes as obj to mesh

meshtool.exe -morphanim -src ..\assets\animations\cerberus\cerberus_walk -dst ..\psp\export\cerberus_walk.mesh -length 3.0 -numKeys 31

cd ..\psp
