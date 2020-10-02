//
// Created by nek on 2020-10-02.
//
#include "util.hpp"
#ifdef __linux__
int getch() // Getch for linux
{
    struct termios oldattr, newattr; // NOLINT
    int            ch = 0;
    tcgetattr(0, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(0, TCSANOW, &oldattr);
    return (ch);
}
#endif

inline void cls() { // This function depends on platform
    system(std::string(CLS).c_str());    // NOLINT
}
