:: Name:        export_materials.cmd
:: Autor:       evolver
:: Description: export materials

:: setup variables
set ProjectFolder=%CD%\..
set SrcFolder=%ProjectFolder%\assets\materials
set DstFolder=%ProjectFolder%\psp\export

xcopy /y /i %SrcFolder%\materials.material %DstFolder%\materials.material*

cd %ProjectFolder%\psp