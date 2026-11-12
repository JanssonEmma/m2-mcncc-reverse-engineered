// Real utils.c + signal.c, with deterministic clocks/process/signal fixtures.
#include <assert.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/main.h"
static char stream_tokens[2];
FILE* ut_stdout = (FILE*)&stream_tokens[0];
FILE* ut_stderr = (FILE*)&stream_tokens[1];
void* (*thecore_memcpy)(void*, const void*, size_t) = memcpy;
volatile int tics, shutdowned;
static char calls[32];
static int call_count, fork_result, flush_result, abort_expected;
static int log_line, logged_origin_line;
static const char* log_function;
static const char* log_format;
static const char* logged_origin;
static jmp_buf escape;
static struct timeval clock_value, selected_timeout;
static struct timeval* selected_pointer;
static int select_count;
static void record(char op) { assert(call_count < 31); calls[call_count++] = op; calls[call_count] = 0; }
int ut_fork(void) { record('F'); return fork_result; }
int ut_fflush(FILE* stream) {
    assert(stream == ut_stdout || stream == ut_stderr);
    record(stream == ut_stdout ? 'O' : 'E');
    return flush_result;
}
void ut_abort(void) {
    record('A'); assert(abort_expected); longjmp(escape, 1);
}
void _sys_err(const char* function, int line, const char* format, ...) {
    va_list args;
    record('L'); log_function = function; log_line = line; log_format = format;
    va_start(args, format);
    if (!strcmp(function, "core_dump_unix")) {
        logged_origin = va_arg(args, const char*);
        logged_origin_line = va_arg(args, int);
    }
    va_end(args);
}
int gettimeofday(struct timeval* value, struct timezone* zone) {
    assert(!zone); *value = clock_value; return 0;
}
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout) {
    assert(!nfds && !readfds && !writefds && !exceptfds);
    ++select_count; selected_pointer = timeout; selected_timeout = *timeout;
    timeout->tv_sec = timeout->tv_usec = 0;
    return 0;
}
long random(void) { assert(!"random is outside this fixture"); return 0; }
int setitimer(int which, const struct itimerval* timer, struct itimerval* old) {
    (void)which; (void)timer; (void)old; assert(!"unexpected timer setup"); return -1;
}
int waitpid(int pid, int* status, int options) {
    (void)pid; (void)status; (void)options; assert(!"unexpected waitpid"); return -1;
}
void (*signal(int sig, void (*handler)(int)))(int) {
    (void)sig; (void)handler; assert(!"unexpected signal registration"); return SIG_IGN;
}
extern void usrsig(int sig);
static void reset(void) { call_count = 0; calls[0] = 0; abort_expected = 0; }
static void check_dump_log(const char* who, int line) {
    assert(!strcmp(log_function, "core_dump_unix") && log_line == 371);
    assert(!strcmp(log_format, "*** Dumping Core %s:%d ***"));
    assert(!strcmp(logged_origin, who) && logged_origin_line == line);
}
int main(void) {
    struct timeval* first;
    unsigned long long stamp;
    assert(sizeof(DWORD) == 4 && sizeof(long) == 4);
    reset(); fork_result = 123;
    core_dump_unix("caller.cpp", 65535);
    assert(!strcmp(calls, "LOEF")); check_dump_log("caller.cpp", 65535);
    reset(); fork_result = -1; flush_result = EOF;
    core_dump_unix("failure.c", 0);
    assert(!strcmp(calls, "LOEF")); check_dump_log("failure.c", 0);
    reset(); fork_result = 0; abort_expected = 1;
    if (!setjmp(escape)) { core_dump_unix("child.c", 7); assert(!"child returned"); }
    assert(!strcmp(calls, "LOEFA")); check_dump_log("child.c", 7);
    reset(); fork_result = 456; flush_result = 0;
    usrsig(SIGUSR1);
    assert(!strcmp(calls, "LOEF")); check_dump_log("signal.c", 44);

    thecore_msleep(0); assert(!selected_timeout.tv_sec && !selected_timeout.tv_usec);
    first = selected_pointer;
    thecore_msleep(1001);
    assert(selected_pointer == first);
    assert(selected_timeout.tv_sec == 1 && selected_timeout.tv_usec == 1001000);
    thecore_msleep(4294968UL);
    assert(selected_timeout.tv_sec == 4294 && selected_timeout.tv_usec == 704);
    thecore_msleep(4294967UL);
    assert(selected_timeout.tv_sec == 4294 && selected_timeout.tv_usec == -296);
    thecore_msleep(0xffffffffUL);
    assert(selected_timeout.tv_sec == 4294967 && selected_timeout.tv_usec == -1000);
    assert(select_count == 5);

    clock_value.tv_sec = 1057699978L; clock_value.tv_usec = 125000;
    assert(get_float_time() == 0.125f);
    clock_value.tv_sec -= 1; clock_value.tv_usec = 750000;
    assert(get_float_time() == -0.25f);
    clock_value.tv_sec = LONG_MIN; clock_value.tv_usec = 0;
    assert(get_float_time() == (float)(long)((DWORD)LONG_MIN - 1057699978UL));
    clock_value.tv_sec = LONG_MAX; clock_value.tv_usec = 500000;
    assert(get_float_time() == (float)((double)(LONG_MAX - 1057699978L) + 0.5));
    stamp = rdtsc();
    (void)stamp; // Bare hardware counter has no portable monotonicity guarantee.
    puts("utility process/time regression passed");
    return 0;
}
