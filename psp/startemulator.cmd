:: Name:        startemulator.bat
:: Autor:       evolver
:: Description: starts PSP Emulator ("PPSSPP")
:: History:     26.05.2010 -> creation

:: open PPSSPP
XCOPY /y export C:\dev\ppsspp_win\memstick\PSP\GAME\ChrisslyEngine_PSP\export
cd C:\dev\ppsspp_win
PPSSPPWindows.exe
