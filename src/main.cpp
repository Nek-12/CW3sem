//READ THIS FILE FROM BOTTOM TO TOP

#include "util.hpp"
#include "Data.hpp"
#include "UI.hpp"
//TODO: to_string method to print a summary to console (for h,g,a)
//TODO: Replace push_back->emplace_back

int main(int, const char**) {
    //Log::flip_cerr();
#ifndef NDEBUG
    std::cout << "DEBUG IS ON\n";
    wait(1000);
#endif

    Log() << Log::flush;
    auto& d = Data::get();
    std::cout << UI::select_entry(CONST::HABIT_MENU, "Test a", "test b");

    wait(1000);
    return EXIT_SUCCESS;
}
