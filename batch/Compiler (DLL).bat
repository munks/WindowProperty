@echo off
echo Compile Start

cd ../DLL

g++32 main.cpp -o ../bin/dllinjector_x86.exe -mwindows -municode
g++32 dll.cpp -o ../bin/cmdcapture_x86.dll -mwindows -shared
g++ main.cpp -o ../bin/dllinjector_x64.exe -mwindows -municode
g++ dll.cpp -o ../bin/cmdcapture_x64.dll -mwindows -shared
pause