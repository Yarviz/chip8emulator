#include "cpu.h"

Cpu::Cpu()
{
    init();
}

Cpu::~Cpu()
{
    //dtor
}

void Cpu::reset()
{
    memset(&ROM[0], 255, sizeof(ROM));
    memset(&Stack[0], 0, sizeof(Stack));
    memset(&V[0], 0, sizeof(V));
    loadFonts();
    DT = 0;
    ST = 0;
    I = 0;
    PC = 512;
    SP = 0;
    cpu_state = STATE_HOLD;
}

void Cpu::init()
{
    reset();
    cpu_Hz = 500;
    delay_Hz = 60;

    srand((unsigned)time(NULL));

    uint16_t op_codes_[34] = {0x00e0, 0x00ee, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x8001,
                              0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007, 0x800e, 0x9000, 0xa000, 0xb000, 0xc000,
                              0xd000, 0xe09e, 0xe0a1, 0xf007, 0xf00a, 0xf015, 0xf018, 0xf01e, 0xf029, 0xf033, 0xf055,
                              0xf065};

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

double Cpu::clockToMs(clock_t ticks)
{
    return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

void Cpu::loadFonts()
{
    uint8_t font_data[80] = {0xF0, 0x90, 0x90, 0x90, 0xF0,
                             0x20, 0x60, 0x20, 0x20, 0x70,
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

    for (int i = 0; i < 80; i++) ROM[i] = font_data[i];
}

void Cpu::load(std::vector<uint8_t> program)
{
    reset();
    int c = 512;
    for (auto i: program)
    {
        ROM[c] = i;
        c++;
        if (c == 4096) break;
    }
}

int Cpu::hexInt(std::string str)
{
    int x;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> x;
    return x;
}

void Cpu::drawSprite(int x, int y, int nibble)
{
    std::vector<uint8_t> sprite;
    for (int i = I; i < I + nibble; i++)
    {
        sprite.push_back(ROM[i]);
    }
    V[15] = renderer.drawSprite(x, y, sprite);
}

void Cpu::BCD_decode(int v)
{
    std::stringstream BCD_string;
    BCD_string << (int)V[v];
    std::string BCD_byte = BCD_string.str();
    BCD_byte = std::string(3 - BCD_byte.length(), '0') + BCD_byte;
    ROM[I] = (uint8_t)(BCD_byte[0] - '0');
    ROM[I + 1] = (uint8_t)(BCD_byte[1] - '0');
    ROM[I + 2] = (uint8_t)(BCD_byte[2] - '0');
}

void Cpu::executeInstruction()
{
    uint16_t bytes = ROM[PC] << 8 | ROM[PC + 1];
    uint16_t I_temp, bytes_op;

    uint8_t n1_1 = (bytes & 0x0f00) >> 8;
    uint8_t n2_1 = (bytes & 0x00f0) >> 4;
    uint8_t n2_2 = bytes & 0x00ff;
    uint8_t n3_1 = bytes & 0x000f;
    uint16_t n1_3 = bytes & 0x0fff;

    for (int i = 0; i < 34; i++)
    {
        bytes_op = bytes & op_masks[i];
        if (!(bytes_op ^ op_codes[i]))
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

void Cpu::run(int cpu_speed)
{
    cpu_Hz = cpu_speed;
    cpu_time = delay_time = clock();
    cpu_state = STATE_LOAD;
    cpu_run = true;

    cout << endl;
    renderer.draw();
    renderer.clr();
    renderer.drawKeyboard();

    keyboard.clearKeys();

    while (cpu_run)
    {
        clock_t cpu_cycle = clock();

        if (clockToMs(cpu_cycle - cpu_time) > 1000.0 / (double)cpu_Hz)
        {
            cpu_time = clock();
            switch (cpu_state)
            {
                case STATE_HOLD:
                    break;
                case STATE_LOAD:
                    if (PC >= 4096) cpu_run = false;
                    cpu_state = STATE_EXECUTE;
                    break;
                case STATE_EXECUTE:
                    keyboard.readKeys();
                    if (keyboard.getKey(16)) cpu_run = false;
                    if (keyboard.getKey(17))
                    {
                        while(_kbhit()) getch();
                        getch();
                    }
                    executeInstruction();
                    cpu_state = STATE_LOAD;
                    break;
            }
        }
        if (clockToMs(cpu_cycle - delay_time) > 1000.0 / (double)delay_Hz)
        {
            delay_time = clock();
            if (DT > 0) DT--;
            if (ST > 0) ST--; // No sound effects programmed
        }
    }
    while(_kbhit()) getch();
    cout << endl;
}

int summa(int *t, int N)
{
    int tulos = 0;
    for (int i = 0; i < N; i++) tulos += t[i];
    return tulos;
}
