/* External logger/syscall observation only; full application graph is linked. */
#define sys_log banword_locale_unused_base_sys_log
#define _sys_err banword_locale_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void banword_locale_observe_name_diag(int,int,const char*,unsigned,unsigned,unsigned);
void sys_log(int level,const char* format,...) {
    if(!strcmp(format,"check_name_big5[%d][%s] %x %x %x")) {
        int index;const char* text;unsigned first,second,code;va_list args;
        va_start(args,format);index=va_arg(args,int);text=va_arg(args,const char*);
        first=va_arg(args,unsigned);second=va_arg(args,unsigned);code=va_arg(args,unsigned);
        va_end(args);banword_locale_observe_name_diag(level,index,text,first,second,code);return;
    }
    banword_locale_unused_base_sys_log(level,format);
}
void _sys_err(const char* function,int line,const char* format,...) {
    fprintf(stderr,"unexpected banword locale dependency error %s:%d %s\n",function,line,format);
    abort();
}
