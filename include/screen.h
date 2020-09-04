#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

#include "com.h"

using namespace std;

class Screen : public Com
{
    public:
        Screen();
        virtual ~Screen();

        void clr();                     // Clear screen and erase screen buffer
        void draw(bool cur_up = true);  // Draw buffer on screen
        void start();                   // Start renderer
        void drawKeyboard();            // Draw keys on screen
        uint8_t drawSprite(int x, int y, std::vector<uint8_t> sprite);  // Draw sprite on screen

    private:
        int width, height;              // Screen width, height
        std::vector<uint8_t> buffer;    // Screen buffer where pixels are stored
};

#endif // SCREEN_H
