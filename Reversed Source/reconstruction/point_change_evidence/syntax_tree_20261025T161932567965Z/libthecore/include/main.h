// Recovered libthecore main.c interface; unrelated declarations remain pending.
#ifndef MCNCC_LIBTHECORE_MAIN_H
#define MCNCC_LIBTHECORE_MAIN_H

#include "heart.h"

#ifdef __cplusplus
extern "C" {
#endif
extern LPHEART thecore_heart;
extern volatile int tics;
extern volatile int shutdowned;
enum ENUM_PROFILER { PF_IDLE, PF_HEARTBEAT, NUM_PF };
extern unsigned int thecore_profiler[NUM_PF];
int thecore_init(int fps, HEARTFUNC callback);
void thecore_shutdown(void);
int thecore_idle(void);
void thecore_destroy(void);
int thecore_pulse(void);
float thecore_pulse_per_second(void);
float thecore_time(void);
#ifdef __cplusplus
}
#endif

#endif
