#include "util.hpp"
#include "Journal.h"
#include "Entry.hpp"


#include <thread>
//TODO: to_string method to print a summary to console

int main(int, const char**) {
    Log::flip_cerr();
    Log() << Log::flush;
    DateTime dt = DateTime::get_current();
    DateTime dt2 = DateTime(2002, 2, 28, 0, 0, 0);
    std::cout << dt - dt2 << "max_double: " << std::numeric_limits<double>::max();
    Habit h(gen_id(), "habit", 2);
    Activity a(gen_id(), "activ", 3);
    Goal g(gen_id(), "goal", -5);
    h.check_in();
    a.add_time(DateTime(0, 0, 0, 5, 50, 0));
    a.add_time(DateTime(0, 0, 0, 12, 9, 15));
    g.toggle_completed();
    g.set_deadline(DateTime(2021, 01, 01, 0, 0, 0));
    std::cout << "time left: " << g.time_left() << '\n';
    std::cout << a.serialize() << '\n' << g.serialize() << '\n' << h.serialize() << '\n';
    auto str = g.serialize();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto g2 = Goal::deserialize(str);
    std::cout << "----------------\n" << g.serialize() << '\n' << g2.serialize() << '\n';

    return EXIT_SUCCESS;
}
