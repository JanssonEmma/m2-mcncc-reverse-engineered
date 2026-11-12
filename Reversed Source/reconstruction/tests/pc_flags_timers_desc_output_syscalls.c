#define sys_log pc_storage_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void pc_storage_log(int,int,const char*,int,void*,unsigned long);
void sys_log(int level,const char* format,...) {
    int kind=0,value=0;const char* text=0;void* pointer=0;unsigned long id=0;
    va_list args;va_start(args,format);
    if(!strcmp(format,"QUEST Setting flag %s %d")) {kind=1;text=va_arg(args,const char*);value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST getting flag %s %d")) {kind=2;text=va_arg(args,const char*);value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST remove timer %p")) {kind=3;pointer=va_arg(args,void*);}
    else if(!strcmp(format,"QUEST timer map size %d by RemoveTimer")) {kind=4;value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST timer map size %d by RemoveTimerNotCancel")) {kind=5;value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST add timer %p %d")) {kind=6;pointer=va_arg(args,void*);value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST remove with no cancel %p")) {kind=7;pointer=va_arg(args,void*);}
    else if(!strcmp(format,"FreeEventInfo: event free id %lu")) {kind=8;id=va_arg(args,unsigned long);}
    va_end(args);
    if(kind)pc_storage_log(kind,level,text,value,pointer,id);
    else pc_storage_unused_sys_log(level,format);
}
