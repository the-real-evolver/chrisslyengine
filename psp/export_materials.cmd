:: Name:        export_materials.cmd
:: Autor:       evolver
:: Description: export materials
:: History:     30.10.2010 -> creation

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\materials
set DstFolder=%ProjectFolder%\psp\export

XCOPY /y /i %SrcFolder%\materials.material %DstFolder%\materials.material

cd %ProjectFolder%\psp