:: Name:        startemulator.cmd
:: Autor:       evolver
:: Description: starts PSP Emulator ("PPSSPP")

:: open PPSSPP
xcopy /i /y export C:\dev\ppsspp_win\memstick\PSP\GAME\ChrisslyEngine_PSP\export
cd C:\dev\ppsspp_win
PPSSPPWindows.exe
