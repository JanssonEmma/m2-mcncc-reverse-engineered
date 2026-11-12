#ifndef MCNCC_LIBTHECORE_HEART_H
#define MCNCC_LIBTHECORE_HEART_H

#include "typedef.h"
#ifndef __WIN32__
#include <sys/time.h>
#endif

typedef struct heart HEART;
typedef struct heart* LPHEART;
typedef void (*HEARTFUNC)(LPHEART heart, int pulse);

// DWARF event.c DIE 0x383b: 36 bytes on the original 32-bit FreeBSD ABI.
struct heart
{
    HEARTFUNC func;
    struct timeval before_sleep;
    struct timeval opt_time;
    struct timeval last_time;
    int passes_per_sec;
    int pulse;
};

#ifdef __cplusplus
extern "C" {
#endif
LPHEART heart_new(int opt_usec, HEARTFUNC func);
void heart_delete(LPHEART heart);
int heart_idle(LPHEART heart);
#ifdef __cplusplus
}
#endif

#endif
