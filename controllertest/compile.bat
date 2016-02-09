..\..\bin\cc65.exe -t nes -I ..\..\include controllertest.c
..\..\bin\ca65.exe -t nes -l controllertest.s
..\..\bin\ld65.exe -t nes -o controllertest.nes controllertest.o ..\..\lib\nes.lib
ECHO %ERRORLEVEL%
IF ERRORLEVEL 1 GOTO ERROR
D:\Games\Emulation\NES\Emul\FCEu\fceu.exe tetris.nes
EXIT
:ERROR
PAUSE