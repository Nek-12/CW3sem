//READ THIS FILE FROM BOTTOM TO TOP
//TODO: Add colors everywhere (AESTHETIC)
//TODO: Add customized greeting screen
//TODO: Make journals private members and provide the necessary methods
#include "util.hpp"
#include "Data.hpp"
#include "UI.hpp"

using namespace CONST; //NOLINT

void edit_habit(Habit& h) {

}

void edit_activity(Activity& a) {

}

void edit_goal(Goal& g) {

}

void habit_menu(Habit& h) {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(HABIT_MENU)) {
            case 0:
                std::cout << h.summary();
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
        if (!UI::yes_no("Continue working with " + h.get_name() + " ?"))
            return;
    } //loop
}

void activity_menu(Activity& a) {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(ACTIVITY_MENU)) {
            case 0:
                std::cout << a.summary();
                break;
            case 1:
                edit_activity(a);
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
        if (!UI::yes_no("Continue working with " + a.get_name() + " ?"))
            return;
    } //loop
}

void goal_menu(Goal& g) {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(GOAL_MENU)) {
            case 0:
                std::cout << g.summary();
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
        if (!UI::yes_no("Continue working with " + g.get_name() + " ?"))
            return;
    } //loop
}

//use RTTI to select which type of menu to use
void forward_entry_menu(Entry* pe) {
    if (!pe) return;
    if (typeid(*pe) == typeid(Habit))
        return habit_menu(*static_cast<Habit*>(pe));
    if (typeid(*pe) == typeid(Activity))
        return activity_menu(*static_cast<Activity*>(pe));
    if (typeid(*pe) == typeid(Habit))
        return goal_menu(*static_cast<Goal*>(pe));
    throw std::runtime_error("Unknown type encountered");
}

void search_entry() {
    auto& d = Data::get();
    std::vector<Entry*> sought;
    std::string s;
    DateTime dt;
    bool before; //NOLINT
    switch (UI::select_entry(SEARCH_MENU)) {
        case 0: //search by substring
            s = UI::get_string("What do you want to search for?", CHECK::LINE);
            sought = d.find(s); //find by name
            break;
        case 1: //search by date
            dt = UI::get_datetime();
            before = UI::yes_no("Do you want the entries before that date? \n" //TODO: Move to CONST
                                "If not, entries past the date will be shown");
            sought = d.find(dt, before);
            break;
        case 2: //search goals
            return;
    }
    if (sought.empty()) {
        std::cerr << "Nothing was found...\n";
        wait(WAIT_TIME);
        return;
    }
    //invoke a menu on the entry which the user selected from a list of the entries found
    forward_entry_menu(sought[UI::select_entry(sought, "Select an entry")]);
}

//select an entry from a list and operate on it or search
void main_menu() {
    auto& d = Data::get();
    while (true) {
        switch (UI::select_entry(MAIN_MENU, std::string(WELCOME_STR))) {
            case 0:
                habit_menu(UI::select_from(d.h));
            case 1:
                activity_menu(UI::select_from(d.a));
            case 2:
                goal_menu(UI::select_from(d.g));
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
        p = UI::get_string(ENTER_PASS, CHECK::PASS);
        if (p == UI::get_string(CONFIRM_PASS, CHECK::PASS))
            return p;
        std::cerr << PASS_DONT_MATCH << '\n';
        wait(WAIT_TIME);
    }
}

void log_in() {
    auto& d = Data::get();
    while (true) {
        std::string l, p;
        l = UI::get_string(ENTER_LOGIN, CHECK::WORD);
        if (lowercase(l) == "exit")
            return;
        p = UI::get_string(ENTER_PASS, CHECK::PASS);
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

int main(int /*unused*/, const char** /*unused*/) try {
    //Log::flip_cerr();
    Log() << Log::flush;
    auto& d = Data::get();
    bool out = false;
    while (!out)
        switch (UI::select_entry(START_MENU, std::string(WELCOME_STR))) {
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
} catch (std::exception& e) {
    std::cerr << "Something bad happened: \n" << e.what() << std::endl;
    if (UI::yes_no("Attempt to save your data? "))
        Data::get().save();
    return 1;
}
