//
// Created by nek on 2020-10-02.
//

#pragma once
#include <string>
namespace CONST {
    using sv = std::string_view;
    template<size_t T>
    using svarr = typename std::array<sv, T>;

    static constexpr sv WELCOME_STR = "Welcome to SOMeter CLI alpha. Use arrow keys to navigate.";
    static constexpr svarr<2> START_MENU{"Log in", "Exit"};
    static constexpr svarr<6> MAIN_MENU{"Progress", "Habits", "Activities", "Goals", "Search", "Log out"};
    static constexpr svarr<4> ENTRY_MENU{"Select", "View summary", "New", "Go Back"};
    static constexpr svarr<7> GOAL_MENU{"Summary", "Edit", "Delete", "Toggle", "Change duration", "Set deadline",
                                        "Go back"};
    static constexpr svarr<6> ACTIVITY_MENU{"Summary", "Edit", "Delete", "Add time", "Change multiplier", "Go back"};
    static constexpr svarr<7> HABIT_MENU{"Summary", "Edit", "Delete", "Check/uncheck", "Archive", "History", "Go back"};
    //TODO: also implement check-ins from the main menu
    static constexpr svarr<3> SEARCH_MENU{"Search by name", "Filter by date", "Go back"};
    static constexpr double ACTIVITY_MULTIPLIER_PER_HOUR = 1.0;
} //namespace CONST


/* File template:
 * 1st line: password hashed
 * 2...EOF: entries
*/
