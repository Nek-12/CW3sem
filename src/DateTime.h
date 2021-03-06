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

    //used in deserialize, specifies constraints on the datetime value
    enum {
        PAST = 1, FUTURE = 0, ANY = -1
    };


    friend DateTime operator-(const DateTime& lhs, const DateTime& rhs);

    friend DateTime operator+(const DateTime& lhs, const DateTime& rhs);

    DateTime operator-=(const DateTime& rhs);

    DateTime operator+=(const DateTime& rhs);

    friend bool operator<(const DateTime& lhs, const DateTime& rhs);

    friend bool operator>(const DateTime& lhs, const DateTime& rhs);

    friend bool operator==(const DateTime& lhs, const DateTime& rhs);

    friend bool operator!=(const DateTime& lhs, const DateTime& rhs);

    friend bool operator<=(const DateTime& lhs, const DateTime& rhs);

    friend bool operator>=(const DateTime& lhs, const DateTime& rhs);

    friend std::ostream& operator<<(std::ostream& os, const DateTime& t);

    //returns a value at a position from 0 to 5, may throw
    int operator[](int n) const;

    //Returns true if the date is not blank (0000-00-00 00:00:00)
    bool operator!() const;

    //create a blank value 0000-00-00 00:00:00
    DateTime() = default;

    DateTime(int year, int month, int day, int hour, int minute, int second);

    static DateTime get_current();

    static DateTime from_epoch(long s);

    //PAST - perform a check if a date should be in the past (according to local time)
    //FUTURE - in the future
    //ANY - skip the check
    static DateTime deserialize(const std::string& str, int is_past = ANY);

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] std::string to_printable(bool short_date) const;

    [[nodiscard]] std::string to_duration_printable() const;

    [[nodiscard]] std::string serialize() const { return to_string(); }

    //check if a year is leap
    static bool is_leap(int year);

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

    static unsigned get_weekday(int y, int m, int d);

    static std::string weekday_string(int y, int m, int d, bool short_str);

    static std::string number_postfix(int num);

    [[nodiscard]] bool incomplete() const;

    [[nodiscard]] time_t to_epoch() const;

    [[nodiscard]] tm to_tm_struct() const;

private:
    int y = 0;
    int mon = 0;
    int d = 0;
    int h = 0;
    int min = 0;
    int s = 0;

    //calculate a magic number for date operations
    static int g(int y, int m, int d);

    static const std::regex date_time_regex;


};

