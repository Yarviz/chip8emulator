#ifndef COM_H
#define COM_H

#include <iostream>
#include <sstream>

class Com
{
    protected:
        const std::string reset = "\033[0m";
        const std::string red = "\033[31m";
        const std::string green = "\033[32m";
        const std::string yellow = "\033[33m";

        std::string curLeft(int n);
        std::string curRight(int n);
        std::string curUp(int n);
        std::string curDown(int n);
        std::string curSave();
        std::string curLoad();
};

#endif // COM_H
