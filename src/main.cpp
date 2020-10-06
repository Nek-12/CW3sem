#include "util.hpp"
#include "Journal.h"
#include "Entry.hpp"

int main(int , const char** ) {
    Log() << Log::flush;
    DateTime dt = DateTime::get_current();
    DateTime dt2 = DateTime(2002,2,28,0,0,0);
    std::cout << dt-dt2 << "max_double: " << std::numeric_limits<double>::max();
    Journal<Habit> j;
    j.push_back(Habit(gen_id()));
    j[0].rename("HEllo");
    j.push_back(Habit(gen_id()));
    j[1].rename("CHIKFDJKLSKF:DSJ");
    j.emplace_back(gen_id());

    return EXIT_SUCCESS;
}
