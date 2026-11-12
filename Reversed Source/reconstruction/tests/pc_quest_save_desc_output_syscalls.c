// Test-only nativeOS boundary and Save diagnostic observers; all appmethods actual.
#define sys_log pc_save_unused_sys_log
#define _sys_err pc_save_unused_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err
extern void pc_save_log(int,const char*,const char*,long,int);
extern void pc_save_error(int,const char*);
void sys_log(int level,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(format,"QUEST Save Flag %s, %s %ld (%d)")){
  const char* name=va_arg(args,const char*);const char* state=va_arg(args,const char*);long value=va_arg(args,long);int index=va_arg(args,int);assert(level==1);pc_save_log(1,name,state,value,index);
 }else if(!strcmp(format,"QuestPC::Save %d")){int count=va_arg(args,int);assert(level==1);pc_save_log(2,0,0,0,count);}
 else pc_save_unused_sys_log(level,format);
 va_end(args);
}
void _sys_err(const char* function,int line,const char* format,...){
 if(!strcmp(function,"Save")){
  assert((line==537&&!strcmp(format,"quest::PC::Save : cannot find . in FlagMap"))||(line==549&&!strcmp(format,"quest::PC::Save : invalid quest data: %s"))||(line==557&&!strcmp(format,"quest::PC::Save : quest name overflow"))||(line==563&&!strcmp(format,"quest::PC::Save : quest state overflow")));pc_save_error(line,format);
 }else pc_save_unused_sys_err(function,line,format);
}
