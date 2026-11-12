#define sys_log tail_unused_sys_log
#define random tail_unused_random
#define _sys_err tail_unused_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef random
#undef _sys_err
extern long tail_test_random(void);
long random(void) { return tail_test_random(); }
void _sys_err(const char* f,int line,const char* format,...) { (void)f;(void)line;(void)format;assert(!"unexpected tail test diagnostic"); }
extern void tail_test_rare_full(void);
void sys_log(int level,const char* format,...) { if(!strcmp(format,"RARE: over 2")){assert(level==0);tail_test_rare_full();}else tail_unused_sys_log(level,format); }
