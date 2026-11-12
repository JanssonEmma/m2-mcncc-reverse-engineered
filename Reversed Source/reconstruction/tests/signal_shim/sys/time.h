#ifndef SIGNAL_TEST_TIME_H
#define SIGNAL_TEST_TIME_H
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
#define __timeval_defined 1
struct timeval { long tv_sec; long tv_usec; };
#endif
struct itimerval { struct timeval it_interval; struct timeval it_value; };
#define ITIMER_VIRTUAL 1
int setitimer(int which, const struct itimerval* timer, struct itimerval* old);
#endif
