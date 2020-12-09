//Copyright Nek.12: vaizin.nikita@gmail.com
#pragma once

#include <ctime>
#include <iomanip>
#include <regex>

/* Usage:
DateTime dt(year,month,day,hour,minute,second) OR
DateTime() - creates a 0000:00:00 00:00:00 object OR
static DateTime::get_current() - obtains current LOCAL time OR
static DateTime::deserialize(string, enum value ANY|PAST|FUTURE ) - creates an object from a string using the following pattern:
YYYY_MM_DD HH:MM:SS
         /\
         |
Space or letter T (as in ISO8601)

Possible date delimiters (shown as " _ ") include: " - ", " . ", " / ", " " (space)
Possible time delimiters (shown as " : ") include: " - ", " . ", " : "
PAST - perform a check if a date should be in the past (according to local time)
FUTURE - in the future
ANY - skip the check (other checks will still be performed)

 ! Any of the following methods (including constructors) may throw if the values are wrong

You can represent both duration and time point with a single object
Use operators + , - , += , -= for the appropriate operations
Lexicographical comparison also compares dates
Use serialize() or to_string() to obtain a text view of the date (ISO8601)

Operator[] allows you to get values as follows:
 0: years
 1: months
 ...
 5: seconds
 Any other values will cause an exception

*/


// ISO8601 datetime class
class DateTime {
public:
    inline static const double APPROX_MONTH_DURATION_DAYS = 30.4583333;
    inline static const double APPROX_YEAR_DURATION_DAYS = 365.242;
    static constexpr std::array<std::string_view, 7> WDAYS{
            "Wednesday",
            "Thursday",
            "Friday",
            "Saturday",
            "Sunday",
            "Monday",
            "Tuesday"
    };

    static constexpr std::array<std::string_view, 12> MONTHS{
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December"
    };

    static constexpr std::array<std::string_view, 7> WDAYS_SHORT{
            "Wed",
            "Thu",
            "Fri",
            "Sat",
            "Sun",
            "Mon",
            "Tue"
    };

    static constexpr std::array<std::string_view, 12> MONTHS_SHORT{
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec"
    };

    enum {
        PAST = 1, FUTURE = 0, ANY = -1 //
    };


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
        return rhs <= lhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const DateTime& t) {
        return os << t.serialize();
    }

    //returns a value at a position from 0 to 5, may throw
    int operator[](int n) const;

    //Returns true if the date is not blank (0000-00-00 00:00:00)
    bool operator!() const {
        return y == 0 && mon == 0 && d == 0 && h == 0 && min == 0 && s == 0;
    }

    //create a blank value 0000-00-00 00:00:00
    DateTime() = default;

    DateTime(int year, int month, int day, int hour, int minute, int second);

    static DateTime get_current() {
        time_t t = time(nullptr); // get system time
        tm* now = localtime(&t); // format it according to the region
        return DateTime(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
                        now->tm_hour, now->tm_min, now->tm_sec);
    }

    //PAST - perform a check if a date should be in the past (according to local time)
    //FUTURE - in the future
    //ANY - skip the check
    static DateTime deserialize(const std::string& str, int is_past = ANY);

    [[nodiscard]] std::string to_string() const {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << y << "-" << std::setw(2) << mon
           << "-" << std::setw(2) << d << " " << std::setw(2) << h
           << ":" << std::setw(2) << min << ":" << std::setw(2) << s;
        return ss.str();
    }

    [[nodiscard]] std::string to_printable(bool short_date) const {
        if (incomplete())
            return "----.--.-- --:--:--";
        std::stringstream ss;
        ss << weekday_string(y, mon, d, short_date) << ", "
           << (short_date ? MONTHS_SHORT.at(mon - 1) : MONTHS.at(mon - 1)) << " " << d << number_postfix(d) << " "
           << y << ", " << std::setfill('0')
           << std::setw(2) << h << ":" << std::setw(2) << min << ":" << std::setw(2) << s;
        return ss.str();
    }

    [[nodiscard]] std::string to_duration_printable() const {
        std::stringstream ss;
        ss << y << " years, " << mon << " months, " << d << " days, "
           << h << " hrs, " << min << " min, " << s << " s. ";
        return ss.str();
    }

    [[nodiscard]] std::string serialize() const {
        return to_string();
    }

    //check if a year is leap
    static bool is_leap(int year) {
        if (year < 0)
            throw std::invalid_argument("negative year");
        return year != 0 &&
               (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0));
    }

    //returns the amount of days in a month (needs year to check Feb)
    //default year returns respectable default value (28) for Feb
    //If month and year are 0, returns the amount of days in the current month (localtime)
    static int days_in(int month, int year = 0);

    [[nodiscard]] double to_s_approx() const;

    [[nodiscard]] double to_min_approx() const;

    [[nodiscard]] double to_h_approx() const;

    [[nodiscard]] double to_d_approx() const;

    [[nodiscard]] double to_mon_approx() const;

    [[nodiscard]] double to_y_approx() const;

    static DateTime from_stream(std::istream& is);

    static unsigned get_weekday(int y, int m, int d) {
        if (y == 0 || m == 0 || d == 0) //unknown
            return 0;
        m = (m + 9) % 12;
        y -= m / 10;
        return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10 + (d - 1);
    }

    static std::string weekday_string(int y, int m, int d, bool short_str) {
        if (y == 0 || m == 0 || d == 0)
            return "-";
        unsigned val = get_weekday(y, m, d) % 7;
        return std::string((short_str ? WDAYS_SHORT.at(val) : WDAYS.at(val)));
    }

    static std::string number_postfix(int num) {
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

    [[nodiscard]] bool incomplete() const {
        return y == 0 || mon == 0 || d == 0;
    }

private:
    int y = 0;
    int mon = 0;
    int d = 0;
    int h = 0;
    int min = 0;
    int s = 0;

    static const std::regex date_time_regex;


};

