//Unreachable initialization clock boundary for isolated descriptor/input suites.
//Real clock and actual initialization are tested in their integration suites.
#include <cstdlib>
extern "C" float get_float_time() { std::abort(); }
extern "C" long double get_float_time_extended() { std::abort(); }

extern "C" int MINMAX(int,int,int) { std::abort(); }
