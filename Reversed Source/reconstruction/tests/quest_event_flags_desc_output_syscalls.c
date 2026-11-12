// Explicit nativeOS/log observer; real CQM, ChatPacket, DESC and CLIENT_DESC remain intact.
#define sys_log quest_event_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void quest_event_db_log(unsigned,unsigned long,unsigned long,int);
void sys_log(int level,const char* format,...){
 if(!strcmp(format,"DB_PACKET: header %d handle %d size %d buffer_size %d")){
  unsigned header;unsigned long handle,size;int buffered;va_list args;va_start(args,format);header=va_arg(args,unsigned);handle=va_arg(args,unsigned long);size=va_arg(args,unsigned long);buffered=va_arg(args,int);va_end(args);assert(level==1);quest_event_db_log(header,handle,size,buffered);
 }else quest_event_unused_sys_log(level,format);
}
