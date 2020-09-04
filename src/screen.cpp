#include "screen.h"

Screen::Screen()
{
    width = 64;
    height = 32;

    buffer.resize(width * height);
    clr();
}

Screen::~Screen()
{
    //dtor
}

void Screen::start()    // Start renderer
{
    clr();
    draw(false);
}

void Screen::clr()  // Clear screen and erase screen buffer
{
    //cout << curUp(height);
    std::fill(buffer.begin(), buffer.end(), 0);
    //draw();
}

void Screen::drawKeyboard() // Draw keys on screen
{
    int k = 0;
    char keys[] = {'1', '2', '3', '4',
                   'q', 'w', 'e', 'r',
                   'a', 's', 'd', 'f',
                   'z', 'x', 'c', 'v'};

    cout << curSave() << curUp(height / 2 + 6);
    for (int y = 0; y < 4; y++)
    {
        cout << curRight(width + 4) << "+-+-+-+-+" << endl;
        cout << curRight(width + 4);
        for (int x = 0; x < 4; x++)
        {
            cout << reset << "|" << yellow << keys[k];
            k++;
        }
        cout << reset << "|" << endl;
    }
    cout << curRight(width + 4) << "+-+-+-+-+" << endl << endl;
    cout << curRight(width + 4) << yellow << "space " << reset << ": pause" << endl;
    cout << curRight(width + 4) << yellow << "esc   " << reset << ": quit" << endl;
    cout << curLoad() << reset;
}

void Screen::draw(bool cur_up) // Draw screen buffer
{
    if (cur_up) cout << curUp(height);
    cout << green;
    for (int y = 0; y < height; y++)
    {
        cout << " ";
        for (int x = 0; x < width; x++)
        {
            if (buffer[y * width + x]) cout << (uint8_t)219;
                else cout << (uint8_t)177;
        }
        cout << endl;
    }
    cout << reset;
}

uint8_t Screen::drawSprite(int x, int y, std::vector<uint8_t> sprite)   // Draw sprite on screen
{
    cout << curSave() << curUp(height) << green;    // Save cursor position and move cursor top-left corner
    uint8_t collis = 0; // Return value for sprite collision
    x %= width;         // If sprite is outside buffer, wrap around
    y %= height;
    int x2 = x, y2 = y;
    std::stringstream ss;
    ss << curRight(x + 1);
    if (y > 0) ss << curDown(y);
    std::string pos = ss.str();
    cout << pos;            // Move cursor to (x, y) position
    ss.str("");
    ss << curRight(x + 1);
    pos = ss.str();         // Save string for cursor x-position

    for (int i = 0; i < sprite.size(); i++) // Loop for sprite height
    {
        uint8_t b = 0x80;    // Sample bits 0x80 = 1000 0000
        while (b)            // Loop while sample bits > 0000 0000
        {
            if (sprite[i] & b) // AND sprite bits with sample bits if match
            {
                if (!(buffer[y2 * width + x2] ^= 1))    // XOR buffer and look collision
                {                                       // if pixel is 1 on buffer(x2, y2).
                    collis = 1;                         // Pixel turn off is exists and collision = 1
                    cout << (uint8_t)177;
                }
                else cout << (uint8_t)219;
            }
            else cout << curRight(1);
            if (++x2 == width) b = 0x01;        // If sprite over width, set sample bits to 0000 0001
            b >>= 1;                            // Shift sample bits to right
        }
        x2 = x;
        if (++y2 == height) break;              // If sprite over height, stop drawing
        cout << endl << pos;
    }
    cout << curLoad() << reset;                 // Load old cursor position
    return collis;
}
