#include "keyboard.h"

Keyboard::Keyboard()
{
    clearKeys();
}

Keyboard::~Keyboard()
{
    //dtor
}

void Keyboard::readKeys()
{
    // Virtual keykodes for keys to 0-F, esc and Space
    int codes[] = {0x58, 0x31, 0x32, 0x33, 0x51, 0x57, 0x45, 0x41,
                   0x53, 0x44, 0x5A, 0x43, 0x34, 0x52, 0x46, 0x56,
                   VK_ESCAPE, VK_SPACE};

    for (int i = 0; i < MAX_KEYS; i++)  // Loop if keys pressed, store states in keybuffer
        if (GetKeyState(codes[i]) & 0x80) key[i] = true; else key[i] = false;
}

bool Keyboard::getKey(int key_code) // Read keys value from buffer
{
    if (key_code < MAX_KEYS) return key[key_code];
        else return false;
}

void Keyboard::clearKeys()  // Clear keybuffer
{
   for (int i = 0; i < MAX_KEYS; i++) key[i] = false;
}

uint8_t Keyboard::waitKeyPressed()  // Wait until some key is pressed
{
    while(true)
    {
        readKeys();
        for (int i = 0; i < 17; i++)
        {
            if (getKey(i)) return i;  // Return value of pressed key
        }
    }
}
