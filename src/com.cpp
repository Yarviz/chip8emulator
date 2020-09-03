#include "com.h"

std::string Com::curLeft(int n)
{
    std::stringstream os;
    os << "\033[" << n << "A";
    return os.str();
}

std::string Com::curRight(int n)
{
    std::stringstream os;
    os << "\033[" << n << "C";
    return os.str();
}

std::string Com::curUp(int n)
{
    std::stringstream os;
    os << "\033[" << n << "A";
    return os.str();
}

std::string Com::curDown(int n)
{
    std::stringstream os;
    os << "\033[" << n << "B";
    return os.str();
}

std::string Com::curSave()
{
    return "\033[s";
}

std::string Com::curLoad()
{
    return "\033[u";
}
