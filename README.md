# chip8emulator
Chip-8 emulator running in Windows command prompt. 10 (public domain) roms included in roms-directory.

Compiled in Windows with Mingw and gcc version 5.1.0 (tdm-1) Thread model: posix

g++ -std=c++11 -Iinclude -c "main.cpp" "src\\*.cpp"

g++ -o chi8.exe main.o com.o emulator.o keyboard.o filehandle.o screen.o cpu.o
