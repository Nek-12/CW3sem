//
// Created by nek on 2020-10-12.
//

#include "UI.hpp"

namespace UI {

#ifdef __linux__

    int getch() // Getch for linux
    {
        struct termios oldattr, newattr; // NOLINT
        int ch = 0;
        tcgetattr(0, &oldattr);
        newattr = oldattr;
        newattr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(0, TCSANOW, &newattr);
        ch = getchar();
        tcsetattr(0, TCSANOW, &oldattr);
        return (ch);
    }

#endif

    inline void cls() {                   // This function depends on platform
        system(std::string(CLS).c_str()); // NOLINT
    }


// Input password, hide it with *'s
    std::string get_password() {
        std::string password;
        int a = 0;
        while ((a = getch()) !=
               CARRIAGE_RETURN_CHAR) // Differs on linux and windows
        {                            // While ENTER is not pressed
            if (a == BACKSPACE_CHAR) {
                if (password.empty())
                    continue;
                password.pop_back();              // remove char
                std::cout << '\b' << ' ' << '\b'; // replace a star with a space
            } else {
                password += static_cast<char>(a); // Add this char
                std::cout << '*';                 // But output the star
            }
        }
        std::cout << std::endl;
        return password;
    }

    void set_table_style(fort::char_table& t, unsigned green_col,
                         unsigned red_col) {
        t.set_cell_text_align(fort::text_align::center);
        t.set_border_style(FT_BASIC2_STYLE);
        t.column(green_col).set_cell_content_fg_color(fort::color::green);
        t.column(red_col).set_cell_content_fg_color(fort::color::red);
    }

    template<int C>
    void fancy_print(const svarr<C>& menu, int selected = 0) {
        using std::cout;
    }

    template<int C>
    int select_entry(const svarr<C>& list) {
        return 0;
    }


} //namespace UI
