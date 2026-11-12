// ExplicitnativeOS and diagnostic observer only, never a fakeLua/PC/CHAR implementation.
#define _sys_err quest_predicate_unused_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
extern void quest_predicate_diagnostic_error(const char*,const char*,const char*);
void _sys_err(const char* function,int line,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(function,"WriteRunningStateToSyserr")){const char* quest=va_arg(args,const char*);const char* state=va_arg(args,const char*);const char* event=va_arg(args,const char*);assert(line==1437&&!strcmp(format,"LUA_ERROR: quest %s.%s %s"));quest_predicate_diagnostic_error(quest,state,event);}
 else quest_predicate_unused_sys_err(function,line,format);
 va_end(args);
}
