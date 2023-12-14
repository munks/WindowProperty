@echo off
echo Compile Start

rem Variable
setlocal

set FILES=main.cpp control.cpp menu.cpp log.cpp util.cpp process.cpp dlgfunction.cpp vk.cpp hook.cpp debug.cpp

rem Path
if not exist ../bin mkdir ..\bin
cd ../src

rem Resource Compile
windres resource.rc -O coff ../bin/resource.res -I ../Include

rem Executable Compile
x86_64-w64-mingw32-g++ -I../include %FILES% -o ../bin/WindowProperty.exe ../bin/resource.res -mwindows -static-libgcc -static-libstdc++ -Wno-int-to-pointer-cast -mconsole -D_DEBUG

pause