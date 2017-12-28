:: Name:        export_audio.cmd
:: Autor:       evolver
:: Description: export audio

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\audio
set DstFolder=%ProjectFolder%\windows\ChrisslyEngine_Windows

XCOPY /y /i %SrcFolder%\intro.ogg %DstFolder%\intro.ogg*

cd %ProjectFolder%\windows