// Explicit nativeOS/diagnostic observer only; actualLua/PC/script/net application methods retained.
#define sys_log quest_script_unused_sys_log
#include "desc_output_syscalls.c"
#undef sys_log
extern void quest_script_debug(const char*,int);
void sys_log(int level,const char* format,...){
 if(!strcmp(format,"m_strScript %s size %d")){
  const char* text;int bytes;va_list args;va_start(args,format);text=va_arg(args,const char*);bytes=va_arg(args,int);va_end(args);assert(level==0);quest_script_debug(text,bytes);
 }else quest_script_unused_sys_log(level,format);
}
