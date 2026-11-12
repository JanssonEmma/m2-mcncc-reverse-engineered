// Test-only deterministic syscall interface, matching the target timeval fields.
// Used only via an explicit test include path; never included in server builds.
#ifndef MCNCC_TEST_SYS_TIME_H
#define MCNCC_TEST_SYS_TIME_H
#if !defined(_TIMEVAL_DEFINED) && !defined(__timeval_defined)
#define _TIMEVAL_DEFINED
#define __timeval_defined 1
struct timeval { long tv_sec; long tv_usec; };
#endif
#ifndef _TIMEZONE_DEFINED
#define _TIMEZONE_DEFINED
struct timezone { int tz_minuteswest; int tz_dsttime; };
#endif
#ifdef __cplusplus
extern "C" {
#endif
int gettimeofday(struct timeval* value, struct timezone* zone);
struct itimerval { struct timeval it_interval; struct timeval it_value; };
#define ITIMER_VIRTUAL 1
int setitimer(int which, const struct itimerval* timer, struct itimerval* old);
#ifdef __cplusplus
}
#endif
#endif
