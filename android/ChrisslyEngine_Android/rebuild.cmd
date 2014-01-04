:: Name:        rebuild.cmd
:: Autor:       evolver
:: Description: copy code to 'jni' folder, compile native code, build shared libary
:: History:     13.05.2012 -> creation

:: setup variables
set NDK=C:\dev\android\android-ndk-r8
set CE=..\..

XCOPY /i /y %CE%\code\core             %CE%\android\ChrisslyEngine_Android\jni\core
XCOPY /i /y %CE%\code\core\android     %CE%\android\ChrisslyEngine_Android\jni\core\android
XCOPY /i /y %CE%\code\graphics         %CE%\android\ChrisslyEngine_Android\jni\graphics
XCOPY /i /y %CE%\code\graphics\android %CE%\android\ChrisslyEngine_Android\jni\graphics\android

%NDK%\ndk-build
