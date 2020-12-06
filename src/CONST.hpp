#pragma once
#include <string>
namespace CONST {
    using sv = std::string_view;

    template<size_t T>
    using svarr = typename std::array<sv, T>;

    static constexpr int WAIT_TIME = 700;
    static constexpr sv ANY_KEY = "Press any key to continue... ";
    static constexpr sv WELCOME_STR = "Welcome to SOMeter CLI alpha. Use arrow keys to navigate.";
    static constexpr sv ENTER_LOGIN = "Enter your login or \"exit\" to go back: ";
    static constexpr sv ENTER_PASS = "Enter your password: ";
    static constexpr sv INVALID_PASS = "Invalid password.";
    static constexpr sv CONFIRM_PASS = "Please confirm your password: ";
    static constexpr sv PASS_DONT_MATCH = "Your passwords don't match!. ";
    static constexpr svarr<2> START_MENU{"Log in", "Exit"};
    static constexpr svarr<5> MAIN_MENU{"Habits", "Activities", "Goals", "Search", "Log out"};
    static constexpr svarr<4> ENTRY_MENU{"Select", "View summary", "New", "Go Back"};
    static constexpr svarr<7> GOAL_MENU{"Summary", "Edit", "Delete", "Toggle", "Change duration", "Set deadline",
                                        "Go back"};
    static constexpr svarr<6> ACTIVITY_MENU{"Summary", "Edit", "Delete", "Add time", "Change multiplier", "Go back"};
    static constexpr svarr<7> HABIT_MENU{"Summary", "Edit", "Delete", "Mark completed", "Archive", "History",
                                         "Go back"};
    static constexpr svarr<3> SEARCH_MENU{"Search", "Filter by date", "Go back"};
    static constexpr double ACTIVITY_MULTIPLIER_PER_HOUR = 1.0;
    static constexpr svarr<6> HEADERS_ACTIVITY{"Name", "Cost", "Created", "Time elapsed", "Total time",
                                               "Benefit multiplier"};
    static constexpr svarr<6> HEADERS_GOAL{"Name", "Cost", "Created", "Completed", "Est. Length", "Deadline"};
    static constexpr svarr<7> HEADERS_HABIT{"Name", "Cost", "Created", "Check-ins", "Is archived?", "Best streak",
                                            "Streak"};
} //namespace CONST


/* File template:
 * 1st line: password hashed
 * 2...EOF: entries
*/
