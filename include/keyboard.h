#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef _WIN32

    #include <conio.h>
    #include <windows.h>

#elif __linux

    #include <termios.h>
    #include <sys/ioctl.h>
    #include <cstring>
    #include <fcntl.h>
    #include <unistd.h>

#endif // _WIN32

#include <iostream>

#define MAX_KEYS   18           // Number of keys to read

class Keyboard
{
    public:
        Keyboard();
        virtual ~Keyboard();

        void init(bool on_off);     // Init keyboard on linux
        void readKeys();            // Read keys if they are pressed or not. Store values in keybuffer
        void clearKeys();           // Clear keybuffer
        bool getKey(int key_code);  // Read keys value from buffer
        uint8_t waitKeyPressed();   // Wait until some defined key is pressed
        void WaitKeyHit();          // Wait until some key is pressed

    private:
        bool key[MAX_KEYS];         // keybuffer

        #ifdef __linux

            termios t_old, t_new;
            int oldf;

        #endif
};

#endif // KEYBOARD_H
