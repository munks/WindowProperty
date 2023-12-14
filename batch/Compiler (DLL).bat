@echo off
echo Compile Start

cd ../DLL

i686-w64-mingw32-g++ main.cpp -o ../bin/dllinjector_x86.exe -mwindows -municode -static-libgcc -static-libstdc++
i686-w64-mingw32-g++ dll.cpp -o ../bin/cmdcapture_x86.dll -mwindows -shared -static-libgcc -static-libstdc++
x86_64-w64-mingw32-g++ main.cpp -o ../bin/dllinjector_x64.exe -mwindows -municode -static-libgcc -static-libstdc++
x86_64-w64-mingw32-g++ dll.cpp -o ../bin/cmdcapture_x64.dll -mwindows -shared -static-libgcc -static-libstdc++
pause