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

        void clr();
        void draw();
        void start();
        void drawKeyboard();
        uint8_t drawSprite(int x, int y, std::vector<uint8_t> sprite);

    protected:

    private:
        int width, height;
        std::vector<uint8_t> buffer;
};

#endif // SCREEN_H
