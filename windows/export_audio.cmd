:: Name:        export_audio.cmd
:: Author:      evolver
:: Description: export audio

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\audio
set DstFolder=%ProjectFolder%\windows\export

xcopy /y /i %SrcFolder%\intro.ogg %DstFolder%\intro.ogg*

cd %ProjectFolder%\windows