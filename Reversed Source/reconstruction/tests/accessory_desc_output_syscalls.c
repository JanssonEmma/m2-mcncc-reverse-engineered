#define _sys_err accessory_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
/* New fixture accepts only the actual locale_find disabled-locale diagnostic.
   All other error paths fail at the test boundary. */
extern void accessory_test_locale_error(const char*,int,const char*,const char*);
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;const char* key;
    assert(!strcmp(function,"locale_find") && line==51);
    va_start(args,format);key=va_arg(args,const char*);va_end(args);
    accessory_test_locale_error(function,line,format,key);
}
