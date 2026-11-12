// Explicit nativeOS/logobserver only; genuine production owning lifecycle must be globallylinked.
#define sys_log quest_ownership_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void quest_ownership_clear_timer(int);
void sys_log(int level,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(format,"QUEST clear timer %d")){int count=va_arg(args,int);quest_ownership_clear_timer(count);}
 else quest_ownership_unused_sys_log(level,format);
 va_end(args);
}
