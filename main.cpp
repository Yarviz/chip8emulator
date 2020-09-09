//
//             Chip-8 Command Line Emulator v 1.0
//                  by Jussi Laitala in 2020
//
//
//     Emulator can run standard chip-8 prgrams locating
//     in folder ".\roms\*.ch8".
//
//     This program is free to use, copy or modify.


#include <iostream>

#ifdef _WIN32

    #include <windows.h>

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING              // Enable virtual processing for ANSI escape codes in windows console
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

#endif // _WIN32

using namespace std;

#include "emulator.h"

int main()
{
    #ifdef _WIN32

        HANDLE hConsole;
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        SetConsoleOutputCP(850);                        // Sets the console output code page
        GetConsoleMode(hConsole, &dwMode);

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);

    #endif // _WIN32

    Emulator emu;
    int ext = 1;

    emu.start();

    while (ext == 1)
    {
        ext = emu.typeText();       // Run emulator until user quits program
    }
    return 0;
}
