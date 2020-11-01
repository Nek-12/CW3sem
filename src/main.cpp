//READ THIS FILE FROM BOTTOM TO TOP

#include "util.hpp"
#include "Data.hpp"
#include "UI.hpp"
//TODO: to_string method to print a summary to console (for h,g,a)
//TODO: Replace push_back->emplace_back







int main(int, const char**) {
    Log::flip_cerr();
    Log() << Log::flush;
    auto& d = Data::get();
    d.create_user("admin", "admin");
    d.h.push_back(Habit(gen_id(), "Drink water", 1));
    d.a.push_back(Activity(gen_id(), "Study", 15));
    d.g.push_back(Goal(gen_id(), "Get 10 on CW", 60));
    d.save();
    wait(1000);
    d.load();
    wait(1000);
    d.save();
    wait(1000);
    return EXIT_SUCCESS;
}
