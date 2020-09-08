#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>
#elif __linux
    #include <ncurses.h>
#endif // _WIN32

#include "screen.h"
#include "keyboard.h"

#define STATE_HOLD      0       // Cpu running states
#define STATE_EXECUTE   1

using namespace std;

class Cpu : public Com
{
    public:
        Cpu();
        virtual ~Cpu();

        void load(std::vector<uint8_t> program);    // Load program in ROM
        void run(int cpu_speed = 100);              // Start Cpu and set clock speed(Hz)

    private:
        void init();                                // Init Cpu
        void reset();                               // Reset Cpu
        void loadFonts();                           // Load Chip-8 fonts (0 - F) in ROM[0x000 - 0x050]
        double clockToMs(clock_t ticks);            // Convert clock ticks to milliseconds
        void executeInstruction();                  // Execute instruction from ROM-address where program counter points
        void drawSprite(int x, int y, int nibble);  // Draw sprite on screen with renderer
        void BCD_decode(int v);                     // Decode byte from register V[v] to individual numbers
        int hexInt(std::string str);                // Change hexadecimal string to integer

        uint8_t ROM[4096];                          // Random Access Memory for program and sprites
        uint16_t Stack[16];                         // Stack where PC is stored after CALL
        uint8_t V[16];                              // 8-bit registers V1-VF
        uint8_t DT;                                 // Delay timer
        uint8_t ST;                                 // Sound timer
        uint8_t SP;                                 // Stack pointer
        uint16_t I;                                 // Register where 12-bit addresses are stored
        uint16_t PC;                                // Program counter

        clock_t cpu_time, delay_time;               // Clocks for Cpu and delay timer
        bool cpu_run;                               // Boolean for program running\exit
        int cpu_Hz;                                 // Cpu clock Hz
        uint8_t cpu_state;                          // Variable for Cpu state
        uint16_t op_codes[34];                      // Hexadecimal chip-8 instructions
        uint16_t op_masks[34];                      // Masks for hexadecimal instructions

        Screen renderer;                            // Object for rendering sprites
        Keyboard keyboard;                          // Object for keyboard input
};

#endif // CPU_H
