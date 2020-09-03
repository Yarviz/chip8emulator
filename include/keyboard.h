#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <iostream>

#define MAX_KEYS   18

class Keyboard
{
    public:
        Keyboard();
        virtual ~Keyboard();

        void readKeys();
        void clearKeys();
        bool getKey(int key_code);
        uint8_t waitKeyPressed();

    private:
        bool key[MAX_KEYS];
};

#endif // KEYBOARD_H
