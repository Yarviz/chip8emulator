#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <vector>
#include <conio.h>
#include <future>

#include "screen.h"
#include "keyboard.h"

#define STATE_HOLD      0       // Cpu running states
#define STATE_LOAD      1
#define STATE_EXECUTE   2

using namespace std;

class Cpu
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
        double clockToMs(clock_t ticks);
        void executeInstruction();
        void drawSprite(int x, int y, int nibble);
        void BCD_decode(int v);
        int hexInt(std::string str);

        uint8_t ROM[4096];
        uint16_t Stack[16];
        uint8_t V[16];
        uint8_t DT;
        uint8_t ST;
        uint8_t SP;
        uint16_t I;
        uint16_t PC;

        clock_t cpu_time, delay_time;
        bool cpu_run;
        int cpu_Hz, delay_Hz;
        uint8_t cpu_state;
        uint16_t op_codes[34];
        uint16_t op_masks[34];

        Screen renderer;
        Keyboard keyboard;
};

#endif // CPU_H
