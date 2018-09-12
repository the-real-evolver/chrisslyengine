:: Name:        clean.cmd
:: Author:      evolver
:: Description: clean the build folder

rd /s /q obj
rd /s /q libs
rd /s /q bin
rd /s /q gen

set NDK=C:\dev\android\android-ndk-r8
%NDK%\ndk-build clean