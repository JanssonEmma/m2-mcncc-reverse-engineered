#define _sys_err item_attr_unused_sys_err
#define sys_log item_attr_unused_sys_log
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void item_attr_clamp(int,int);
extern void item_attr_overflow(const char*,int,const char*);
void _sys_err(const char* function,int line,const char* format,...) {
    item_attr_overflow(function,line,format);
}
void sys_log(int level,const char* format,...) {
    assert(format);
    if(!strcmp(format,"Invalid Socket Count %d, set to maximum")) {
        va_list args;int value;va_start(args,format);value=va_arg(args,int);va_end(args);
        item_attr_clamp(level,value);
    } else item_attr_unused_sys_log(level,format);
}
