extern void quest_lua_char_stone_log(const char*, double);
// NativeOS and diagnostics observer only; Lua and CQM method implementations are real.
#define random quest_lua_unused_random
#define sys_log quest_lua_unused_sys_log
#define _sys_err quest_lua_unused_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef random

long random(void) { return 23; }
extern void quest_lua_callback_error(const char*, int, const char*);
#undef _sys_err
extern void quest_lua_reverse_error(const char*);
extern void quest_lua_error(int,const char*,const char*,int);
extern void quest_lua_debug(int,int,const char*,const char*);
void _sys_err(const char* function,int line,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(function,"GetQuestStateName")){const char* quest=va_arg(args,const char*);int index=va_arg(args,int);assert(line==939&&!strcmp(format,"QUEST wrong quest state file %s.%d"));quest_lua_error(line,quest,0,index);}
 else if(!strcmp(function,"GetQuestStateIndex")){const char* quest=va_arg(args,const char*);const char* state=va_arg(args,const char*);assert(line==957&&!strcmp(format,"QUEST wrong quest state file %s.%s"));quest_lua_error(line,quest,state,0);}
 else if (!strcmp(function, "BuildStateIndexToName")) {
     assert(line == 1740 && !strcmp(format, "%s"));
     quest_lua_reverse_error(va_arg(args, const char*));
 }
 else if (!strcmp(function, "_set_skin") || !strcmp(function, "_raw_script")) {
     assert(!strcmp(format, "%s"));
     quest_lua_callback_error(function, line, va_arg(args, const char*));
 }
 else quest_lua_unused_sys_err(function,line,format);
 va_end(args);
}
void sys_log(int level,const char* format,...){
 va_list args;va_start(args,format);
 if(!strcmp(format,"[QUESTMANAGER] GetQuestStateIndex x(%d) v(%d) %s %s")){int value=va_arg(args,int);int top=va_arg(args,int);const char* quest=va_arg(args,const char*);const char* state=va_arg(args,const char*);assert(!level);quest_lua_debug(value,top,quest,state);}
 else if (!strcmp(format, "dropevent.drop_char_stone = on") ||
          !strcmp(format, "dropevent.drop_char_stone = off")) {
     assert(level == 0);
     quest_lua_char_stone_log(format, 0);
 } else if (!strcmp(format, "dropevent.drop_char_stone: %d") ||
            !strcmp(format, "dropevent.drop_char_stone.level_range: %d")) {
     assert(level == 0);
     quest_lua_char_stone_log(format, va_arg(args, int));
 } else if (!strcmp(format, "dropevent.drop_char_stone.percent_lv01_10: %f") ||
            !strcmp(format, "dropevent.drop_char_stone.percent_lv11_30: %f") ||
            !strcmp(format, "dropevent.drop_char_stone.percent_lv31_MX: %f")) {
     assert(level == 0);
     quest_lua_char_stone_log(format, va_arg(args, double));
 }
 else quest_lua_unused_sys_log(level,format);
 va_end(args);
}
