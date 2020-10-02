//
// Created by nek on 2020-10-02.
//
#pragma once
#include <iostream>
#include <vector>
#include <../lib/fort.hpp>
#include <iomanip>
#include <chrono>
#include <regex>
#include <fstream>
#include <memory>
#include <typeinfo>

#ifndef __linux__
constexpr std::string_view CLS = "cls";
#include <conio.h>
static const int CARRIAGE_RETURN_CHAR = 13; //getch() returns different keycodes for windows and linux
static const int BACKSPACE_CHAR = 8;
using ull = unsigned long long; //The size of unsigned long on my linux distro is around ~10^25, however on my Windows OS the size of
//unsigned long long (!) is just ~10^20
using us = unsigned short;
#else
constexpr std::string_view CLS = "clear";
using ull = unsigned long;
static const int CARRIAGE_RETURN_CHAR = 10;
static const int BACKSPACE_CHAR = 127;
#include <termios.h>
int getch();
#endif


inline char* getCurTime() {
    auto t = std::chrono::system_clock::now();
    std::time_t ttime = std::chrono::system_clock::to_time_t(t);
    return std::ctime(&ttime);
}

class Log {

    inline static const std::string LOG_FNAME = "log.txt"; //NOLINT

    struct Stream {

        static std::ostream& stream() {
            return (get().use_cerr ? std::cerr : get().f );
        }

        static void flip_cerr() {
            get().use_cerr = !get().use_cerr;
            std::string s("-------------| Now outputting to ");
            s.append((get().use_cerr ? "cerr" : "file")).append(" |-------------\n");
            get().f << s; std::cerr << s;
        }
        static  bool get_cerr() {
            return get().use_cerr;
        }

        static void flush() {
            if (get().use_cerr) throw std::runtime_error("unable to flush the console");
            auto& ref = static_cast<std::fstream&>(stream());
            ref.close();
            ref.open(LOG_FNAME, std::fstream::out | std::fstream::trunc | std::fstream::in);
            ref << "-------------| Log flushed |-------------" << std::endl;
        }

        ~Stream() {
            f << "-------------| The program was closed |-------------" << std::endl;
        }

    private:
        static Stream& get() {
            static Stream s;
            return s;
        }
        Stream() {
            if (!f) throw std::runtime_error("can't open log file");
            f << "-------------| Started log session |-------------" << std::endl;
        }
        bool use_cerr = false;
        std::fstream f{LOG_FNAME, std::ios::in | std::ios::out | std::ios::app};
    };

public:
    Log() {
        Stream::stream() << getCurTime() << " | ";
    }

    ~Log() {
        Stream::stream() << '\n';
    }

    template<typename T>
    Log& operator<<(const T& o) {
        Stream::stream() << o;
        return *this;
    }

    static Log& flush(Log& l) {
        Stream::flush();
        return l;
    }

    static void flip_cerr() {
        Stream::flip_cerr();
    }

    static bool is_using_cerr() {
        return Stream::get_cerr();
    }
};
