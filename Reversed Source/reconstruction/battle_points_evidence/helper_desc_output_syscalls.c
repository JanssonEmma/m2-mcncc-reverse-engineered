#define _sys_err helper_unused_base_sys_err
#define sys_log helper_unused_base_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err
#include <stdarg.h>
extern void helper_test_error(const char*,int,const char*,const char*,unsigned long);
extern void helper_test_log(int,const char*,const char*,unsigned long);
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;const char* name;unsigned long value;
    assert((!strcmp(function,"GetSkillLevel")&&line==2614&&!strcmp(format,"%s skill vnum overflow %lu"))||(!strcmp(function,"GetJob")&&line==1569&&!strcmp(format,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE")));
    va_start(args,format);name=va_arg(args,const char*);
    value=!strcmp(function,"GetSkillLevel")?va_arg(args,unsigned long):(unsigned long)va_arg(args,unsigned int);va_end(args);
    helper_test_error(function,line,format,name,value);
}
void sys_log(int level,const char* format,...) {
    if(!strcmp(format,"%s skill vnum overflow %lu")) {
        va_list args;const char* name;unsigned long value;va_start(args,format);name=va_arg(args,const char*);value=va_arg(args,unsigned long);va_end(args);
        helper_test_log(level,format,name,value);return;
    }
    /* Unrelated normal logs retain the existing boundary; they do not consume
       varargs in the original descriptor observer. */
    helper_unused_base_sys_log(level,format);
}
