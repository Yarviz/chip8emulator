#include "emulator.h"

Emulator::Emulator()
{
    program.clear();
}

Emulator::~Emulator()
{
    //delete(File);
}

void Emulator::start()  // Start emulator
{
    cout << endl << yellow << std::string("   --- Chip-8 command line emulator ---") << endl << endl;
    cout << reset << " Type command " << green << "help" << reset;
    cout << " for list of commands." << endl << endl;
}

int Emulator::typeText()    // Get typed command from user
{
    string txt;
    cout << ">";
    std::getline(cin, txt);

    return readText(txt);
}

int Emulator::readText(std::string txt)     // Process users command
{
    std::vector<string> commands = {"exit", "help", "list", "load", "run", "source"};

    std::istringstream word_string(txt);
    std::vector<std::string> words;

    while (word_string)         // Cut typed command to individual words
    {
        string word;
        word_string >> word;
        if (word != "") words.push_back(word);
    }

    // Iterate typed command trough acceptable commands

    for (std::vector<string>::iterator it = commands.begin(); it < commands.end(); ++it)
    {
        if (txt.size() >= it->size())
        {
            if (words[0] == *it)
            {
                switch (it - commands.begin())
                {
                case 0:     // Exit program
                    return 0;
                    break;

                case 1:     // List all commands
                    help();
                    return 1;
                    break;

                case 2:     // List all Chip-8 programs in ".\roms"
                    File.getFiles(&files);
                    if (files.size() > 0)
                    {
                        cout << red << endl;
                        for (std::string file : files)
                            cout << " " << file << endl;
                        cout << reset << endl;
                    }
                    return 1;
                    break;

                case 3:     // Load program in memory
                    if (words.size() > 1)
                    {
                        if (File.loadFile(&program, words[1]))
                        {
                            cout << endl << " " << words[1] << " loaded succesfully" << endl << endl;
                        }
                        else
                            cout << endl << " file loading failed!" << endl << endl;
                    }
                    return 1;
                    break;

                case 4:     // Run loaded program
                    if (program.size())
                    {
                        cpu.load(program);
                        if (words.size() > 1)
                        {
                            int clk_speed = atoi(words[1].data());
                            if (clk_speed > 0) cpu.run(clk_speed);
                                else cpu.run(DEFAULT_CLOK_SPEED);
                        }
                        else cpu.run(DEFAULT_CLOK_SPEED);
                    }
                    else cout << endl << " no program in memory!" << endl << endl;
                    return 1;
                    break;

                case 5:     // View loaded programs source code
                    if (program.size()) viewProgramSource();
                    else
                        cout << endl << " no program in memory" << endl << endl;
                    return 1;
                    break;
                }
            }
        }
    }

    if (txt.size() > 0) cout << endl << " Undefined command!" << endl << endl;
    return 1;
}

void Emulator::help()   // Show all acceptable commands
{
    std::vector<string> commands = {"help", "exit", "list", "load (program)", "run (Hz)", "source"};
    std::vector<string> infos = {"list of commands", "close emulator", "list of chip-8 programs",
                                 "load program in memory", "start program (optional clock speed variable)", "view programs source code"};

    cout << endl;
    for (int i = 0; i < commands.size(); ++i)
    {
        cout << " " << green << commands[i] << std::string(20 - commands[i].size(), ' ');
        cout << reset << "-> " << infos[i] << endl;
    }
    cout << endl;
}

void Emulator::viewProgramSource()  // View loaded programs source code
{
    int lines = 0;
    int memory_pt = 512;
    uint8_t b1, b2;
    std::string hex_bytes;

    // Addr: Address in ROM starting from 0x200
    // Hex:  Chip-8 instruction in 16-bit hexadecimal number
    // Com:  Hexadecimal number decoded to Chip-8 instruction. "---" means undefined instruction,
    //       maybe direct bit data. Some instructions can also be similar with programs data.

    cout << endl << yellow << " Addr   " << red << "Hex   " << reset << "Com";
    cout << endl << endl;
    for (std::vector<uint8_t>::iterator it = program.begin(); it != program.end(); ++it)
    {
        cout << yellow << " 0x" << hex << setfill('0') << setw(3) << memory_pt << "  ";
        cout << red << hex << setfill('0') << setw(2) << (int)*it;
        b1 = *it;
        if (it + 1 == program.end()) hex_bytes = decode(b1, 0);
            else
            {
                b2 = *it++;
                hex_bytes = decode(b1, b2);
            }
        cout << hex << setfill('0') << setw(2) << (int)*it << "  " << reset << hex_bytes << endl;
        lines++;
        memory_pt += 2;
        if (lines == 25) {getch();lines = 0;}
    }
    cout << reset << endl;
}

std::string Emulator::decode(uint8_t b1, uint8_t b2)    // Decode loaded program to Chip-8 instructions
{
    std::stringstream hex_string, op_code;
    uint16_t bits = b1 << 8 | b2, bits_op;      // Merge two 8-bit variables to one 16-bit variable

    // Chip-8 instructions in hexadecimal numbers
    uint16_t op_codes[] = {0x00e0, 0x00ee, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x8001,
                           0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007, 0x800e, 0x9000, 0xa000, 0xb000, 0xc000,
                           0xd000, 0xe09e, 0xe0a1, 0xf007, 0xf00a, 0xf015, 0xf018, 0xf01e, 0xf029, 0xf033, 0xf055,
                           0xf065};

    // Masks for instructions to compare with bits
    uint16_t op_masks[] = {0xffff, 0xffff, 0xf000, 0xf000, 0xf000, 0xf000, 0xf00f, 0xf000, 0xf000, 0xf00f, 0xf00f,
                           0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf00f, 0xf000, 0xf000, 0xf000,
                           0xf000, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff, 0xf0ff,
                           0xf0ff};

    for (int i = 0; i < 23; i++)
    {
        bits_op = bits & op_masks[i];   // AND readed bits with current instructions meaning bits
        if (!(bits_op ^ op_codes[i]))   // XOR bits_op with current instruction if they match
        {
            switch (i) {
                case 0: op_code << "CLS"; break;
                case 1: op_code << "RET"; break;
                case 2: op_code << "JP $" << hex << (bits & 0x0fff); break;
                case 3: op_code << "CALL $"  << hex << (bits & 0x0fff); break;
                case 4: op_code << "SE V" << hex << (bits & 0x0f00 >> 16) << ",#$" << hex << (bits & 0x00ff); break;
                case 5: op_code << "SNE V" << hex << (bits & 0x0f00 >> 16) << ",#$" << hex << (bits & 0x00ff); break;
                case 6: op_code << "SE V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 7: op_code << "LD V" << hex << (bits & 0x0f00 >> 16) << ",#$" << hex << (bits & 0x00ff); break;
                case 8: op_code << "ADD V" << hex << (bits & 0x0f00 >> 16) << ",#$" << hex << (bits & 0x00ff); break;
                case 9: op_code << "LD V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 10: op_code << "OR V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 11: op_code << "AND V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 12: op_code << "XOR V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 13: op_code << "ADD V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 14: op_code << "SUB V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 15: op_code << "SHR V" << hex << (bits & 0x0f00 >> 16) << ",{V" << hex << (bits & 0x00f0 >> 8) << "}"; break;
                case 16: op_code << "SUBN V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 17: op_code << "SHL V" << hex << (bits & 0x0f00 >> 16) << ",{V" << hex << (bits & 0x00f0 >> 8) << "}"; break;
                case 18: op_code << "SNE V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8); break;
                case 19: op_code << "LD I,$" << hex << (bits & 0x0fff); break;
                case 20: op_code << "JP V0,$" << hex << (bits & 0x0fff); break;
                case 21: op_code << "RND V" << hex << (bits & 0x0f00 >> 16) << ",#$" << hex << (bits & 0x00ff); break;
                case 22: op_code << "DRW V" << hex << (bits & 0x0f00 >> 16) << ",V" << hex << (bits & 0x00f0 >> 8) << ",#$" << hex << (bits & 0x000f); break;
                case 23: op_code << "SKP V" << hex << (bits & 0x0f00 >> 16); break;
                case 24: op_code << "SKNP V" << hex << (bits & 0x0f00 >> 16); break;
                case 25: op_code << "LD V" << hex << (bits & 0x0f00 >> 16) << ",DT"; break;
                case 26: op_code << "LD V" << hex << (bits & 0x0f00 >> 16) << ",K"; break;
                case 27: op_code << "LD DT,V" << hex << (bits & 0x0f00 >> 16); break;
                case 28: op_code << "LD ST,V" << hex << (bits & 0x0f00 >> 16); break;
                case 29: op_code << "ADD I,V" << hex << (bits & 0x0f00 >> 16); break;
                case 30: op_code << "LD F,V" << hex << (bits & 0x0f00 >> 16); break;
                case 31: op_code << "LD B,V" << hex << (bits & 0x0f00 >> 16); break;
                case 32: op_code << "LD [I],V" << hex << (bits & 0x0f00 >> 16); break;
                case 33: op_code << "LD V" << hex << (bits & 0x0f00 >> 16) << ",[I]"; break;
            }
            return op_code.str();   // Return decoded instruction in string
        }
    }
    return std::string("---");      // Undefined instruction
}
