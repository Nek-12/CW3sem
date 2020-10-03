#include "util.hpp"

int main(int argc, const char** argv) {
    Log() << Log::flush;
    fort::char_table t;
    t << fort::header << "HELLO" << fort::endr;
    std::cout << t.to_string() << std::endl;
    DateTime dt = DateTime::get_current();
    DateTime dt2 = DateTime(2002,2,28,0,0,0);
    std::cout << dt-dt2;

    return EXIT_SUCCESS;
}
