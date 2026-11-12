#include <ctime>
extern "C" time_t game_time_test_clock(time_t* target);
#define time game_time_test_clock
