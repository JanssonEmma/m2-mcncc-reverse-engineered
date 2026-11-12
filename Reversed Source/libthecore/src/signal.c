// Recovered FreeBSD signal module: all seven emitted functions.
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "../include/signal.h"
#include "../include/main.h"
#include "../include/log.h"
#include "../include/utils.h"
void reap(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
    signal(SIGCHLD, reap);
}
void checkpointing(int sig)
{
    (void)sig;
    if (tics != 0) tics = 0;
    else {
        _sys_err("checkpointing", 28, "CHECKPOINT shutdown: tics did not updated.");
        abort();
    }
}
void hupsig(int sig)
{
    (void)sig;
    shutdowned = 1;
    _sys_err("hupsig", 39, "SIGHUP, SIGINT, SIGTERM signal has been received. shutting down.");
}
void usrsig(int sig)
{
    (void)sig;
    core_dump_unix("signal.c", 44);
}
void signal_timer_disable(void)
{
    struct itimerval timer;
    timer.it_interval.tv_sec = timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = timer.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}
void signal_timer_enable(int sec)
{
    struct itimerval timer;
    timer.it_interval.tv_sec = timer.it_value.tv_sec = sec;
    timer.it_interval.tv_usec = timer.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}
void signal_setup(void)
{
    signal_timer_enable(30);
    signal(SIGVTALRM, checkpointing);
    signal(SIGHUP, hupsig);
    signal(SIGCHLD, reap);
    signal(SIGINT, hupsig);
    signal(SIGTERM, hupsig);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    signal(SIGUSR1, usrsig);
}
