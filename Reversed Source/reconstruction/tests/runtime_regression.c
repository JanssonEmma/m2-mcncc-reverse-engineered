// main.c with external dependencies and all file/process/time calls controlled.
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "runtime_hooks.h"
#include "../../libthecore/include/main.h"
#include "../../libthecore/include/typedef.h"

static char trace[128];
static unsigned int trace_length;
static int log_success = 1, file_success = 1, null_heart;
static unsigned char file_token;
static HEART fixture_heart = {0, {0, 0}, {0, 0}, {0, 0}, 25, 125};
static HEARTFUNC expected_callback;
static int fixture_pulses = 3;
static DWORD milliseconds[2];
static unsigned int millisecond_index;

static void Record(char code)
{
    assert(trace_length + 1 < sizeof(trace));
    trace[trace_length++] = code;
    trace[trace_length] = 0;
}
static void Clear(void) { trace_length = 0; trace[0] = 0; }

time_t mc_test_time(time_t* value)
{
    assert(value == NULL);
    Record('T');
    return 1000;
}
int mc_test_getpid(void) { Record('P'); return 4321; }
unsigned int mc_test_getuid(void) { Record('U'); return 99; }
void mc_test_srandom(unsigned int seed) { Record('R'); assert(seed == 5420); }
void mc_test_srandomdev(void) { Record('D'); }
void signal_setup(void) { Record('S'); }
int log_init(void) { Record('L'); return log_success; }
void thecore_find_best_memcpy(void) { Record('M'); }
void GOST_Init(void) { Record('G'); }

FILE* mc_test_fopen(const char* path, const char* mode)
{
    assert(!strcmp(path, "pid") && !strcmp(mode, "w"));
    Record('O');
    return file_success ? (FILE*)&file_token : NULL;
}
int mc_test_fprintf(FILE* file, const char* format, ...)
{
    va_list args;
    assert(file == (FILE*)&file_token && !strcmp(format, "%d"));
    va_start(args, format);
    assert(va_arg(args, int) == 4321);
    va_end(args);
    Record('F');
    return 4;
}
int mc_test_fclose(FILE* file)
{
    assert(file == (FILE*)&file_token);
    Record('C');
    return 0;
}
int mc_test_printf(const char* format, ...)
{
    assert(!strcmp(format, "pid_init(): could not open file for writing. (filename: ./pid)"));
    Record('Q');
    return 0;
}
int mc_test_remove(const char* path)
{
    assert(!strcmp(path, "./pid"));
    Record('X');
    return 0;
}
LPHEART heart_new(int period, HEARTFUNC callback)
{
    assert(period == 40000 && callback == expected_callback);
    Record('H');
    return null_heart ? NULL : &fixture_heart;
}
int heart_idle(LPHEART heart)
{
    assert(heart == &fixture_heart);
    Record('I');
    return fixture_pulses;
}
DWORD get_dword_time(void)
{
    assert(millisecond_index < 2);
    Record('A');
    return milliseconds[millisecond_index++];
}
static void Callback(LPHEART heart, int pulse) { (void)heart; (void)pulse; }

int main(void)
{
    expected_callback = Callback;
    assert(!shutdowned && !tics && !thecore_heart);
    assert(thecore_init(25, Callback) == 1);
    assert(!strcmp(trace, "TPURDSLOPFCMGH"));
    assert(thecore_heart == &fixture_heart);
    assert(thecore_pulse() == 125);
    assert(thecore_pulse_per_second() == 25.0f);
    assert(thecore_time() == 5.0f);

    Clear();
    log_success = 0;
    assert(thecore_init(25, Callback) == 0 && !strcmp(trace, "TPURDSL"));
    Clear();
    log_success = 1;
    file_success = 0;
    assert(thecore_init(25, Callback) == 0 && !strcmp(trace, "TPURDSLOQ"));
    Clear();
    file_success = 1;
    null_heart = 1;
    expected_callback = NULL;
    shutdowned = 1;
    assert(thecore_init(25, NULL) == 1); // No post-heart_new success check.
    assert(thecore_heart == NULL && shutdowned == 1); // Init does not reset shutdown.

    thecore_heart = &fixture_heart;
    shutdowned = 0;
    Clear();
    milliseconds[0] = 100;
    milliseconds[1] = 125;
    thecore_profiler[PF_HEARTBEAT] = 77;
    assert(thecore_idle() == 3 && !strcmp(trace, "AIA"));
    assert(tics == 1 && thecore_profiler[PF_IDLE] == 25);
    assert(thecore_profiler[PF_HEARTBEAT] == 77 && fixture_heart.pulse == 125);

    Clear();
    millisecond_index = 0;
    milliseconds[0] = 0xfffffff0UL;
    milliseconds[1] = 0x10;
    fixture_pulses = 0;
    assert(thecore_idle() == 0 && !strcmp(trace, "AIA"));
    assert(tics == 2 && thecore_profiler[PF_IDLE] == 57); // 32-bit wrap delta.

    Clear();
    thecore_shutdown();
    assert(thecore_idle() == 0 && trace_length == 0);
    assert(shutdowned == 1 && tics == 3);
    thecore_destroy();
    assert(!strcmp(trace, "X"));
    return 0;
}
