//
// Created by nek on 2020-10-02.
//
#include "util.hpp"
#include "../lib/sha256.h"
#include <filesystem>
#include <random>
#include <cstddef>


const std::regex DateTime::date_time_regex{ // NOLINT NOLINTNEXTLINE
        R"((\d{4})([-. /])(\d{1,2})([-. /])(\d{1,2})(\s|T)(\d{2})([-:])(\d{2})([-:])(\d{2}))"};

std::string hash(const std::string& s) {
    SHA256 sha256;
    return sha256(s);
}

std::vector<std::string> split(
        const std::string& s,
        const std::string& delims,
        bool empty_tokens_allowed) {
    std::vector<std::string> result;
    size_t current = 0;
    size_t next = -1;
    do {
        if (!empty_tokens_allowed) {
            next = s.find_first_not_of( delims, next + 1 );
            if (next == std::string::npos) break;
            next -= 1;
        }
        current = next + 1;
        next = s.find_first_of( delims, current );
        result.push_back( s.substr( current, next - current ) );
    } while (next != std::string::npos);
    return result;
}

id_type gen_id() { // Static to not diminish randomness
    static std::default_random_engine e(
        std::random_device{}()); // Initialize a random engine from system
    // random device
    static std::uniform_int_distribution<id_type> rng(
        0, MAX_ID); // Generate numbers for IDs from 0 to a large system value
    return rng(e);  // use random engine
}

id_type stoid(const std::string& s) {
#ifndef __linux__
    return std::stoull(s);
#else // This function depends on platform. See header.h for details
    return std::stoul(s);
#endif
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

/////////////////DATETIME/////////////////

DateTime DateTime::deserialize(const std::string& str, int is_past) try {
    auto exc = [&str]() { throw std::invalid_argument(str); };
    std::smatch res;
    Log() << "deserializing DT:" << str;
    if (!std::regex_match(str, res, date_time_regex)
        || res.str(2) != res.str(4) || res.str(8) != res.str(10))
        exc();

    int year = std::stoi(res.str(1));
    int month = std::stoi(res.str(3));
    int day = std::stoi(res.str(5));
    int h = std::stoi(res.str(7));
    int m = std::stoi(res.str(9));
    int s = std::stoi(res.str(11));

    DateTime cur = get_current();
    DateTime dt(year, month, day, h, m, s);

    if (is_past != ANY) { // the user requested sanity check
        bool cond = dt < cur;      // is in the past?
        if (!is_past)              // if we want future date
            cond = !cond; // invert the result
        if (!cond)
            exc();
    }
    return dt;
} catch (const std::exception& e) {
    throw std::invalid_argument(str);
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
    if (year <= 0 || month <= 0) return days_in(get_current()[1], get_current()[0]);
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

double DateTime::to_y_approx() const {
    double yd = APPROX_YEAR_DURATION_DAYS;
    return static_cast<double>(s) / 3600 / 24 / yd +
           static_cast<double>(min) / 60 / 24 / yd +
           static_cast<double>(h) / 24 / yd + static_cast<double>(d) / yd +
           static_cast<double>(mon) / 12 + y;
}

double DateTime::to_mon_approx() const {
    double md = APPROX_MONTH_DURATION_DAYS;
    return static_cast<double>(s) / 3600 / 24 / md +
           static_cast<double>(min) / 60 / 24 / md +
           static_cast<double>(h) / 24 / md + static_cast<double>(d) / md +
           mon + y * APPROX_YEAR_DURATION_DAYS / md;
}

double DateTime::to_d_approx() const {
    return static_cast<double>(s) / 3600 / 24 +
           static_cast<double>(min) / 60 / 24 +
           static_cast<double>(h) / 24 + d +
           mon * APPROX_MONTH_DURATION_DAYS + y * APPROX_YEAR_DURATION_DAYS;
}

double DateTime::to_h_approx() const {
    return static_cast<double>(s) / 3600 + static_cast<double>(min) / 60 +
           h + d * 24 + mon * APPROX_MONTH_DURATION_DAYS * 24 +
           y * APPROX_YEAR_DURATION_DAYS * 24;
}

double DateTime::to_min_approx() const {
    return static_cast<double>(s) / 60 + min + h * 60 + d * 24 * 60 +
           mon * APPROX_MONTH_DURATION_DAYS * 24 * 60 +
           y * APPROX_YEAR_DURATION_DAYS * 24 * 60;
}

double DateTime::to_s_approx() const {
    return s + min * 60 + h * 3600 + d * 24 * 3600 +
           mon * APPROX_MONTH_DURATION_DAYS * 24 * 3600 +
           y * APPROX_YEAR_DURATION_DAYS * 24 * 3600;
}

int DateTime::operator[](int n) const {
    switch (n) {
        case 0:
            return y;
        case 1:
            return mon;
        case 2:
            return d;
        case 3:
            return h;
        case 4:
            return min;
        case 5:
            return s;
        default:
            throw std::invalid_argument("subscript out of range");
    }
}
