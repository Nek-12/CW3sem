//READ THIS FILE FROM BOTTOM TO TOP
//TODO: Add colors everywhere (AESTHETIC)
//TODO: Add customized greeting screen
//TODO: Make journals private members and provide the necessary methods
#include "Data.hpp"
#include "UI.hpp"
#include "util.hpp"
//TODO: Comment the code
//TODO: Translate to Russian
//FIXME: Fix the streak calculation for habit
//TODO: Add stats, total points summary and other
//TODO: add sorting
//TODO: Implement foltering by archived / not archived


using namespace CONST; //NOLINT

bool edit_entry(Entry& e, size_t selected) { //the item must be already selected before (it's a wrapper)
    auto& d = Data::get();
    switch (selected) {
        case 0: {//Rename
            std::string name = UI::get_string("Enter a new title: ", CHECK::LINE);
            std::stringstream ss;
            ss << "Rename " << e.get_name() << " to " << name << " ?";
            if (UI::yes_no(ss.str()))
                e.rename(name);
        }
            return false;
        case 1: { //change points cost
            double val = std::stod(UI::get_string("Enter the new point cost", CHECK::DOUBLE));
            std::stringstream ss;
            ss << "Would you like to replace " << e.get_cost()
               << " with " << val << " ?";
            if (UI::yes_no(ss.str()))
                e.set_cost(val);
        }
            return false;
        case 2:
            if (UI::yes_no("Would you really like to delete " + e.get_name() + " ?")) {
                if (d.erase(e)) return true;
                throw std::runtime_error("This entry is not in a journal" + e.get_name());
            }
            break;
        default:
            throw std::invalid_argument("Invalid selected value");
    }
    return false;
}

//"Rename", "Change points cost", "Delete", "Mark completed", "Archive/Unarchive", "Go back"
void habit_menu(Habit& h) {
    while (true) {
        auto sel = UI::select_entry(HABIT_MENU, " ", h.summary());
        switch (sel) {
            case 0:
            case 1:
            case 2:
                if (edit_entry(h, sel))
                    return; //the entry was deleted
                break;
            case 3:
                if (h.check_in())
                    std::cout << "Checked in at " << DateTime::get_current().to_printable(true) << '\n';
                else
                    std::cout << color::red << "Can't check in archived habit!\n" << color::reset;
                wait(WAIT_TIME);
                break;
            case 4:
                h.swap_archived();
                std::cout << h.get_name() << " is now "
                          << (h.is_archived() ? "archived " : "not archived") << '\n';
                wait(WAIT_TIME);
                break;
            case 5:
                return;
            default:
                throw std::invalid_argument("This habit menu entry should not be available");
        }
        if (!UI::yes_no("Continue working with " + h.get_name() + " ?"))
            return;
    } //loop
}

//"Rename", "Change points cost",  "Delete", "Add time", "Change multiplier", "Go back"
void activity_menu(Activity& a) {
    while (true) {
        auto sel = UI::select_entry(ACTIVITY_MENU, " ", a.summary());
        switch (sel) {
            case 0:
            case 1:
            case 2:
                if (edit_entry(a, sel))
                    return; //the entry was deleted
                break;
            case 3: {
                auto dt = UI::get_time();
                if (UI::yes_no("Add this time? " + dt.to_duration_printable()))
                    a.add_time(dt);
            }
                break;
            case 4: {
                double val = std::stod(UI::get_string("Enter the new multiplier", CHECK::DOUBLE));
                std::stringstream ss;
                ss << "Would you like to replace " << a.get_benefit_multiplier()
                   << " with " << val << " ?";
                if (UI::yes_no(ss.str()))
                    a.set_benefit_multiplier(val);
            }
                break;
            case 5:
                return;
            default:
                throw std::invalid_argument("This activity menu entry should not be available");
        }
        if (!UI::yes_no("Continue working with " + a.get_name() + " ?"))
            return;
    } //loop
}

//"Rename", "Change points cost",  "Delete", "Toggle", "Change duration", "Set deadline","Go back"
void goal_menu(Goal& g) {
    while (true) {
        auto sel = UI::select_entry(GOAL_MENU, g.summary());
        switch (sel) {
            case 0:
            case 1:
            case 2:
                if (edit_entry(g, sel))
                    return; //the entry was deleted
                break;
            case 3:
                g.toggle_completed();
                std::cout << g.get_name() << " is now "
                          << (g.is_completed() ? "completed " : "not completed") << '\n';
                wait(WAIT_TIME);
                break;
            case 4: {
                DateTime dt = UI::get_datetime();
                if (UI::yes_no("Set this date as a new estimated duration? "))
                    g.set_est_length(dt);
            }
                break;
            case 5: {
                DateTime dt = UI::get_datetime();
                if (UI::yes_no("Set this date as a new deadline? "))
                    g.set_deadline(dt);
            }
                break;
            case 6:
                return;
            default:
                throw std::invalid_argument("This goal menu entry should not be available");
        }
        if (!UI::yes_no("Continue working with " + g.get_name() + " ?"))
            return;
    } //loop
}

//use RTTI to select which type of menu to use
void forward_entry_menu(Entry* pe) {
    if (!pe) return;
    if (typeid(*pe) == typeid(Habit))
        return habit_menu(*static_cast<Habit*>(pe)); //NOLINT
    if (typeid(*pe) == typeid(Activity))
        return activity_menu(*static_cast<Activity*>(pe)); //NOLINT
    if (typeid(*pe) == typeid(Habit))
        return goal_menu(*static_cast<Goal*>(pe)); //NOLINT
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
        std::cout << color::red << "Nothing was found...\n" << color::reset;
        wait(WAIT_TIME);
        return;
    }
    //invoke a menu on the entry which the user selected from a list of the entries found
    forward_entry_menu(sought[UI::select_entry(sought, "Select an entry")]);
}

Habit& add_habit() {
    std::string name = UI::get_string("Enter the title: ", CHECK::LINE);
    double cost = std::stod(UI::get_string("Enter the point cost: ", CHECK::DOUBLE));
    return Data::get().h.emplace_back(gen_id(), name, cost);
}

Activity& add_activity() {
    std::string name = UI::get_string("Enter the title: ", CHECK::LINE);
    double cost = std::stod(UI::get_string("Enter the point cost: ", CHECK::DOUBLE));
    return Data::get().a.emplace_back(gen_id(), name, cost);
}

Goal& add_goal() {
    std::string name = UI::get_string("Enter the title: ", CHECK::LINE);
    double cost = std::stod(UI::get_string("Enter the point cost: ", CHECK::DOUBLE));
    auto& d = Data::get();
    Goal& g = d.g.emplace_back(gen_id(), name, cost);
    return g;
}

//select an entry from a list and operate on it or search
void main_menu() {
    auto& d = Data::get();
    auto adder = [](std::vector<std::string_view> vals) {
        vals.emplace_back("+ Add New +");
        vals.emplace_back(" <- Go back ");
        return vals;
    };
    while (true) {
        switch (UI::select_entry(MAIN_MENU, std::string(WELCOME_STR))) {
            case 0: { //habits
                std::vector<std::string_view> vals = adder(d.h.as_names());
                size_t selected = UI::select_entry(vals, "", UI::as_table(d.h, HEADERS_HABIT));
                if (selected == vals.size() - 1) // "go back"
                    break;
                else if (selected == vals.size() - 2) //add new
                    habit_menu(add_habit());
                else habit_menu(d.h[selected]);
            }
                break;
            case 1: { //activities
                auto vals = adder(d.a.as_names());
                size_t selected = UI::select_entry(vals, "", UI::as_table(d.a, HEADERS_ACTIVITY));
                if (selected == vals.size() - 1) // "go back"
                    break;
                else if (selected == vals.size() - 2) //add new
                    activity_menu(add_activity());
                else activity_menu(d.a[selected]);
            }
                break;
            case 2: { //activities
                auto vals = adder(d.g.as_names());
                size_t selected = UI::select_entry(vals, "", UI::as_table(d.g, HEADERS_GOAL));
                if (selected == vals.size() - 1) // "go back"
                    break;
                else if (selected == vals.size() - 2) //add new
                    goal_menu(add_goal());
                else goal_menu(d.g[selected]);
            }
                break;
            case 3:
                search_entry();
                break;
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
        std::cerr << color::blue << "Creating account for user " << l << '\n' << color::reset;
        wait(WAIT_TIME);
        p = UI::get_string(ENTER_PASS, CHECK::PASS);
        if (p == UI::get_string(CONFIRM_PASS, CHECK::PASS))
            return p;
        std::cout << color::red << PASS_DONT_MATCH << '\n' << color::reset;
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
            if (Data::user_exists(l)) {
                std::cout << color::red << INVALID_PASS << color::reset << '\n';
                wait(WAIT_TIME);
                continue;
            }
            //user does not exist
            std::cout << color::yellow << "Account " << l << " was not found... \n"
                      << color::reset;
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
    std::cout << color::red << "Something bad happened: \n" << e.what() << color::reset << std::endl;
    Log() << e.what();
    if (UI::yes_no("Attempt to save your data? "))
        Data::get().save();
    return 1;
}
