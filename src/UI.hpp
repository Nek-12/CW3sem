//
// Created by nek on 2020-10-12.
//

#pragma once

#include "util.hpp"
#include <fort.hpp>
#include "CONST.hpp"

namespace UI {
    using CONST::svarr;

#ifndef __linux__
    constexpr std::string_view CLS = "cls";
#include <conio.h>
    static const int CARRIAGE_RETURN_CHAR =
        13; // getch() returns different keycodes for windows and linux
    static const int BACKSPACE_CHAR = 8;
#else

#include <termios.h>

    constexpr std::string_view CLS = "clear";
    static const int CARRIAGE_RETURN_CHAR = 10;
    static const int BACKSPACE_CHAR = 127;
#endif

    int getch();

    inline void pause(); // Wait for a keypress

    inline void cls();

    std::string get_password();

    void set_table_style(fort::char_table& t, unsigned green_col, unsigned red_col);

    template<size_t C>
    int select_entry(const svarr<C>& list);

    template<size_t C>
    void fancy_print(const svarr<C>& menu);

} //namespace UI
