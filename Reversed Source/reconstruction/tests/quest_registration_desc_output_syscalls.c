#define sys_log quest_registration_base_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void quest_registration_log(int,const char*,unsigned int);
void sys_log(int level,const char* format,...) {
    va_list args;va_start(args,format);
    if(!strcmp(format,"QUEST loading begin condition for %s")) {
        assert(level==0);quest_registration_log(1,va_arg(args,const char*),0);
    } else if(!strcmp(format,"QUEST: Register %4u %s")) {
        assert(level==0);unsigned int index=va_arg(args,unsigned int);const char* name=va_arg(args,const char*);quest_registration_log(2,name,index);
    } else quest_registration_base_log(level,format);
    va_end(args);
}
