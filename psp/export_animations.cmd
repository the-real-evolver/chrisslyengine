:: Name:        export_animations.cmd
:: Autor:       evolver
:: Description: export animations

cd ..\tools

:: convert keyframes as obj to mesh

meshtool.exe -morphanim -src ..\assets\animations\cerberus\cerberus_walk -dst ..\psp\export\cerberus_walk.mesh -length 3.0 -numKeys 31

cd ..\psp
