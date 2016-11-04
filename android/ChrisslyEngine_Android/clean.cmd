:: Name:        clean.cmd
:: Autor:       evolver
:: Description: clean the build folder

RD /S /Q obj
RD /S /Q libs
RD /S /Q bin
RD /S /Q gen

set NDK=C:\dev\android\android-ndk-r8
%NDK%\ndk-build clean