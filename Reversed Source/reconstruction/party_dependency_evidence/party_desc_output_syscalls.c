#define sys_log party_unused_base_sys_log
#define gettimeofday party_base_gettimeofday
#include "desc_output_syscalls.c"
#undef gettimeofday
#undef sys_log
#include <stdarg.h>
int party_test_clock_reads;
int gettimeofday(struct timeval* value,struct timezone* zone) {
    ++party_test_clock_reads;
    return party_base_gettimeofday(value,zone);
}
extern void party_test_log(int,const char*,const char*);
void sys_log(int level,const char* format,...) {
    char text[1024];va_list args;
    va_start(args,format);vsnprintf(text,sizeof(text),format,args);va_end(args);
    party_test_log(level,format,text);
    /* Retain existing command-log counter behavior at the same external seam. */
    party_unused_base_sys_log(level,format);
}
