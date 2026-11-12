// Explicit nativeOS/log observer only; application PC/GetFlag/vector remain real.
#define sys_log pc_reward_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void pc_reward_producer_log(int,const char*,unsigned long,int);
extern void pc_reward_get_log(const char*,int);
void sys_log(int level,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(format,"QUEST GiveItem %s %d %d")){
  const char* label=va_arg(args,const char*);unsigned long first=va_arg(args,unsigned long);int second=va_arg(args,int);assert(level==1);pc_reward_producer_log(1,label,first,second);
 }else if(!strcmp(format,"QUEST GiveExp %s %d")){
  const char* label=va_arg(args,const char*);unsigned long first=va_arg(args,unsigned long);assert(level==1);pc_reward_producer_log(2,label,first,0);
 }else if(!strcmp(format,"QUEST getting flag %s %d")){
  const char* key=va_arg(args,const char*);int value=va_arg(args,int);assert(level==0||level==1);pc_reward_get_log(key,value);
 }else pc_reward_unused_sys_log(level,format);
 va_end(args);
}
