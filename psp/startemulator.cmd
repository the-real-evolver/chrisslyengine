:: Name:        startemulator.bat
:: Autor:       evolver
:: Description: starts PSP Emulator ("jpcsp") 
:: History:     26.05.2010 -> creation

:: open JPCSP
XCOPY /y export C:\dev\devkitpro\emulators\jpcsp0.5win64(rev1379)\ms0\PSP\GAME\ChrisslyEngine_PSP\export
cd C:\dev\devkitpro\emulators\jpcsp0.5win64(rev1379)
jpcsp0.5.bat
