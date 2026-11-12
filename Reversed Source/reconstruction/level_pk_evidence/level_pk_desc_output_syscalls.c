#define sys_log level_pk_unused_base_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
#include <stdarg.h>
extern void level_pk_test_log(int,const char*,const char*);
void sys_log(int level,const char* format,...) {
    char text[256];va_list args;va_start(args,format);vsnprintf(text,sizeof(text),format,args);va_end(args);
    level_pk_test_log(level,format,text);
    level_pk_unused_base_sys_log(level,format);
}
