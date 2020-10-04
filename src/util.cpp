//
// Created by nek on 2020-10-02.
//
#include "util.hpp"
#include "../lib/sha256.h"
#include <filesystem>

const std::regex DateTime::date_time_regex{ // NOLINT NOLINTNEXTLINE
    R"((\d{1,2})([-. /])(\d{1,2})([-. /])(\d{4})[ ](\d{2})([-.:])(\d{2})([-.:])(\d{2}))"};

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

inline void cls() {                   // This function depends on platform
    system(std::string(CLS).c_str()); // NOLINT
}

std::string hash(const std::string& s) {
    SHA256 sha256;
    return sha256(s);
}

// Input password, hide it with *'s
std::string get_password() {
    std::string password;
    int         a = 0;
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

void ensure_file_exists(const std::string& f) {
    if (!std::filesystem::exists(f)) {
        Log() << "The file " << f << " does not exist! Creating a blank one...";
        std::ofstream file(f); // Create a new one
    }
}

// first is whether the check succeeded, second is error message
auto check_string(const std::string& s, CHECK mode)
    -> std::pair<bool, std::string> {
    auto msgFalse = [&s](const std::string& msg) {
        return std::make_pair(false, "The value " + s + " is invalid: \n" +
                                         msg + '\n');
    };
    if (s.empty())
        return msgFalse("No data?");
    size_t                       cnt = 0;
    std::pair<bool, std::string> p;
    switch (mode) {
    case 'p': // password
    case 'n': // No spaces
        if (s.size() < 3 || s.size() > 75)
            return msgFalse("Too short/long for a word");
        for (const auto& ch : s)
            if (!(isalnum(ch) || ch == '.' || ch == '-' || ch == '_' ||
                  ch == '\'' || ch == '#'))
                return msgFalse("Invalid characters");
        break;
    case 's': // spaces
        if (s.size() < 2)
            return msgFalse("Too short/long for a line");
        for (const auto& ch : s)
            if (!(isalnum(ch) || ispunct(ch) || ch == ' '))
                return msgFalse("Invalid characters");
        break;
    case 'i': // integer
        if (s.size() > 15)
            return msgFalse("too long for a number");
        for (const auto& ch : s)
            if (!isdigit(ch))
                return msgFalse("invalid characters in a number");
        break;
    case 'f': // float
        if (s.empty() || s.size() > 7)
            return msgFalse("too short/long for floating-point number");
        for (const auto& ch : s) {
            if (!isdigit(ch) && ch != '.')
                return msgFalse("invalid characters in a number");
            if (ch == '.')
                ++cnt;
            if (cnt > 1)
                return msgFalse("not a number");
        }
        break;
    case 'b': // bool
        if (s.size() != 1 || (s[0] != '0' && s[0] != '1'))
            return msgFalse("not a boolean");
        break;
    default: throw std::invalid_argument("Bad argument for checkString");
    }
    return std::make_pair(true, "");
}

DateTime DateTime::deserialize(const std::string& str, IS_PAST is_past) {
    auto        exc = [&str]() { throw std::invalid_argument(str); };
    std::smatch res;
    if (!std::regex_match(str, res, date_time_regex) ||
        res.str(2) != res.str(4) || res.str(7) != res.str(9))
        exc();

    int year  = std::stoi(res.str(1));
    int month = std::stoi(res.str(3));
    int day   = std::stoi(res.str(5));
    int h     = std::stoi(res.str(6));
    int m     = std::stoi(res.str(8));
    int s     = std::stoi(res.str(10));

    DateTime cur = get_current();
    DateTime dt(year, month, day, h, m, s);

    if (is_past != IS_PAST::ANY) { // the user requested sanity check
        bool cond = dt < cur;      // is in the past?
        if (!is_past)              // if we want future date
            cond =
                !cond; // invert the result (if in the past then becomes false)
        if (!cond)
            exc();
    }
    return dt;
}

DateTime::DateTime(int year, int month, int day, int hour, int minute,
                   int second) :
    y(year),
    mon(month), d(day), h(hour), min(minute), s(second) {
    auto exc = []() {
        throw std::invalid_argument("invalid values for DateTime()");
    };
    if (year < 0 || hour >= 24 || minute >= 60 || second >= 60 || hour < 0 ||
        minute < 0 || second < 0)
        exc();
    if (day > days_in(mon, year))
        exc();
}

DateTime DateTime::operator-=(const DateTime& rhs) {
    s -= rhs.s;
    if (s < 0) {
        s += 60;
        --min;
    }
    min -= rhs.min;
    if (min < 0) {
        min += 60;
        --h;
    }
    h -= rhs.h;
    if (h < 0) {
        h += 24;
        --d;
    }

    d -= rhs.d;
    if (d < 0) {
        --mon;
        d += days_in(mon, std::abs(y - rhs.y));
    } // if dt.d < 0
    mon -= rhs.mon;
    if (mon < 0) {
        mon += 12;
        --y;
    }

    y -= rhs.y;
    if (y < 0)
        y = 0;

    return *this;
}
DateTime DateTime::operator+=(const DateTime& rhs) {

    s += rhs.s;
    min += s / 60;
    s %= 60;
    min += rhs.min;
    h += min / 60;
    min %= 60;
    h += rhs.h;

    int leap_years = std::abs(rhs.y - y) / 4;
    d += rhs.d + leap_years;
    mon += d / 365;
    mon = d % 365;
    y += rhs.y;
    return *this;
}

bool operator<(const DateTime& lhs, const DateTime& rhs) {
    if (lhs.y < rhs.y)
        return true;
    if (lhs.y == rhs.y) {
        if (lhs.mon < rhs.mon)
            return true;
        if (lhs.mon == rhs.mon) {
            if (lhs.d < rhs.d)
                return true;
            if (lhs.d == rhs.d) {
                if (lhs.h < rhs.h)
                    return true;
                if (lhs.h == rhs.h) {
                    if (lhs.min < rhs.min)
                        return true;
                    if (lhs.min == rhs.min) {
                        if (lhs.s < rhs.s)
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

int DateTime::days_in(int month, int year) {
    switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: return 31;
    case 4:
    case 6:
    case 9:
    case 11: return 30;
    case 2: return (DateTime::is_leap(year) ? 29 : 28);
    default: throw std::invalid_argument("Default case when parsing month");
    }
}
