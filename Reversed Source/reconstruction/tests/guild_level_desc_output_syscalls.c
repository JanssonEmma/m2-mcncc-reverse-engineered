/* External-only observation. Real CLIENT_DESC and DESC implementations are linked. */
#define sys_log guild_level_unused_base_sys_log
#define _sys_err guild_level_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void guild_level_observe_db_log(int,const char*,unsigned,unsigned long,unsigned long,unsigned long);
void sys_log(int level,const char* format,...) {
    if(!strcmp(format,"DB_PACKET: header %d handle %d size %d buffer_size %d")) {
        va_list args;unsigned header;unsigned long handle,size,buffered;
        va_start(args,format);header=va_arg(args,unsigned);
        handle=va_arg(args,unsigned long);size=va_arg(args,unsigned long);
        buffered=va_arg(args,unsigned long);va_end(args);
        guild_level_observe_db_log(level,format,header,handle,size,buffered);
    }
    guild_level_unused_base_sys_log(level,format);
}
void _sys_err(const char* function,int line,const char* format,...) {
    fprintf(stderr,"unexpected guild level dependency diagnostic %s:%d %s\n",function,line,format);
    abort();
}
