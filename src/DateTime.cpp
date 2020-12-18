#include <ctgmath>
#include "DateTime.h"
#include <iostream>

const std::regex DateTime::date_time_regex{ // NOLINT NOLINTNEXTLINE
        R"((\d{4})([-.\s/])(\d{1,2})([-.\s/])(\d{1,2})(\s|T)(\d{1,2})([-:.\s])(\d{1,2})([-:.\s])(\d{1,2}))"};

DateTime DateTime::deserialize(const std::string& str, int is_past) try {
    auto exc = [&str]() { throw std::invalid_argument(str); };
    std::smatch res;
    if (!std::regex_match(str, res, date_time_regex)
        || res.str(2) != res.str(4) || res.str(8) != res.str(10)) //check delimiters
        exc();

    int year = std::stoi(res.str(1));
    int month = std::stoi(res.str(3));
    int day = std::stoi(res.str(5));
    int h = std::stoi(res.str(7));
    int m = std::stoi(res.str(9));
    int s = std::stoi(res.str(11));

    DateTime cur = get_current(); //compare with current LOCAL time
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
    throw std::invalid_argument("Invalid datetime: " + str);
}

DateTime::DateTime(int year, int month, int day, int hour, int minute,
                   int second) :
        y(year),
        mon(month), d(day), h(hour), min(minute), s(second) {
    auto exc = [&]() {
        std::stringstream ss;
        ss << "Invalid values for DateTime: " << year << "-" << month << "-" << day << " " << hour << ":" << minute
           << ":" << second;
        throw std::invalid_argument(ss.str());
    };
    if ((year < 1970 && year != 0) || day > days_in(mon, year) || hour >= 24 || minute >= 60 || second >= 60 ||
        hour < 0 ||
        minute < 0 || second < 0)
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

//FIXME: UNSAFE CRUTCH
DateTime operator+(const DateTime& lhs, const DateTime& rhs) {
    return DateTime::from_epoch(lhs.to_epoch() + rint(rhs.to_s_approx()));
}

DateTime DateTime::operator+=(const DateTime& rhs) {
    *this = *this + rhs;
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
    if (year == 0 && month == 0) return days_in(get_current()[1], get_current()[0]);
    if (year < 0) throw std::invalid_argument("Invalid value for year");
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            return (DateTime::is_leap(year) ? 29 : 28);
        default:
            throw std::invalid_argument("Invalid value for month");
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
            throw std::invalid_argument("Subscript out of range");
    }
}

DateTime DateTime::from_stream(std::istream& is) {
    std::string s;
    std::getline(is, s);
    return DateTime::deserialize(s);
}

bool operator==(const DateTime& lhs, const DateTime& rhs) {
    return lhs.y == rhs.y && lhs.mon == rhs.mon && lhs.d == rhs.d &&
            lhs.h == rhs.h && lhs.min == rhs.min && lhs.s == rhs.s;
}

DateTime operator-(const DateTime& lhs, const DateTime& rhs) {
    DateTime sum = lhs;
    return sum -= rhs;
}

bool operator>(const DateTime& lhs, const DateTime& rhs) {
    return rhs < lhs;
}

bool operator!=(const DateTime& lhs, const DateTime& rhs) {
    return !(lhs == rhs);
}

bool operator<=(const DateTime& lhs, const DateTime& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

bool operator>=(const DateTime& lhs, const DateTime& rhs) {
    return rhs <= lhs;
}

std::ostream& operator<<(std::ostream& os, const DateTime& t) {
    return os << t.serialize();
}

bool DateTime::operator!() const {
    return y == 0 && mon == 0 && d == 0 && h == 0 && min == 0 && s == 0;
}

DateTime DateTime::get_current() {
    time_t t = time(nullptr); // get system time
    tm* now = localtime(&t); // format it according to the region
    return DateTime(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
                    now->tm_hour, now->tm_min, now->tm_sec);
}

std::string DateTime::to_string() const {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << y << "-" << std::setw(2) << mon
            << "-" << std::setw(2) << d << " " << std::setw(2) << h
            << ":" << std::setw(2) << min << ":" << std::setw(2) << s;
    return ss.str();
}

std::string DateTime::to_printable(bool short_date) const {
    if (incomplete())
        return "----.--.-- --:--:--";
    std::stringstream ss;
    ss << weekday_string(y, mon, d, short_date) << ", "
            << (short_date ? MONTHS_SHORT.at(mon - 1) : MONTHS.at(mon - 1)) << " " << d << number_postfix(d) << " "
            << y << ", " << std::setfill('0')
            << std::setw(2) << h << ":" << std::setw(2) << min << ":" << std::setw(2) << s;
    return ss.str();
}

std::string DateTime::to_duration_printable() const {
    std::stringstream ss;
    ss << y << " years, " << mon << " months, " << d << " days, "
            << h << " hrs, " << min << " min, " << s << " s. ";
    return ss.str();
}

bool DateTime::is_leap(int year) {
    if (year < 0)
        throw std::invalid_argument("negative year");
    return year != 0 &&
            (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0));
}

unsigned DateTime::get_weekday(int y, int m, int d) {
    if (y == 0 || m == 0 || d == 0) //unknown
        return 0;
    m = (m + 9) % 12;
    y -= m / 10;
    return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 1);
}

std::string DateTime::weekday_string(int y, int m, int d, bool short_str) {
    if (y == 0 || m == 0 || d == 0)
        return "-";
    unsigned val = get_weekday(y, m, d) % 7;
    return std::string((short_str ? WDAYS_SHORT.at(val) : WDAYS.at(val)));
}

std::string DateTime::number_postfix(int num) {
    switch (std::abs(num)) {
        case 1:
            return "st";
        case 2:
            return "nd";
        case 3:
            return "rd";
        default:
            return "th";
    }
}

bool DateTime::incomplete() const {
    return y == 0 || mon == 0 || d == 0;
}

int DateTime::g(int y, int m, int d) {
    m = (m + 9) % 12;
    y = y - m / 10;
    return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 1);
}


//uses local time
DateTime DateTime::from_epoch(time_t s) {
    tm* t = localtime(&s);
    return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                    t->tm_hour, t->tm_min, t->tm_sec);
}

time_t DateTime::to_epoch() const {
    auto tm = to_tm_struct();
    return mktime(&tm);
}

tm DateTime::to_tm_struct() const {
    std::tm t{};  // Initalize to all 0's
    t.tm_year = y - 1900;  // This is year-1900, so 112 = 2012
    t.tm_mon = mon - 1; //months are from 0
    t.tm_mday = d;
    t.tm_hour = h;
    t.tm_min = min;
    t.tm_sec = s;
    return t;
}
