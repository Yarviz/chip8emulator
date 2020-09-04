#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include <fstream>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>

using namespace std;

class Filehandle
{
    public:
        Filehandle();
        virtual ~Filehandle();

        void getFiles(std::vector<std::string> *file_list);                 // Get list of programs in "rom\"-directory
        bool loadFile(std::vector<uint8_t> *rom, std::string filename);     // Load program from "rom\"-directory
};

#endif // FILEHANDLE_H
