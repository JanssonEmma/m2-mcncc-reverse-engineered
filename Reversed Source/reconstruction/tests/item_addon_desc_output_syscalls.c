#define random item_addon_unused_random
#define _sys_err item_addon_unused_sys_err
#include "desc_output_syscalls.c"
#undef random
#undef _sys_err
extern long item_addon_random(void);
extern void item_attr_overflow(const char*,int,const char*);
long random(void) { return item_addon_random(); }
void _sys_err(const char* function,int line,const char* format,...) {
    item_attr_overflow(function,line,format);
}
