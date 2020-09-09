#include "keyboard.h"

Keyboard::Keyboard()
{
    clearKeys();
}

Keyboard::~Keyboard()
{
    //dtor
}

void Keyboard::init(bool on_off)
{
    #ifdef __linux

        if (on_off)
            {
                tcgetattr(fileno(stdin), &t_old);
                memcpy(&t_new, &t_old, sizeof(termios));
                t_new.c_lflag &= ~(ECHO | ICANON);
                t_new.c_cc[VTIME] = 0;
                t_new.c_cc[VMIN] = 1;
                tcsetattr(fileno(stdin), TCSANOW, &t_new);

                oldf = fcntl(fileno(stdin), F_GETFL, 0);
                fcntl(fileno(stdin), F_SETFL, oldf | O_NONBLOCK);
            }
            else
            {
                tcsetattr(fileno(stdin), TCSANOW, &t_old);
                fcntl(fileno(stdin), F_SETFL, oldf);
            }

    #endif // _linux
}

void Keyboard::readKeys()
{
    #ifdef __WIN32

        // Virtual keykodes for keys to 0-F, esc and Space in windows
        int codes[] = {0x58, 0x31, 0x32, 0x33, 0x51, 0x57, 0x45, 0x41,
                       0x53, 0x44, 0x5A, 0x43, 0x34, 0x52, 0x46, 0x56,
                       VK_ESCAPE, VK_SPACE};

        for (int i = 0; i < MAX_KEYS; i++)  // Loop if keys pressed, store states in keybuffer
            if (GetKeyState(codes[i]) & 0x80) key[i] = true; else key[i] = false;

    #elif __linux

        char codes[] = {0x78, 0x31, 0x32, 0x33, 0x71, 0x77, 0x65, 0x61, 0x73,
                        0x64, 0x7A, 0x63, 0x34, 0x72, 0x66, 0x76, 0x1B, 0x20};

        char bytes_waiting;

        while(read(0, &bytes_waiting, 1) > 0)
        {
            for (int i = 0; i < MAX_KEYS; i++)  // Loop if keys pressed, store states in keybuffer
                if (bytes_waiting == codes[i]) {key[i] = true;break;}
        }

    #endif // __WIN32
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
    clearKeys();
    while(true)
    {
        readKeys();
        for (int i = 0; i < 17; i++)
        {
            if (getKey(i)) return i;  // Return value of pressed key
        }
    }
}

void Keyboard::WaitKeyHit()
{
    #ifdef _WIN32

        while(_kbhit()) getch();
        getch();

    #elif __linux

        fflush(stdout);
        while(fgetc(stdin) == -1);

    #endif // _WIN32
}
