// Compile the production duration helper; keep unrelated game-clock helpers private to this test object.
// Clock calls in these unused renamed helpers abort; CalculateDuration itself is unchanged production code.
#include <ctime>
#include <cstdlib>
#include "../../game/src/utils.h"
#include "../../libthecore/include/utils.h"
static time_t duration_unused_time(time_t*) { std::abort(); }
static char* duration_unused_time_str(time_t) { std::abort(); }
#define time duration_unused_time
#define time_str duration_unused_time_str
#define get_global_time duration_unused_get_global_time
#define set_global_time duration_unused_set_global_time
#include "../../game/src/utils.cpp"
#undef set_global_time
#undef get_global_time
#undef time_str
#undef time
