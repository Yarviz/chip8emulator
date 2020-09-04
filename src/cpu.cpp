#include "cpu.h"

Cpu::Cpu()
{
    init();
}

Cpu::~Cpu()
{
    //dtor
}

void Cpu::reset()   // Reset Cpu
{
    memset(&ROM[0], 0, sizeof(ROM));      // Empty memory and registers
    memset(&Stack[0], 0, sizeof(Stack));
    memset(&V[0], 0, sizeof(V));
    loadFonts();                          // Load fonts in ROM
    DT = 0;
    ST = 0;
    I = 0;
    PC = 512;                            // Set program counter to 0x200
    SP = 0;
    cpu_state = STATE_HOLD;
}

void Cpu::init()    // Init Cpu
{
    reset();
    cpu_Hz = 500;       // Default clock speeds for processor and delay timer

    srand((unsigned)time(NULL));

    // Chip-8 instructions in hexadecimal numbers
    uint16_t op_codes_[34] = {0x00e0, 0x00ee, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x8001,
                              0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007, 0x800e, 0x9000, 0xa000, 0xb000, 0xc000,
                              0xd000, 0xe09e, 0xe0a1, 0xf007, 0xf00a, 0xf015, 0xf018, 0xf01e, 0xf029, 0xf033, 0xf055,
                              0xf065};

    // Masks for instructions to compare with bits
    uint16_t op_masks_[34] = {0xffff, 0xffff, 0xf000, 0xf000, 0xf000, 0xf000, 0xf00f, 0xf000, 0xf000, 0xf00f, 0xf00f,
                              0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf000, 0xf000, 0xf000,
                              0xf000, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff,
                              0xf0ff};

    for (int i = 0; i < 34; i++)
    {
        op_codes[i]= op_codes_[i];
        op_masks[i]= op_masks_[i];
    }
}

double Cpu::clockToMs(clock_t ticks)    // Convert clock ticks to milliseconds
{
    return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

void Cpu::loadFonts()   // Load Chip-8 fonts (0 - F) in ROM[0x000 - 0x050]
{
    uint8_t font_data[80] = {0xF0, 0x90, 0x90, 0x90, 0xF0,  // Font data stored in bits.
                             0x20, 0x60, 0x20, 0x20, 0x70,  // One font is five bytes long (8x5 pixels)
                             0xF0, 0x10, 0xF0, 0x80, 0xF0,
                             0xF0, 0x10, 0xF0, 0x10, 0xF0,
                             0x90, 0x90, 0xF0, 0x10, 0x10,
                             0xF0, 0x80, 0xF0, 0x10, 0xF0,
                             0xF0, 0x80, 0xF0, 0x90, 0xF0,
                             0xF0, 0x10, 0x20, 0x40, 0x40,
                             0xF0, 0x90, 0xF0, 0x90, 0xF0,
                             0xF0, 0x90, 0xF0, 0x10, 0xF0,
                             0xF0, 0x90, 0xF0, 0x90, 0x90,
                             0xE0, 0x90, 0xE0, 0x90, 0xE0,
                             0xF0, 0x80, 0x80, 0x80, 0xF0,
                             0xE0, 0x90, 0x90, 0x90, 0xE0,
                             0xF0, 0x80, 0xF0, 0x80, 0xF0,
                             0xF0, 0x80, 0xF0, 0x80, 0x80};

    for (int i = 0; i < 80; i++) ROM[i] = font_data[i];     // Store font data in ROM[0x000 - 0x050]
}

void Cpu::load(std::vector<uint8_t> program)    // Load program in ROM
{
    reset();
    int c = 512;
    for (auto i: program)
    {
        ROM[c] = i;
        c++;
        if (c == 4096) break;   // Stop if can't fit program in 4Kb memory
    }
}

int Cpu::hexInt(std::string str)    // Change hexadecimal string to integer
{
    int x;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;
    return x;
}

void Cpu::drawSprite(int x, int y, int nibble)  // Draw sprite on screen with renderer
{                                               // Nibble is 4-bit value for sprite height to draw
    std::vector<uint8_t> sprite;
    for (int i = I; i < I + nibble; i++)
    {
        sprite.push_back(ROM[i]);
    }
    V[15] = renderer.drawSprite(x, y, sprite);  // Store collision info in register VF
}

void Cpu::BCD_decode(int v) // Decode byte from register V[v] to individual numbers
{
    std::stringstream BCD_string;
    BCD_string << (int)V[v];
    std::string BCD_byte = BCD_string.str();
    BCD_byte = std::string(3 - BCD_byte.length(), '0') + BCD_byte;
    ROM[I] = (uint8_t)(BCD_byte[0] - '0');          // Save hundreds in ROM[I], tens in ROM[I + 1]
    ROM[I + 1] = (uint8_t)(BCD_byte[1] - '0');      // and ones in ROM[I + 2].
    ROM[I + 2] = (uint8_t)(BCD_byte[2] - '0');
}

void Cpu::executeInstruction()  // Execute instruction from ROM-address where program counter points
{
    uint16_t bytes = ROM[PC] << 8 | ROM[PC + 1];    // Merge two 8-bit variables to one 16-bit variable
    uint16_t I_temp, bytes_op;

    uint8_t n1_1 = (bytes & 0x0f00) >> 8;           // Byte from position 0x0X00
    uint8_t n2_1 = (bytes & 0x00f0) >> 4;           // Byte from position 0x00X0
    uint8_t n2_2 = bytes & 0x00ff;                  // Byte from position 0x00XX
    uint8_t n3_1 = bytes & 0x000f;                  // Byte from position 0x000X
    uint16_t n1_3 = bytes & 0x0fff;                 // UShort from position 0x0XXX

    for (int i = 0; i < 34; i++)    // Loop trough possible instructions
    {
        bytes_op = bytes & op_masks[i];     // AND readed bits with current instructions meaning bits
        if (!(bytes_op ^ op_codes[i]))      // XOR bits_op with current instruction if they match
        {
            switch (i) {
                case 0: // CLS
                    renderer.clr();
                    PC += 2;
                    break;
                case 1: // RET
                    SP--;
                    PC = Stack[SP];
                    PC += 2;
                    break;
                case 2: // JP addr
                    PC = n1_3;
                    break;
                case 3: // CALL addr
                    Stack[SP] = PC;
                    SP++;
                    PC = n1_3;
                    break;
                case 4: // SE Vx, byte
                    if (V[n1_1] == n2_2) PC += 4;
                        else PC += 2;
                    break;
                case 5: // SNE Vx, byte
                    if (V[n1_1] != n2_2) PC += 4;
                        else PC += 2;
                    break;
                case 6: // SE Vx, Vy
                    if (V[n1_1] == V[n2_1]) PC += 4;
                        else PC += 2;
                    break;
                case 7: // LD Vx, byte
                    V[n1_1] = n2_2;
                    PC += 2;
                    break;
                case 8: // ADD Vx, byte
                    if ((int)V[n1_1] + (int)n2_2 > 255) V[15] = 1; else V[15] = 0;
                    V[n1_1] += n2_2;
                    PC += 2;
                    break;
                case 9: // LD Vx, Vy
                    V[n1_1] = V[n2_1];
                    PC += 2;
                    break;
                case 10: // OR Vx, Vy
                    V[n1_1] = V[n1_1] | V[n2_1];
                    PC += 2;
                    break;
                case 11: // AND Vx, Vy
                    V[n1_1] = V[n1_1] & V[n2_1];
                    PC += 2;
                    break;
                case 12: // XOR Vx, Vy
                    V[n1_1] = V[n1_1] ^ V[n2_1];
                    PC += 2;
                    break;
                case 13: // ADD Vx, Vy
                    if ((int)V[n1_1] + (int)V[n2_1] > 255) V[15] = 1; else V[15] = 0;
                    V[n1_1] += V[n2_1];
                    PC += 2;
                    break;
                case 14: // SUB Vx, Vy
                    if (V[n1_1] > V[n2_1]) V[15] = 1; else V[15] = 0;
                    V[n1_1] -= V[n2_1];
                    PC += 2;
                    break;
                case 15: // SHR Vx
                    if (V[n1_1] & 0x01) V[15] = 1; else V[15] = 0;
                    V[n1_1] = V[n1_1] >> 1;
                    PC += 2;
                    break;
                case 16: // SUBN Vx, Vy
                    if (V[n2_1] > V[n1_1]) V[15] = 1; else V[15] = 0;
                    V[n1_1] = V[n2_1] - V[n1_1];
                    PC += 2;
                    break;
                case 17: // SHL Vx
                    if (V[n1_1] & 0x80) V[15] = 1; else V[15] = 0;
                    V[n1_1] = V[n1_1] << 1;
                    PC += 2;
                    break;
                case 18: // SNE Vx, Vy
                    if (V[n1_1] != V[n2_1]) PC += 4;
                        else PC += 2;
                    break;
                case 19: // LD I, addr
                    I = n1_3;
                    PC += 2;
                    break;
                case 20: // JP V0, addr
                    PC = n1_3 + (uint16_t)V[0];
                    break;
                case 21: // RND Vx, byte
                    V[n1_1] = (rand() % 256) & (uint8_t)n2_2;
                    PC += 2;
                    break;
                case 22: // DRW Vx, Vy, nibble
                    drawSprite((int)V[n1_1], (int)V[n2_1], n3_1);
                    PC += 2;
                    break;
                case 23: // SKP Vx
                    if (keyboard.getKey(V[n1_1])) PC += 2;
                    PC += 2;
                    break;
                case 24: // SKNP Vx
                    if (!keyboard.getKey(V[n1_1])) PC += 2;
                    PC += 2;
                    break;
                case 25: // LD Vx, DT
                    V[n1_1] = DT;
                    PC += 2;
                    break;
                case 26: // LD Vx, K
                    V[n1_1] = keyboard.waitKeyPressed();
                    if (V[n1_1] == 16) cpu_run = false; else PC += 2;
                    break;
                case 27: // LD DT, Vx
                    DT = V[n1_1];
                    PC += 2;
                    break;
                case 28: // LD ST, Vx
                    ST = V[n1_1];
                    PC += 2;
                    break;
                case 29: // ADD I, Vx
                    I += (uint16_t)V[n1_1];
                    PC += 2;
                    break;
                case 30: // LD F, Vx
                    I = (uint16_t)V[n1_1] * 5;
                    PC += 2;
                    break;
                case 31: // LD B, Vx
                    BCD_decode(n1_1);
                    PC += 2;
                    break;
                case 32: // LD [I], Vx
                    I_temp = I;
                    for (int i = 0; i < n1_1 + 1; i++)
                    {
                        ROM[I_temp] = V[i];
                        I_temp++;
                    }
                    PC += 2;
                    break;
                case 33: // LD Vx, [I]
                    I_temp = I;
                    for (int i = 0; i < n1_1 + 1; i++)
                    {
                        V[i] = ROM[I_temp];
                        I_temp++;
                    }
                    PC += 2;
                    break;
            }
            return;
        }
    }
    PC += 2;
}

void Cpu::run(int cpu_speed)    // Start Cpu and set clock speed(Hz)
{
    cpu_Hz = cpu_speed;
    cpu_state = STATE_EXECUTE;
    cpu_run = true;
    float ticks = 0;

    cout << endl << curHide() << flush;

    renderer.start();           // Start renderer
    renderer.draw();
    renderer.drawKeyboard();
    keyboard.clearKeys();       // Clear keybuffer

    chrono::duration<long, ratio<1,60>> cpu_tick_Hz(1); // Screen refresh rate 60Hz

    while (cpu_run)             // Loop while program stop
    {
        auto cpu_timer = std::chrono::steady_clock::now();  //  Start_cpu_timer;
        ticks += (float)cpu_Hz / 60.0;  // Count how many ticks emulate on next frame

        for (int i = 0; i < (int)ticks; i++)
        {
            switch (cpu_state)  // Each tick check Cpu state
            {
                case STATE_HOLD:
                    break;
                case STATE_EXECUTE:
                    keyboard.readKeys();
                    if (keyboard.getKey(16) || PC >= 4096)
                    {
                        cpu_run = false;   // If Esc is pressed, stop
                        cpu_state = STATE_HOLD;
                    }
                    if (keyboard.getKey(17))    // If space is pressed, pause
                    {
                        while(_kbhit()) getch();
                        getch();
                    }
                    executeInstruction();   // Execute next instruction from memory where PC points
                    break;
            }
        }
        ticks -= (int)ticks;
        if (DT > 0) DT--;   // Decrease delay- and sound timers
        if (ST > 0) ST--;   // No sound effects programmed

        std::this_thread::sleep_until(cpu_timer + cpu_tick_Hz);   // Slow emulator down to 60Hz
    }

    cout << endl << curShow() << flush;
    while(_kbhit()) getch();    // Clear keybuffer before exit to console
}
