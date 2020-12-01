//READ THIS FILE FROM BOTTOM TO TOP
//TODO: Add colors everywhere (AESTHETIC)
//TODO: Add customized greeting screen
#include "util.hpp"
#include "Data.hpp"
#include "UI.hpp"

using namespace CONST; //NOLINT

void habit_menu() {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(HABIT_MENU)) {
            case 0:
                //TODO: Summary
                break;
            case 1:
                //TODO: Edit
                break;
            case 2:
                //TODO: Delete
                break;
            case 3:
                //TODO: Check/Uncheck
                break;
            case 4:
                //TODO: Change duration
                break;
            case 5:
                //TODO: Deadline
                break;
            case 6:
                return;
        }
    } //loop
}

void activity_menu() {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(ACTIVITY_MENU)) {
            case 0:
                //TODO: Summary
                break;
            case 1:
                //TODO: Edit
                break;
            case 2:
                //TODO: Delete
                break;
            case 3:
                //TODO: Add time
                break;
            case 4:
                //TODO: Change multiplier
                break;
            case 5:
                return;
        }
    } //loop
}

void goal_menu() {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(GOAL_MENU)) {
            case 0:
                //TODO: Summary
                break;
            case 1:
                //TODO: Edit
                break;
            case 2:
                //TODO: Delete
                break;
            case 3:
                //TODO: Toggle
                break;
            case 4:
                //TODO: Set duration
                break;
            case 5:
                //TODO: Deadline
                break;
            case 6:
                return;
        }
    } //loop
}

void search_entry() {
    auto& d = Data::get();
    switch (UI::select_entry(SEARCH_MENU)) {
        case 0:
            //TODO: Search by name
            break;
        case 1:
            //TODO: Search by date
            break;
        case 2:
            return;
    }
}

void select_from() {
    //TODO: Implement
}

void main_menu() {
    while (true) {
        switch (UI::select_entry(MAIN_MENU, WELCOME_STR)) {
            case 0:
                habit_menu(); //TODO: Replace with select_from() all of these
            case 1:
                activity_menu();
            case 2:
                goal_menu();
            case 3:
                search_entry();
            case 4:
                return;
            default:
                break;
        }
    }
}

//returns a password for a newly created account
std::string create_account(const std::string& l) {
    while (true) {
        std::string p;
        UI::cls();
        std::cout << "Creating account for user " << l << '\n';
        wait(WAIT_TIME);
        p = UI::read_string(ENTER_PASS, CHECK::PASS);
        if (p == UI::read_string(CONFIRM_PASS, CHECK::PASS))
            return p;
        std::cerr << PASS_DONT_MATCH << '\n';
        wait(WAIT_TIME);
    }
}

void log_in() {
    auto& d = Data::get();
    while (true) {
        std::string l, p;
        l = UI::read_string(ENTER_LOGIN, CHECK::WORD);
        if (lowercase(l) == "exit")
            return;
        p = UI::read_string(ENTER_PASS, CHECK::PASS);
        if (!d.attempt_login(l, p)) {
            if (d.user_exists(l)) {
                std::cout << color::red << INVALID_PASS << color::reset << '\n';
                wait(WAIT_TIME);
                continue;
            }
            //user does not exist
            if (UI::yes_no("Create a new account?")) {
                p = create_account(l);
                d.create_user(l, p);
            } else continue;
        }
        d.load();
        main_menu();
    }
}

int main(int, const char**) {
    //Log::flip_cerr();
    Log() << Log::flush;
    auto& d = Data::get();
    bool out = false;
    while (!out)
        switch (UI::select_entry(START_MENU, WELCOME_STR)) {
            case 0:
                log_in();
                break;
            case 1:
                out = true;
                break;
            default:
                break;
        }
    d.save();
    return EXIT_SUCCESS;
}
