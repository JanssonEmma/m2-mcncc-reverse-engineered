// All nine emitted functions of the original libthecore main.c compilation unit.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../include/main.h"
#include "../include/utils.h"
#include "../include/log.h"
#include "../include/crypt.h"

extern void signal_setup(void);
extern void thecore_find_best_memcpy(void);

LPHEART thecore_heart = NULL;
volatile int shutdowned = 0;
volatile int tics = 0;
unsigned int thecore_profiler[NUM_PF];

// 0x081b9770
static int pid_init(void)
{
    FILE* stream = fopen("pid", "w");
    if (stream)
    {
        fprintf(stream, "%d", getpid());
        fclose(stream);
    }
    else
        printf("pid_init(): could not open file for writing. (filename: ./pid)");
    return stream != NULL;
}

// 0x081b97e0
static void pid_deinit(void)
{
    remove("./pid");
}

// 0x081b9800
int thecore_init(int fps, HEARTFUNC callback)
{
    time_t now = time(NULL);
    int pid = getpid();
    unsigned int uid = getuid();
    srandom((unsigned int)(now + pid + uid));
    srandomdev();
    signal_setup();
    if (!log_init() || !pid_init())
        return 0;
    thecore_find_best_memcpy();
    GOST_Init();
    thecore_heart = heart_new(1000000 / fps, callback);
    return 1;
}

// 0x081b9890
void thecore_shutdown(void)
{
    shutdowned = 1;
}

// 0x081b98a0: idle increments tics even after shutdown.
int thecore_idle(void)
{
    DWORD start;
    int pulses;
    const uint32_t tick_bits = (uint32_t)tics + UINT32_C(1);
    int32_t next_tick;
    memcpy(&next_tick, &tick_bits, sizeof(next_tick));
    tics = next_tick;
    if (shutdowned)
        return 0;
    start = get_dword_time();
    pulses = heart_idle(thecore_heart);
    thecore_profiler[PF_IDLE] += (unsigned int)(get_dword_time() - start);
    return pulses;
}

// 0x081b9910: the target calls only pid_deinit, not newer log_destroy().
void thecore_destroy(void)
{
    pid_deinit();
}

// 0x081b9920
int thecore_pulse(void)
{
    return thecore_heart->pulse;
}

// 0x081b9930
float thecore_pulse_per_second(void)
{
    return (float)thecore_heart->passes_per_sec;
}

// 0x081b9940
float thecore_time(void)
{
    return (float)((long double)thecore_heart->pulse / thecore_heart->passes_per_sec);
}
