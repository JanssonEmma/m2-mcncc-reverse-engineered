#define sys_log safebox_unused_base_sys_log
#define _sys_err safebox_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void safebox_observe_log(int);
extern void safebox_observe_error(int,unsigned long);
void sys_log(int level,const char* format,...){
    if(!strcmp(format,"ITEM_SAVE %s:%d in %s window %d")){safebox_observe_log(1);return;}
    if(!strcmp(format,"SAFEBOX: ADD %s %s count %d")){safebox_observe_log(2);return;}
    if(!strcmp(format,"SAFEBOX: SAVE %s")){safebox_observe_log(3);return;}
    safebox_unused_base_sys_log(level,format);
}
void _sys_err(const char* function,int line,const char* format,...){
    if(!strcmp(function,"Add") && line==58 && !strcmp(format,"SAFEBOX: item on wrong position at %d (size of grid = %d)")){
        va_list args;unsigned long pos;va_start(args,format);pos=va_arg(args,unsigned long);va_end(args);safebox_observe_error(line,pos);return;
    }
    if(!strcmp(function,"GetItem") && line==159 && !strcmp(format,"CHARACTER::GetItem: invalid item cell %d")){
        va_list args;unsigned cell;va_start(args,format);cell=va_arg(args,unsigned);va_end(args);safebox_observe_error(line,cell);return;
    }
    safebox_unused_base_sys_err(function,line,format);
}
