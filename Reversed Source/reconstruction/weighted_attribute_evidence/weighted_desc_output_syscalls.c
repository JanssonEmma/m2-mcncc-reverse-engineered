#define random weighted_unused_random
#define _sys_err weighted_unused_sys_err
#include "desc_output_syscalls.c"
#undef random
#undef _sys_err
extern long weighted_test_random(void);
extern void weighted_test_error(const char*,int,const char*,int,int,const char*,int);
long random(void) { return weighted_test_random(); }
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;int a=0,b=0,source=0;const char* file=0;
    va_start(args,format);
    if(!strcmp(function,"number_ex")) {
        a=va_arg(args,int);b=va_arg(args,int);file=va_arg(args,char*);source=va_arg(args,int);
    } else if(!strcmp(function,"PutAttributeWithLevel")) {
        a=va_arg(args,int);b=(int)va_arg(args,unsigned int);
    } else assert(!strcmp(function,"AddAttr"));
    va_end(args);
    weighted_test_error(function,line,format,a,b,file,source);
}
