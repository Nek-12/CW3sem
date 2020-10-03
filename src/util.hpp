//
// Created by nek on 2020-10-02.
//
#pragma once
#include <ctime>
#include <fort.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#ifndef __linux__
constexpr std::string_view CLS = "cls";
#include <conio.h>
static const int CARRIAGE_RETURN_CHAR =
    13; // getch() returns different keycodes for windows and linux
static const int BACKSPACE_CHAR = 8;
using ull                       = unsigned long long;
// unsigned long long (!) is just ~10^20
#else
constexpr std::string_view CLS        = "clear";
using ull                             = unsigned long;
static const int CARRIAGE_RETURN_CHAR = 10;
static const int BACKSPACE_CHAR       = 127;
#include <termios.h>
int getch();
#endif

void ensure_file_exists(const std::string& f);
void set_table_style(fort::char_table& t, unsigned green_col, unsigned red_col);
inline void        pause(); // Wait for a keypress
std::string        get_password();
inline void        cls();
inline std::string lowercase(const std::string&);
inline std::string hash(const std::string& s);

enum CHECK {
    LINE  = 's',
    WORD  = 'n',
    PASS  = 'p',
    BOOL  = 'b',
    FLOAT = 'f',
    ID    = 'i',
    YEAR  = 'y'
};

// ISO8601 datetime class
struct DateTime {
    // TODO: Operator -, +=, -=,

    enum IS_PAST { YES = 1, NO = 0, ANY = -1 };

    int y   = 0;
    int mon = 0;
    int d   = 0;
    int h   = 0;
    int min = 0;
    int s   = 0;

    friend DateTime operator-(const DateTime& lhs, const DateTime& rhs) {
        DateTime sum = lhs;
        return sum -= rhs;
    }

    friend DateTime operator+(const DateTime& lhs, const DateTime& rhs) {
        DateTime sum = lhs;
        return sum += rhs;
    }

    DateTime operator-=(const DateTime& rhs);

    DateTime operator+=(const DateTime& rhs);

    friend bool operator<(const DateTime& lhs, const DateTime& rhs);

    friend bool operator>(const DateTime& lhs, const DateTime& rhs) {
        return rhs < lhs;
    }

    friend bool operator==(const DateTime& lhs, const DateTime& rhs) {
        return lhs.y == rhs.y && lhs.mon == rhs.mon && lhs.d == rhs.d &&
               lhs.h == rhs.h && lhs.min == rhs.min && lhs.s == rhs.s;
    }

    friend bool operator!=(const DateTime& lhs, const DateTime& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<=(const DateTime& lhs, const DateTime& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    friend bool operator>=(const DateTime& lhs, const DateTime& rhs) {
        return !(lhs <= rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const DateTime& t) {
        return os << t.to_string();
    }

    DateTime() = default;

    DateTime(int year, int month, int day, int hour, int minute, int second);

    static DateTime get_current() {
        time_t t   = time(nullptr); // get system time
        tm*    now = localtime(&t); // format it according to the region
        return DateTime(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
                        now->tm_hour, now->tm_min, now->tm_sec);
    }

    static DateTime deserialize(const std::string& str, IS_PAST is_past);

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << this->y << "." << this->mon << "." << this->d << " " << this->h
           << ":" << this->min << ":" << this->s;
        return ss.str();
    }

    static bool is_leap(int year) {
        if (year < 0) throw std::invalid_argument("negative year");
        return year != 0 && (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0));
    }

private:
    static const std::regex date_time_regex;
};

#ifndef NDEBUG
class Log {

    struct Stream {

        static std::ostream& stream() {
            return (get().use_cerr ? std::cerr : get().f);
        }

        static void flip_cerr() {
            get().use_cerr = !get().use_cerr;
            std::string s("-------------| Now outputting to ");
            s.append((get().use_cerr ? "cerr" : "file"))
                .append(" |-------------\n");
            get().f << s;
            std::cerr << s;
        }
        static bool get_cerr() { return get().use_cerr; }

        static void flush() {
            if (get().use_cerr)
                throw std::runtime_error("tried to flush console");
            auto& ref = static_cast<std::fstream&>(stream());
            ref.close();
            ref.open(LOG_FNAME, std::fstream::out | std::fstream::trunc |
                                    std::fstream::in);
            ref << "-------------| Log flushed |-------------" << std::endl;
        }

        ~Stream() { f << "-------------| Closed |-------------" << std::endl; }

    private:
        inline static const std::string LOG_FNAME = "log.txt"; // NOLINT
        static Stream&                  get() {
            static Stream s;
            return s;
        }
        Stream() {
            if (!f)
                throw std::runtime_error("can't open log file");
            f << "-------------| Started log session |-------------"
              << std::endl;
        }
        bool         use_cerr = false;
        std::fstream f{LOG_FNAME, std::ios::in | std::ios::out | std::ios::app};
    };

public:
    Log() { Stream::stream() << DateTime::get_current() << " | "; }

    ~Log() { Stream::stream() << '\n'; }

    template <typename T> Log& operator<<(const T& o) {
        Stream::stream() << o;
        return *this;
    }

    static Log& flush(Log& l) {
        Stream::flush();
        return l;
    }

    static void flip_cerr() { Stream::flip_cerr(); }

    static bool is_using_cerr() { return Stream::get_cerr(); }
};

#else
// do nothing if NDEBUG is on
struct Log {
    template <typename T> Log& operator<<(const T& o) { return *this; }
    static Log&                flush(Log& l) { return l; }
    static void                flip_cerr() {}
    static bool                is_using_cerr() {}
};

#endif
