#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <iostream>

namespace col {
    const static std::string reset  = "\033[0m";
    const static std::string red    = "\033[31m";
    const static std::string green  = "\033[32m";
    const static std::string yellow = "\033[33m";
};

/*#include <iostream>
#include <windows.h>

enum class col {
    reset       = 0,
    red         = 31,
    green       = 32,
    yellow      = 33
};

class color {
    public:
        friend std::ostream& operator<<(std::ostream& os, const col c)
        {
            #ifdef _WIN32
                std::cout << "\x1b[" << (int)c << "m";
            #endif // _WIN32
            return os;
        }
};*/

#endif // COLOR_H_INCLUDED
