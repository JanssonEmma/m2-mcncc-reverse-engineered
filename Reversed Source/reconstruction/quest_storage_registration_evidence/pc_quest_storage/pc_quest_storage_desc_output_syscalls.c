#define sys_log pc_quest_storage_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void pc_quest_storage_log(int,int,const char*,int,unsigned long,int,int);
void sys_log(int level,const char* format,...){
    va_list args;va_start(args,format);
    if(!strcmp(format,"QUEST Setting flag %s %d")){
        const char* text=va_arg(args,const char*);int value=va_arg(args,int);pc_quest_storage_log(1,level,text,value,0,0,0);
    }else if(!strcmp(format,"QUEST getting flag %s %d")){
        const char* text=va_arg(args,const char*);int value=va_arg(args,int);pc_quest_storage_log(2,level,text,value,0,0,0);
    }else if(!strcmp(format,"QUEST reserve Quest State Change quest %s[%lu] from %d to %d")){
        const char* text=va_arg(args,const char*);unsigned long index=va_arg(args,unsigned long);int previous=va_arg(args,int);int next=va_arg(args,int);pc_quest_storage_log(3,level,text,0,index,previous,next);
    }else if(!strcmp(format,"QUEST clear timer %d")){
        int count=va_arg(args,int);pc_quest_storage_log(4,level,0,count,0,0,0);
    }else pc_quest_storage_unused_sys_log(level,format);
    va_end(args);
}
