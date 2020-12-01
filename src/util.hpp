#pragma once
#include "DateTime.h"
#include <fstream>
#include <iostream>
#include <iomanip>

#ifndef __linux__
using id_type                   = unsigned long long;
// unsigned long long (!) is just ~10^20
#else
using id_type = unsigned long;
#endif
#define MAX_ID 1000000000000000000u // 19 digits
#define MAX_ID_LENGTH 19

enum class CHECK {
    LINE = 's',
    WORD = 'n',
    PASS = 'p',
    BOOL = 'b',
    FLOAT = 'f',
    ID = 'i',
    YEAR = 'y'
};

std::vector<std::string> split(const std::string& s, const std::string& delims,
                               bool empty_tokens_allowed = true);

void ensure_file_exists(const std::string& f);

std::string lowercase(const std::string&);

bool stob(const std::string& b);

std::string hash(const std::string& s);

void wait(int ms);

id_type gen_id();

id_type stoid(const std::string& s);

auto check_string(const std::string& s, CHECK mode)
-> std::pair<bool, std::string>;

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
        static Stream& get() {
            static Stream s;
            return s;
        }

        Stream() {
            if (!f)
                throw std::runtime_error("can't open log file");
            f << "-------------| Started log session |-------------"
              << std::endl;
        }

        bool use_cerr = false;
        std::fstream f{LOG_FNAME, std::ios::in | std::ios::out | std::ios::app};
    };

public:
    Log() { Stream::stream() << DateTime::get_current() << " | "; }

    ~Log() { Stream::stream() << std::endl; }

    template<typename T>
    Log& operator<<(const T& o) {
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
