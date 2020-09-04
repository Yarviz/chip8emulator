#ifndef COM_H
#define COM_H

#include <iostream>
#include <sstream>

class Com
{
    protected:
        const std::string reset = "\033[0m";        // ANSI codes for colors
        const std::string red = "\033[31m";
        const std::string green = "\033[32m";
        const std::string yellow = "\033[33m";

        std::string curLeft(int n);                 // Functions for cursor movement on console
        std::string curRight(int n);
        std::string curUp(int n);
        std::string curDown(int n);
        std::string curSave();                      // Save/load cursors position
        std::string curLoad();
        std::string curHide();                      // Hide/show cursor
        std::string curShow();
};

#endif // COM_H
