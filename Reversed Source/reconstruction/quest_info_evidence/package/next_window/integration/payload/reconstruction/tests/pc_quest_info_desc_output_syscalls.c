#define sys_log pc_info_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void pc_info_log(int,const char*,int);
void sys_log(int level,const char* format,...){
    int kind=0,value=0;const char* text=0;va_list args;va_start(args,format);
    if(!strcmp(format,"QUEST BeginFlag %d")){kind=1;value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST Title %s")){kind=2;text=va_arg(args,const char*);}
    else if(!strcmp(format,"QUEST Clock Name %s")){kind=3;text=va_arg(args,const char*);}
    else if(!strcmp(format,"QUEST Clock Value %d")){kind=4;value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST Counter Name %s")){kind=5;text=va_arg(args,const char*);}
    else if(!strcmp(format,"QUEST Counter Value %d")){kind=6;value=va_arg(args,int);}
    else if(!strcmp(format,"QUEST Icon File %s")){kind=7;text=va_arg(args,const char*);}
    va_end(args);if(kind){assert(level==1);pc_info_log(kind,text,value);}else pc_info_unused_sys_log(level,format);
}
