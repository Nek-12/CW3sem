//! READ THIS FILE FROM BOTTOM TO TOP
#include "App.hpp"
//TODO: Comment the code
//TODO: Make journals private members and provide the necessary methods

int main(int /*unused*/, const char** /*unused*/) try {
    //Log::flip_cerr();
    Log() << Log::flush;
    bool out = false;
    App app;
    while (!out)
        switch (UI::select_entry(START_MENU, std::string(WELCOME_STR))) {
            case 0:
                app.log_in();
                break;
            case 1:
                out = true;
                break;
            default:
                break;
        }
    return EXIT_SUCCESS;
} catch (std::exception& e) {
    std::cout << '\a' << color::red << "Something bad happened: \n" << e.what() << color::reset << std::endl;
    Log() << e.what();
    if (UI::yes_no("Attempt to save your data? "))
        Data::get().save();
    return 1;
}
