
#pragma once

#include "util.hpp"
#include <fort.hpp>
#include "CONST.hpp"
#include "Data.hpp"
#include <termios.h>
#include <concepts>

constexpr std::string_view CLS = "clear";

enum class KEY : int {
    ENTER = 10,
    BACKSPACE = 127,
    ARROW_1 = 27,
    ARROW_2 = 91,
    ARROW_LEFT = 68,
    ARROW_UP = 65,
    ARROW_RIGHT = 67,
    ARROW_DOWN = 66,
    SPACE = 32,
};


namespace color {

#define MAKE_COLOR_MANIPULATOR(name, code)                               \
  template < typename CharT, typename Traits = std::char_traits<CharT> > \
  inline std::basic_ostream< CharT, Traits >&                            \
  name(std::basic_ostream< CharT, Traits >& os)                          \
  { return os << (code); }

// These color definitions are based on the color scheme used by Git (the
// distributed version control system) as declared in the file
// `git/color.h`.
    MAKE_COLOR_MANIPULATOR(normal, "")

    MAKE_COLOR_MANIPULATOR(reset, "\033[m")

    MAKE_COLOR_MANIPULATOR(bold, "\033[1m")

    MAKE_COLOR_MANIPULATOR(red, "\033[31m")

    MAKE_COLOR_MANIPULATOR(green, "\033[32m")

    MAKE_COLOR_MANIPULATOR(yellow, "\033[33m")

    MAKE_COLOR_MANIPULATOR(blue, "\033[34m")

    MAKE_COLOR_MANIPULATOR(magenta, "\033[35m")

    MAKE_COLOR_MANIPULATOR(cyan, "\033[36m")

    MAKE_COLOR_MANIPULATOR(bold_red, "\033[1;31m")

    MAKE_COLOR_MANIPULATOR(bold_green, "\033[1;32m")

    MAKE_COLOR_MANIPULATOR(bold_yellow, "\033[1;33m")

    MAKE_COLOR_MANIPULATOR(bold_blue, "\033[1;34m")

    MAKE_COLOR_MANIPULATOR(bold_magenta, "\033[1;35m")

    MAKE_COLOR_MANIPULATOR(bold_cyan, "\033[1;36m")

    MAKE_COLOR_MANIPULATOR(bg_red, "\033[41m")

    MAKE_COLOR_MANIPULATOR(bg_green, "\033[42m")

    MAKE_COLOR_MANIPULATOR(bg_yellow, "\033[43m")

    MAKE_COLOR_MANIPULATOR(bg_blue, "\033[44m")

    MAKE_COLOR_MANIPULATOR(bg_magenta, "\033[45m")

    MAKE_COLOR_MANIPULATOR(bg_cyan, "\033[46m")

} // namespace color

class UI {

    static void set_table_style(fort::char_table& t, unsigned green_col, unsigned yellow_col); //NOLINT

    static std::string colorize_entry(const std::string_view& s, size_t len) {
        std::stringstream ss;
        ss << color::bold_green << "[ " << std::setw(len) << std::left
           << s << " ]" << color::reset;
        return ss.str();
    }

    static std::vector<std::string> to_readable_form(const Entry& e) {
        auto vec = split(e.serialize(), ";", true);
        vec.erase(vec.begin()); //remove the ID
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if (*it == "true")
                *it = "Yes";
            else if (*it == "false")
                *it = "No";
            std::replace(it->begin(), it->end(), ',', '\n');  //replace , with \n
        }
        return vec;
    }

    static std::vector<std::string> to_readable_form(const Entry* pe) {
        return to_readable_form(*pe);
    }

public:
    template<size_t C>
    static std::string as_table(const Entry& e, CONST::svarr<C> headers) {
        fort::char_table t;
        set_table_style(t, 0, 1);
        t << fort::header;
        for (const auto& el: headers)
            t << el;
        t << fort::endr;
        auto vec = to_readable_form(e);
        for (const auto& substr: vec)
            t << substr;
        t << fort::endr; //end the row
        return t.to_string();
    }

    template<std::ranges::range Iterable, size_t C>
    static std::string as_table(const Iterable& j, CONST::svarr<C> headers) {
        fort::char_table t;
        set_table_style(t, 0, 1);
        int no = 0;
        t << fort::header;
        t << "#"; //add a number column
        for (const auto& el: headers)
            t << el; //add other columns
        t << fort::endr;
        for (const auto& el: j) { //for each entry
            auto vec = to_readable_form(el);
            t << ++no; //add a number
            for (size_t i = 0; i < headers.size(); ++i)
                //display only those entries that we need
                //entries are specified by headers, so we'll skip all the unnecessary data
                //when using this function template with different data types
                t << vec[i];
            t << fort::endr; //end the row
        } //for each entry
        return t.to_string();
    }

    //returns a value in range [ 0, list.size() ) based on what the user selects
    template<typename Iterable>
    static size_t select_entry(const Iterable& list,
                               const std::string& pre_data = "",
                               const std::string& post_data = "") {
        if (list.empty())
            throw std::invalid_argument("Nothing to choose from!");
        size_t selected = 0;
        int c = 0; //NOLINT
        do {
            cls();
            std::cout << pre_data << "\n\n";
            print(list, selected);
            std::cout << post_data << "\n";
            c = getch();
            Log() << "Getch() = " << c;
            if (c == static_cast<int>(KEY::ARROW_1) && getch() == static_cast<int>(KEY::ARROW_2)) {
                switch (getch()) {
                    case static_cast<int>(KEY::ARROW_DOWN):
                        if (selected < list.size() - 1)
                            ++selected;
                        else
                            selected = 0;
                        break;
                    case static_cast<int>(KEY::ARROW_UP):
                        if (selected > 0)
                            --selected;
                        else selected = list.size() - 1;
                        break;
                    default:
                        break;
                }
            }
        } while (c != static_cast<int>(KEY::ENTER) && c != static_cast<int>(KEY::SPACE));
        return selected;
    }

    template<typename T>
    static T& select_from(Journal<T>& j) {
        return j[select_entry(j.as_names())];
    }

    template<typename Iterable>
    static void print(const Iterable& list, size_t selected = 0) {
        size_t max_len = 0;
        for (const auto& el: list)
            if (el.size() > max_len)
                max_len = el.size();
        for (size_t i = 0; i < list.size(); ++i) {
            if (i == selected)
                std::cout << colorize_entry(list[i], max_len) << "\n";
            else
                std::cout << "[ " << std::setw(max_len) << std::left << list[i] << " ]\n";
        }
        std::cout << std::endl;
    }

    static void print(const std::vector<Entry*>& vec, size_t selected = 0) {
        std::vector<std::string> name_vec;
        for (auto* el : vec)
            name_vec.push_back(el->get_name());
        print(name_vec, selected);
    }

    static int getch();

    static inline void pause() {
        std::cerr << CONST::ANY_KEY << std::endl;
        getch();
    }

    template<typename Printable>
    static std::string get_string(const Printable& msg, CHECK mode) {
        while (true) {
            cls();
            std::cout << color::blue << msg << color::reset << '\n';
            std::string s;
            if (mode == CHECK::PASS)
                s = get_password();
            else
                std::getline(std::cin, s);
            std::pair<bool, std::string> res = check_string(s, mode);
            if (!res.first) {
                std::cout << color::red << res.second << "\nTry again. \n" << color::reset;
                wait(CONST::WAIT_TIME * 2);
                continue;
            }
            return s;
        }
    }

    static bool yes_no(const std::string& msg) { // Asks for confirmation
        std::cout << color::blue << msg << " y/n" << color::reset << std::endl;
        while (true) {
            switch (tolower(getch())) {
                case 'y':
                    return true;
                case 'n':
                    return false;
                default:
                    break;
            }
        }
    }

    static void cls() { system(std::string(CLS).c_str()); } //NOLINT

    static std::string get_password();

    static DateTime get_datetime() {
        while (true) {
            try {
                cls();
                std::cout << color::blue << "Enter the date in the format: \n"
                                            "YYYY-MM-DD HH:MM:SS\n" << color::reset;
                return DateTime::from_stream(std::cin);
            } catch (std::invalid_argument& e) {
                std::cout << color::red << "Try again: " << e.what() << color::reset << std::endl;
                wait(CONST::WAIT_TIME * 2);
            }
        }
    }

    static DateTime get_time() {
        while (true) {
            try {
                cls();
                std::string s = get_string("Enter the time in the format\nHH:MM:SS", CHECK::TIME);
                s.insert(0, "0000-00-00 ");
                Log() << "Final string from get_time: " << s;
                return DateTime::deserialize(s);
            } catch (std::invalid_argument& e) {}
        }
    }

    static id_type get_number_in_range(id_type min, id_type max) {
        std::stringstream ss;
        ss << "Enter the number from " << min << " to " << max;
        while (true) {
            id_type no = stoid(get_string(ss.str(), CHECK::ID));
            if (no > max || no < min) {
                std::cout << color::red << "Wrong value, try again\n" << color::reset;
                wait(CONST::WAIT_TIME);
                continue;
            }
            return no;
        }
    }

};



