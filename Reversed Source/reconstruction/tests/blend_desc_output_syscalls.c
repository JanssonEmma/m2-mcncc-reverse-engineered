#define random blend_unused_random
#define _sys_err blend_unused_sys_err
#include "desc_output_syscalls.c"
#undef random
#undef _sys_err
extern long blend_test_random(void);
extern void blend_test_init_error(const char*,int,const char*);
long random(void) { return blend_test_random(); }
void _sys_err(const char* function,int line,const char* format,...) {
    blend_test_init_error(function,line,format);
}
