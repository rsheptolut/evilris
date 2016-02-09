mkdir bld
cd bld
%cc65path%\bin\cc65.exe -t nes -I %cc65path%\include ..\src\%prgname%.c -o %prgname%.s
IF ERRORLEVEL 1 GOTO ERROR
%cc65path%\bin\ca65.exe -t nes -l %prgname%.s
IF ERRORLEVEL 1 GOTO ERROR
%cc65path%\bin\ld65.exe -t nes -o ..\%prgname%.nes %prgname%.o %cc65path%\lib\nes.lib
IF ERRORLEVEL 1 GOTO ERROR

cd ..
start %emulator% %CD%\%prgname%.nes
EXIT
:ERROR
PAUSE