#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <iostream>

#define MAX_KEYS   18           // Number of keys to read

class Keyboard
{
    public:
        Keyboard();
        virtual ~Keyboard();

        void readKeys();            // Read keys if they are pressed or not. Store values in keybuffer
        void clearKeys();           // Clear keybuffer
        bool getKey(int key_code);  // Read keys value from buffer
        uint8_t waitKeyPressed();   // Wait until some key is pressed

    private:
        bool key[MAX_KEYS];         // keybuffer
};

#endif // KEYBOARD_H
