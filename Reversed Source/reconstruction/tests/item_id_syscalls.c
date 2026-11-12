#define _sys_err item_id_base_sys_err
#define sys_log item_id_base_sys_log
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void item_id_observe(int,const char*,int,const char*,const char*);
void _sys_err(const char* function,int line,const char* format,...) {
    char text[512];va_list args;va_start(args,format);vsnprintf(text,sizeof(text),format,args);va_end(args);
    item_id_observe(1,function,line,format,text);
}
void sys_log(int level,const char* format,...) {
    if(!strcmp(format,"ITEM: ID start at %lu")) {
        char text[512];va_list args;assert(level==0);va_start(args,format);vsnprintf(text,sizeof(text),format,args);va_end(args);
        item_id_observe(0,"SetMaxItemID",0,format,text);
    } else item_id_base_sys_log(level,format);
}
