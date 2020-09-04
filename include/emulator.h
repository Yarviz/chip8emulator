#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <conio.h>
#include <stdlib.h>

#include "com.h"
#include "screen.h"
#include "filehandle.h"
#include "cpu.h"

#define DEFAULT_CLOK_SPEED   540        // Emulators default speed in Hz

using namespace std;

class Emulator : public Com
{
    public:
        Emulator();
        virtual ~Emulator();

        void start();                   // Start emulator
        int typeText();                 // Get typed command from user

    private:
        int readText(std::string txt);  // Process users command
        void help();                    // Show all acceptable commands
        void viewProgramSource();       // View loaded programs source code
        std::string decode(uint8_t b1, uint8_t b2);  // Decode loaded program to Chip-8 instructions

        Filehandle File;                // Object for file handling
        std::vector<std::string> files; // List for all programs in directory ".roms\"
        std::vector<uint8_t> program;   // Raw program in memory
        Cpu cpu;                        // Object for Chip-8 central processing unit
};

#endif // EMULATOR_H
