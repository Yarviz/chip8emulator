#include "screen.h"

Screen::Screen()
{
    width = 64;
    height = 32;

    buffer.resize(width * height);
}

Screen::~Screen()
{
    //dtor
}

void Screen::start()
{
    cout << curSave();
}

void Screen::clr()
{
    cout << curUp(height);
    std::fill(buffer.begin(), buffer.end(), 0);
    draw();
}

void Screen::drawKeyboard()
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

void Screen::draw()
{
    cout << green;
    for (int y = 0; y < height; y++)
    {
        cout << " " << std::string(width, (uint8_t)177) << endl;
    }
    cout << reset;
}

uint8_t Screen::drawSprite(int x, int y, std::vector<uint8_t> sprite)
{
    cout << curSave() << curUp(height) << green;
    uint8_t collis = 0;
    x %= width;
    y %= height;
    int x2 = x, y2 = y;
    std::stringstream ss;
    ss << curRight(x + 1);
    if (y > 0) ss << curDown(y);
    std::string pos = ss.str();
    cout << pos;
    ss.str("");
    ss << curRight(x + 1);
    pos = ss.str();

    for (int i = 0; i < sprite.size(); i++)
    {
        uint8_t b = 128;
        while (b)
        {
            if (sprite[i] & b)
            {
                if (!(buffer[y2 * width + x2] ^= 1))
                {
                    collis = 1;
                    cout << (uint8_t)177;
                }
                else cout << (uint8_t)219;
            }
            else cout << curRight(1);
            if (++x2 == width) b = 1;
            b >>= 1;
        }
        x2 = x;
        if (++y2 == height) break;
        cout << endl << pos;
    }
    cout << curLoad() << reset;
    return collis;
}
