#define _sys_err quest_indices_base_error
#include "desc_output_syscalls.c"
#undef _sys_err
extern void quest_indices_error(int,const char*,unsigned int);
void _sys_err(const char* function,int line,const char* format,...) {
    if(!strcmp(function,"GetQuestNameByIndex")&&!strcmp(format,"cannot find quest name by index %u")) {
        va_list args;va_start(args,format);unsigned int id=va_arg(args,unsigned int);va_end(args);
        quest_indices_error(line,format,id);
    } else quest_indices_base_error(function,line,format);
}
