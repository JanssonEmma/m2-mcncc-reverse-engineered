// Explicit nativeOS and diagnostic observers; whole CQM::QuestError remains actualapplication.
#define sys_log yield_unused_sys_log
#define _sys_err yield_unused_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err
extern void yield_error(const char*,int,const char*);
extern void yield_debug(const char*);
void _sys_err(const char* function,int line,const char* format,...){
 if(!strcmp(format,"%s")&&(!strcmp(function,"GotoSelectState")||!strcmp(function,"Direct"))){
  char* message;va_list args;va_start(args,format);message=va_arg(args,char*);va_end(args);yield_error(function,line,message);
 }else yield_unused_sys_err(function,line,format);
}
void sys_log(int level,const char* format,...){
 if(!strcmp(format,"%s")){const char* text;va_list args;va_start(args,format);text=va_arg(args,const char*);va_end(args);assert(level==0);yield_debug(text);}
 else yield_unused_sys_log(level,format);
}
