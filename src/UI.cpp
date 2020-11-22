
#include "UI.hpp"

#ifdef __linux__

int UI::getch() {
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

// Input password, hide it with *'s
std::string UI::get_password() {
    std::string password;
    int a = 0;
    while ((a = getch()) !=
           static_cast<int>(KEY::ENTER)) // Differs on linux and windows
    {                            // While ENTER is not pressed
        if (a == static_cast<int>(KEY::BACKSPACE)) {
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

void UI::set_table_style(fort::char_table& t, unsigned green_col,
                         unsigned red_col) {
    t.set_cell_text_align(fort::text_align::center);
    t.set_border_style(FT_BASIC2_STYLE);
    t.column(green_col).set_cell_content_fg_color(fort::color::green);
    t.column(red_col).set_cell_content_fg_color(fort::color::red);
}
