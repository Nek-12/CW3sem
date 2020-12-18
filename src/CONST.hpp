#pragma once

#include <string>

namespace CONST {
    using sv = std::string_view;

    template<size_t T>
    using svarr = typename std::array<sv, T>;

    static constexpr int WAIT_TIME = 1000;
    static constexpr sv ANY_KEY = "Press any key to continue... ";
    static constexpr sv WELCOME_STR = "Welcome to SOMeter CLI alpha. Use arrow keys to navigate.";
    static constexpr sv ENTER_LOGIN = "Enter your login or \"exit\" to go back: ";
    static constexpr sv ENTER_PASS = "Enter your password: ";
    static constexpr sv INVALID_PASS = "Invalid password.";
    static constexpr sv CONFIRM_PASS = "Please confirm your password: ";
    static constexpr sv PASS_DONT_MATCH = "Your passwords don't match!. ";
    static constexpr sv SORT_BY_STR = "Sort entries by: ";
    static constexpr sv VALS_SORTED_MSG = "Your entries were sorted. View them using other menu entries.";
    static constexpr svarr<2> START_MENU{"Log in", "Exit"};
    static constexpr svarr<11> MAIN_MENU{"Summary", "Habits", "Activities", "Goals", "Search", "Display everything",
                                         "Top-3 Entries", "Sort", "Change password", "Delete account",
                                         "Save and log out"};
    static constexpr svarr<7> GOAL_MENU{"Toggle completed", "Rename", "Change points cost", "Delete", "Change duration",
                                        "Set deadline", "Go back"};
    static constexpr svarr<7> ACTIVITY_MENU{"Archive", "Rename", "Change points cost", "Delete", "Add time",
                                            "Change multiplier",
                                            "Go back"};
    static constexpr svarr<6> HABIT_MENU{"Archive/Unarchive", "Rename", "Change points cost", "Delete", "Check in",
                                         "Go back"};
    static constexpr svarr<3> SEARCH_MENU{"Search", "Filter by date", "Go back"};
    static constexpr double ACTIVITY_MULTIPLIER_PER_HOUR = 1.0;
    static constexpr svarr<7> HEADERS_ACTIVITY{"Name", "Cost", "Created", "Is archived?", "Time elapsed", "Total time",
                                               "Benefit multiplier"};
    static constexpr svarr<6> HEADERS_GOAL{"Name", "Cost", "Created", "Completed", "Est. Length", "Deadline"};
    static constexpr svarr<7> HEADERS_HABIT{"Name", "Cost", "Created", "Is archived?", "Check-ins", "Best streak",
                                            "Streak"};
    static constexpr svarr<4> SORT_ENTRIES_MENU{"By name", "By date created", "By points earned", "Go back"};
} //namespace CONST


/* File template:
 * 1st line: password hashed
 * 2...EOF: entries
*/
