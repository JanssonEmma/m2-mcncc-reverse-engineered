#define sys_log pc_unused_sys_log
#define _sys_err pc_unused_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err
extern void pc_test_clear_log(int);
extern void pc_test_free_log(unsigned long);
extern void pc_test_event_error(const char*,int,const char*);
void sys_log(int level,const char* format,...) {
    va_list args;assert(format);va_start(args,format);
    if(!strcmp(format,"QUEST clear timer %d")) {
        assert(level==0);pc_test_clear_log(va_arg(args,int));
    } else if(!strcmp(format,"FreeEventInfo: event free id %lu")) {
        assert(level==1);pc_test_free_log(va_arg(args,unsigned long));
    } else pc_unused_sys_log(level,format);
    va_end(args);
}
void _sys_err(const char* fn,int line,const char* format,...) {
    pc_test_event_error(fn,line,format);
}
