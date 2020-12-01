//READ THIS FILE FROM BOTTOM TO TOP

#include "util.hpp"
#include "Data.hpp"
#include "UI.hpp"

using namespace CONST;


int main(int, const char**) {
    //Log::flip_cerr();
    Log() << Log::flush;
    auto& d = Data::get();
    switch (UI::select_entry(START_MENU, WELCOME_STR) {
        case 0:
            log_in();
            break;
        case 1:
            break;
    }
    d.save();
    return EXIT_SUCCESS;
}
