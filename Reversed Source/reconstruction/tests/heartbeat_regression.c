// Deterministic tests of the real reconstructed heart.c and timing helpers.
// Only clock/select/log calls are fixtures. No real sleeping or server runs.
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/select.h>
#include "../../libthecore/include/heart.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"

static struct timeval clock_values[8];
static int clock_count, clock_index, select_calls, select_result, select_error;
static struct timeval selected_timeout;
static struct timeval* selected_pointer;
static int errors, callback_calls, error_line;
static const char* error_function;
static const char* error_format;
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;

int gettimeofday(struct timeval* value, struct timezone* zone)
{
    assert(zone == NULL && clock_index < clock_count);
    *value = clock_values[clock_index++];
    return 0;
}

int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
           struct timeval* timeout)
{
    assert(nfds == 0 && !readfds && !writefds && !exceptfds);
    ++select_calls;
    selected_timeout = *timeout;
    selected_pointer = timeout;
    timeout->tv_sec = timeout->tv_usec = 0; // Model a syscall modifying timeout.
    errno = select_error;
    return select_result;
}

void _sys_err(const char* function, int line, const char* format, ...)
{
    ++errors;
    error_function = function;
    error_line = line;
    error_format = format;
}

static void Callback(LPHEART heart, int pulse)
{
    (void)heart;
    (void)pulse;
    ++callback_calls;
}

static void Clock(long before_sec, long before_usec, long now_sec, long now_usec)
{
    clock_index = 0;
    clock_count = 2;
    clock_values[0].tv_sec = before_sec;
    clock_values[0].tv_usec = before_usec;
    clock_values[1].tv_sec = now_sec;
    clock_values[1].tv_usec = now_usec;
}

static void CheckUtilities(void)
{
    struct timeval a = {10, 100000};
    struct timeval b = {9, 900000};
    struct timeval* result = timediff(&a, &b);
    struct timeval* borrowed = result;
    assert(result->tv_sec == 0 && result->tv_usec == 200000);
    result = timediff(&a, &a);
    assert(result == borrowed && !result->tv_sec && !result->tv_usec);
    result = timediff(&b, &a);
    assert(result != borrowed && !result->tv_sec && !result->tv_usec);
    a.tv_sec = b.tv_sec;
    assert(timediff(&a, &b) == result); // Same-second negative difference.
    a.tv_sec = 1;
    a.tv_usec = 900000;
    b.tv_sec = 2;
    b.tv_usec = 2300000;
    result = timeadd(&a, &b);
    assert(result->tv_sec == 6 && result->tv_usec == 200000);
    a.tv_usec = -100;
    b.tv_usec = 0;
    result = timeadd(&a, &b);
    assert(result->tv_sec == 3 && result->tv_usec == -100);

    a.tv_sec = 0;
    a.tv_usec = 123;
    thecore_sleep(&a);
    assert(selected_pointer == &a && selected_timeout.tv_usec == 123);
    assert(a.tv_usec == 0 && errors == 0);
    select_result = -1;
    select_error = EINTR;
    thecore_sleep(&a);
    assert(errors == 0); // No retry or diagnostic on interruption.
    select_error = EINVAL;
    thecore_sleep(&a);
    assert(errors == 1 && !strcmp(error_function, "thecore_sleep"));
    assert(error_line == 355 && !strcmp(error_format, "select sleep %s"));
    select_result = select_error = 0;
    Clock(1057699978L, 123000, 1057699979L, 500);
    assert(get_dword_time() == 123);
    assert(get_dword_time() == 1000);
    {
        struct tm input = {0};
        struct tm* result;
        input.tm_year = 109;
        input.tm_mon = 0;
        input.tm_mday = 1;
        result = tm_calc(&input, -3);
        assert(result->tm_year == 108 && result->tm_mon == 11 && result->tm_mday == 29);
        input.tm_year = 108;
        input.tm_mon = 1;
        input.tm_mday = 10;
        result = tm_calc(&input, -3);
        assert(result->tm_mday == 8); // Old leap-day adjustment increments first.
        input.tm_year = 109;
        input.tm_mon = 0;
        input.tm_mday = 31;
        result = tm_calc(&input, 1);
        assert(result->tm_mon == 1 && result->tm_mday == -4);
    }
}

int main(void)
{
    LPHEART heart;
    int old_errors, old_selects;
    CheckUtilities();
    old_errors = errors;
    assert(heart_new(40000, NULL) == NULL);
    assert(errors == old_errors + 1 && !strcmp(error_format, "no function defined"));
    Clock(10, 0, 10, 0);
    heart = heart_new(40000, Callback);
    assert(heart && heart->func == Callback && heart->passes_per_sec == 25);
    assert(heart->pulse == 0 && heart->last_time.tv_sec == 10);
    assert(clock_index == 2);

    Clock(10, 10000, 10, 12000);
    old_selects = select_calls;
    assert(heart_idle(heart) == 1);
    assert(select_calls == old_selects + 1 && selected_timeout.tv_usec == 28000);
    assert(heart->last_time.tv_sec == 10 && heart->last_time.tv_usec == 40000);

    heart->last_time.tv_sec = 10;
    heart->last_time.tv_usec = 0;
    Clock(10, 130000, 10, 132000);
    old_selects = select_calls;
    assert(heart_idle(heart) == 4);
    assert(select_calls == old_selects + 1); // Lag still sleeps in the old code.
    assert(selected_timeout.tv_usec == 28000 && heart->last_time.tv_usec == 160000);

    heart->last_time.tv_sec = 10;
    heart->last_time.tv_usec = 900000;
    Clock(12, 30000, 12, 35000);
    assert(heart_idle(heart) == 29);
    assert(heart->last_time.tv_sec == 12 && heart->last_time.tv_usec == 60000);
    assert(selected_timeout.tv_usec == 25000);

    heart->last_time.tv_sec = heart->last_time.tv_usec = 0;
    Clock(40, 130000, 40, 140000);
    old_errors = errors;
    assert(heart_idle(heart) == 750); // 30 seconds * 25 pulses/second.
    assert(errors == old_errors + 1 && error_line == 83);
    assert(selected_timeout.tv_usec == 20000);

    heart->last_time.tv_sec = 20;
    heart->last_time.tv_usec = 0;
    Clock(19, 900000, 19, 950000);
    assert(heart_idle(heart) == 1);
    assert(heart->last_time.tv_sec == 19 && heart->last_time.tv_usec == 940000);
    assert(!selected_timeout.tv_sec && !selected_timeout.tv_usec);

    heart->last_time.tv_sec = 50;
    heart->last_time.tv_usec = 0;
    Clock(50, 40000, 50, 40000);
    assert(heart_idle(heart) == 2);
    assert(selected_timeout.tv_usec == 40000);
    assert(callback_calls == 0 && heart->pulse == 0); // Idle only returns a count.
    heart_delete(heart);
    heart_delete(NULL);
    return 0;
}
