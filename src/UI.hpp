
#pragma once

#include "util.hpp"
#include <fort.hpp>
#include "CONST.hpp"
#include "Data.hpp"

constexpr std::string_view CLS = "clear";

enum class KEY : int {
    ENTER = 10,
    BACKSPACE = 127,
    ARROW_1 = 27,
    ARROW_2 = 91,
    ARROW_LEFT = 68,
    ARROW_UP = 65,
    ARROW_RIGHT = 67,
    ARROW_DOWN = 66,
    SPACE = 32,
};

#include <termios.h>


namespace color {

#define MAKE_COLOR_MANIPULATOR(name, code)                               \
  template < typename CharT, typename Traits = std::char_traits<CharT> > \
  inline std::basic_ostream< CharT, Traits >&                            \
  name(std::basic_ostream< CharT, Traits >& os)                          \
  { return os << code; }

// These color definitions are based on the color scheme used by Git (the
// distributed version control system) as declared in the file
// `git/color.h`. You can add more manipulators as desired.

    MAKE_COLOR_MANIPULATOR(normal, "")

    MAKE_COLOR_MANIPULATOR(reset, "\033[m")

    MAKE_COLOR_MANIPULATOR(bold, "\033[1m")

    MAKE_COLOR_MANIPULATOR(red, "\033[31m")

    MAKE_COLOR_MANIPULATOR(green, "\033[32m")

    MAKE_COLOR_MANIPULATOR(yellow, "\033[33m")

    MAKE_COLOR_MANIPULATOR(blue, "\033[34m")

    MAKE_COLOR_MANIPULATOR(magenta, "\033[35m")

    MAKE_COLOR_MANIPULATOR(cyan, "\033[36m")

    MAKE_COLOR_MANIPULATOR(bold_red, "\033[1;31m")

    MAKE_COLOR_MANIPULATOR(bold_green, "\033[1;32m")

    MAKE_COLOR_MANIPULATOR(bold_yellow, "\033[1;33m")

    MAKE_COLOR_MANIPULATOR(bold_blue, "\033[1;34m")

    MAKE_COLOR_MANIPULATOR(bold_magenta, "\033[1;35m")

    MAKE_COLOR_MANIPULATOR(bold_cyan, "\033[1;36m")

    MAKE_COLOR_MANIPULATOR(bg_red, "\033[41m")

    MAKE_COLOR_MANIPULATOR(bg_green, "\033[42m")

    MAKE_COLOR_MANIPULATOR(bg_yellow, "\033[43m")

    MAKE_COLOR_MANIPULATOR(bg_blue, "\033[44m")

    MAKE_COLOR_MANIPULATOR(bg_magenta, "\033[45m")

    MAKE_COLOR_MANIPULATOR(bg_cyan, "\033[46m")

} // namespace color

class UI {

    static void set_table_style(fort::char_table& t, unsigned green_col, unsigned red_col); //NOLINT

    static std::string colorize_entry(const std::string_view& s, size_t len) {
        std::stringstream ss;
        ss << "[ " << color::bold << color::green << std::setw(len) << std::left
           << s << color::reset << " ]";
        return ss.str();
    }

public:
    template<typename T, size_t C>
    static void print_data(const Journal<T>& j, CONST::svarr<C> headers) {

    }

    template<size_t C, typename T>
    static size_t select_entry(const CONST::svarr<C>& list,
                               const T& pre_data = "",
                               const T& post_data = "") {
        size_t selected = 0;
        int c = 0;
        do {
            cls();
            std::cout << pre_data << "\n\n";
            print(list, selected);
            std::cout << post_data << "\n";
            c = getch();
            Log() << "Getch() = " << c;
            if (c == static_cast<int>(KEY::ARROW_1) && getch() == static_cast<int>(KEY::ARROW_2)) {
                switch (getch()) {
                    case static_cast<int>(KEY::ARROW_DOWN):
                        if (selected < list.size() - 1)
                            ++selected;
                        break;
                    case static_cast<int>(KEY::ARROW_UP):
                        if (selected > 0)
                            --selected;
                        break;
                    default:
                        break;
                }
            }
        } while (c != static_cast<int>(KEY::ENTER) && c != static_cast<int>(KEY::SPACE));
        return selected;
    }

    template<size_t C>
    static void print(const CONST::svarr<C>& menu, size_t selected = 0) {
        size_t max_len = 0;
        for (const auto& el: menu)
            if (el.size() > max_len)
                max_len = el.size();
        for (size_t i = 0; i < menu.size(); ++i) {
            if (i == selected)
                std::cout << colorize_entry(menu[i], max_len) << "\n";
            else
                std::cout << "[ " << std::setw(max_len) << std::left << menu[i] << " ]\n";
        }
        std::cout << std::endl;
    }

    static int getch();

    static inline void pause() {
        std::cerr << CONST::ANY_KEY << std::endl;
        getch();
    }

    static inline void cls() { system(std::string(CLS).c_str()); } //NOLINT

    static std::string get_password();
};



