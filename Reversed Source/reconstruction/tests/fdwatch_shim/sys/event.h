// Test-only target kevent declarations, never a production platform layer.
#ifndef MCNCC_TEST_SYS_EVENT_H
#define MCNCC_TEST_SYS_EVENT_H
#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec { int tv_sec; long tv_nsec; };
#endif
struct kevent {
    unsigned int ident;
    short filter;
    unsigned short flags;
    unsigned int fflags;
    int data;
    void* udata;
};
#define EVFILT_READ (-1)
#define EVFILT_WRITE (-2)
#define EV_ADD 0x0001
#define EV_DELETE 0x0002
#define EV_ONESHOT 0x0010
#define EV_ERROR 0x4000
#define EV_EOF 0x8000
int kqueue(void);
int kevent(int, const struct kevent*, int, struct kevent*, int, const struct timespec*);
#endif
