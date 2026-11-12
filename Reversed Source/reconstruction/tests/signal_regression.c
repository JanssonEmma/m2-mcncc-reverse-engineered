#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include "../../libthecore/include/signal.h"
volatile int tics, shutdowned;
static int sequence, timer_order, registrations, signals[16], waits, wait_mode;
static signal_test_handler handlers[16];
static struct itimerval recorded;
static int logged, logline, dumped, aborted;
static const char* logfunc;
static char logtext[200];
static jmp_buf jump;
signal_test_handler signal(int sig, signal_test_handler handler)
{
    assert(registrations < 16);
    signals[registrations]=sig; handlers[registrations++]=handler;
    ++sequence;
    return SIG_IGN;
}
int setitimer(int which, const struct itimerval* timer, struct itimerval* old)
{
    assert(which==1 && old==NULL);
    recorded=*timer; timer_order=++sequence;
    return -1; /* original ignores errors */
}
int waitpid(int pid, int* status, int options)
{
    assert(pid==-1 && status==NULL && options==1);
    ++waits;
    return wait_mode ? -1 : (waits<3 ? 101 : 0);
}
void _sys_err(const char* function, int line, const char* format, ...)
{
    va_list ap;
    ++logged; logfunc=function; logline=line;
    va_start(ap, format); vsnprintf(logtext,sizeof(logtext),format,ap); va_end(ap);
    if (!strcmp(function,"hupsig")) assert(shutdowned==1);
}
void core_dump_unix(const char* who, unsigned short line)
{
    assert(!strcmp(who,"signal.c") && line==44); ++dumped;
}
void signal_test_abort(void) { ++aborted; longjmp(jump,1); }
static void timer(int seconds)
{
    assert(recorded.it_interval.tv_sec==seconds && recorded.it_value.tv_sec==seconds);
    assert(recorded.it_interval.tv_usec==0 && recorded.it_value.tv_usec==0);
}
int main(void)
{
    int i;
    const int expected[]={26,1,20,2,15,13,14,30};
    signal_setup(); timer(30);
    assert(timer_order==1 && registrations==8 && sequence==9);
    for(i=0;i<8;++i) assert(signals[i]==expected[i]);
    assert(handlers[1]==handlers[3] && handlers[1]==handlers[4]);
    assert(handlers[5]==SIG_IGN && handlers[6]==SIG_IGN);
    tics=4; handlers[0](26); assert(tics==0 && logged==0);
    tics=-3; handlers[0](26); assert(tics==0 && logged==0);
    if (!setjmp(jump)) { handlers[0](26); assert(0); }
    assert(aborted==1 && logged==1 && logline==28 && !strcmp(logfunc,"checkpointing"));
    assert(!strcmp(logtext,"CHECKPOINT shutdown: tics did not updated."));
    handlers[1](1); assert(shutdowned==1 && logged==2 && logline==39);
    assert(!strcmp(logfunc,"hupsig"));
    assert(!strcmp(logtext,"SIGHUP, SIGINT, SIGTERM signal has been received. shutting down."));
    handlers[7](30); assert(dumped==1);
    handlers[2](20); assert(waits==3 && registrations==9);
    assert(signals[8]==20 && handlers[8]==handlers[2]);
    waits=0; wait_mode=1; handlers[2](20); assert(waits==1 && registrations==10);
    signal_timer_disable(); timer(0);
    signal_timer_enable(17); timer(17);
    signal_timer_enable(-1); timer(-1);
    signal_timer_enable(0); timer(0);
    puts("signal regression passed");
    return 0;
}
