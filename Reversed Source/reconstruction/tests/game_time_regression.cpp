#include "../../game/src/utils.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
static time_t clocks[8];
static int clockIndex;
static int clockCount;
static time_t formatted;
static int logs;
static char logged[128];
extern "C" time_t game_time_test_clock(time_t* target)
{
    assert(clockIndex < clockCount);
    time_t result = clocks[clockIndex++];
    if (target) *target = result;
    return result;
}
extern "C" char* time_str(time_t value)
{
    formatted = value;
    static char text[] = "0123456789012345678901234567890123456789";
    return text;
}
extern "C" void sys_log(int level, const char* format, ...)
{
    assert(level == 0);
    ++logs;
    va_list ap; va_start(ap, format);
    vsnprintf(logged, sizeof(logged), format, ap);
    va_end(ap);
}
static void Script(time_t first, time_t second, int count)
{
    clocks[0] = first; clocks[1] = second;
    clockIndex = 0; clockCount = count;
}
int main()
{
    Script(100, 0, 1);
    assert(get_global_time() == 100 && clockIndex == 1);
    Script(100, 101, 2);
    set_global_time(150);
    assert(clockIndex == 2 && formatted == 151 && logs == 1);
    assert(strcmp(logged, "GLOBAL_TIME: 0123456789012345678901234567890 time_gap 50") == 0);
    Script(110, 0, 1);
    assert(get_global_time() == 160);
    Script(200, 200, 2);
    set_global_time(150);
    assert(formatted == 150 && logs == 2);
    assert(strcmp(logged, "GLOBAL_TIME: 0123456789012345678901234567890 time_gap -50") == 0);
    Script(195, 0, 1);
    assert(get_global_time() == 145);
    Script(0, 0, 2);
    set_global_time(static_cast<time_t>(2147483647));
    Script(1, 0, 1);
    assert(get_global_time() == static_cast<time_t>(-2147483647 - 1));
    Script(static_cast<time_t>(2147483647), static_cast<time_t>(2147483647), 2);
    set_global_time(static_cast<time_t>(-2147483647 - 1));
    assert(strstr(logged, "time_gap 1") != 0);
    puts("game global time regression passed");
}
