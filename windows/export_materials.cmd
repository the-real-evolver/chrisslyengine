:: Name:        export_materials.cmd
:: Author:      evolver
:: Description: export materials

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\materials
set DstFolder=%ProjectFolder%\windows\export

xcopy /y /i %SrcFolder%\materials.material %DstFolder%\materials.material*

cd %ProjectFolder%\windows