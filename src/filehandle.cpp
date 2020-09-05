#include "filehandle.h"

Filehandle::Filehandle()
{
    //ctor
}

Filehandle::~Filehandle()
{
    //dtor
}

// Get list of programs in "rom\"-directory
void Filehandle::getFiles(std::vector<std::string> *file_list)
{
    file_list->clear();

    DIR *rom_dir;
    char *f1, *f2;
    struct dirent *rom_file;

    rom_dir = opendir("roms");
    if (rom_dir != NULL)
    {
        while ((rom_file = readdir(rom_dir)) != NULL)
        {
            f1 = strtok(rom_file->d_name, ".");
            f2 = strtok(NULL, ".");
            if (f2 != NULL)
            {
                if (strcmp(f2, "ch8") == 0)
                {
                    file_list->push_back(rom_file->d_name);
                }
            }
        }
    }
    closedir(rom_dir);
}

// Load program from "rom\"-directory
bool Filehandle::loadFile(std::vector<uint8_t> *rom, std::string filename)
{
    std::string file_name = "roms/" + filename + ".ch8";
    std::ifstream file;

    file.open(file_name.data(), std::ios::in | std::ios::binary);
    if (file.is_open())
    {
        rom->clear();
        file.seekg(0, file.end);
        int file_size = file.tellg();
        rom->reserve(file_size);
        file.seekg(0, file.beg);

        while(true)
        {
            uint8_t b;
            file.read((char *) &b, sizeof(char));
            if (file.eof()) break;
            rom->push_back(b);
        }
        file.close();
        return true;
    }
    return false;
}
