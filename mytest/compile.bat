cd D:\Projects\CC65\mytest
set prgname=mytest
call ..\env

mkdir bld
copy /b /y src\* bld\*
cd bld
%neshelp% -bin2h ..\gfx\palette.bin bin_palette.h bin_palette
%cc65path%\bin\cc65.exe -t nes -I %cc65path%\include %prgname%.c -o %prgname%.s
IF ERRORLEVEL 1 GOTO ERROR
%cc65path%\bin\ca65.exe -t nes -l %prgname%.s
IF ERRORLEVEL 1 GOTO ERROR
%cc65path%\bin\ca65.exe -t nes -l chars.s
IF ERRORLEVEL 1 GOTO ERROR
%cc65path%\bin\ld65.exe -t nes -m %prgname%.amp -vm -o ..\%prgname%.nes %prgname%.o chars.o
IF ERRORLEVEL 1 GOTO ERROR

cd ..
start %emulator% %CD%\%prgname%.nes
EXIT
:ERROR
PAUSE

