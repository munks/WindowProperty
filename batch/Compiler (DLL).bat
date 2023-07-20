@echo off
echo Compile Start

cd ../DLLInjection

g++32 main.cpp -o ../bin/dllinjector_x86.exe -mwindows -municode
g++32 cmdline.cpp -o ../bin/cmdline_x86.dll -mwindows -shared
g++ main.cpp -o ../bin/dllinjector_x64.exe -mwindows -municode
g++ cmdline.cpp -o ../bin/cmdline_x64.dll -mwindows -shared
pause