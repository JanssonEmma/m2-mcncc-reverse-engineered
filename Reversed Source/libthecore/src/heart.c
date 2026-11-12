// Reconstructed from the three emitted heart.c functions in the target binary.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/heart.h"
#include "../include/utils.h"
#include "../include/log.h"

// 0x081bc030
LPHEART heart_new(int opt_usec, HEARTFUNC func)
{
    LPHEART heart;
    if (!func)
    {
        _sys_err("heart_new", 18, "no function defined");
        return NULL;
    }
    heart = (LPHEART)calloc(1, sizeof(HEART));
    if (!heart)
    {
        perror("malloc failure");
        abort();
    }
    heart->func = func;
    heart->opt_time.tv_sec = 0;
    heart->opt_time.tv_usec = opt_usec;
    heart->passes_per_sec = 1000000 / opt_usec;
    gettimeofday(&heart->last_time, NULL);
    gettimeofday(&heart->before_sleep, NULL);
    return heart;
}

// 0x081bc0e0
void heart_delete(LPHEART heart)
{
    free(heart);
}

// 0x081bc0f0: unlike the newer source, lag still takes the sleep path.
int heart_idle(LPHEART heart)
{
    struct timeval process_time, temp_time, now, timeout;
    uint32_t pulse_bits = 1;
    int32_t missed_pulse;
    gettimeofday(&heart->before_sleep, NULL);
    process_time = *timediff(&heart->before_sleep, &heart->last_time);
    if (process_time.tv_sec != 0 || process_time.tv_usec >= heart->opt_time.tv_usec)
    {
        pulse_bits += (uint32_t)process_time.tv_sec * (uint32_t)heart->passes_per_sec
                    + (uint32_t)(process_time.tv_usec / heart->opt_time.tv_usec);
        process_time.tv_usec %= heart->opt_time.tv_usec;
    }
    process_time.tv_sec = 0;
    memcpy(&missed_pulse, &pulse_bits, sizeof(missed_pulse));
    temp_time = *timediff(&heart->opt_time, &process_time);
    heart->last_time = *timeadd(&heart->before_sleep, &temp_time);
    gettimeofday(&now, NULL);
    timeout = *timediff(&heart->last_time, &now);
    thecore_sleep(&timeout);
    if (missed_pulse <= 0)
    {
        _sys_err("heart_idle", 77, "missed_pulse is not positive! (%d)", missed_pulse);
        missed_pulse = 1;
    }
    if (missed_pulse > 30 * heart->passes_per_sec)
    {
        _sys_err("heart_idle", 83, "losing %d seconds. (lag occured)",
                 missed_pulse / heart->passes_per_sec);
        missed_pulse = 30 * heart->passes_per_sec;
    }
    return missed_pulse;
}
