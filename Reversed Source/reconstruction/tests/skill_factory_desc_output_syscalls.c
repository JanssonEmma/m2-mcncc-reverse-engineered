#define sys_log skill_factory_base_sys_log
#define _sys_err skill_factory_base_sys_err
#include "desc_output_syscalls.c"
#undef sys_log
#undef _sys_err
extern void skill_factory_log(int,const char*,const char*,const char*);
extern void skill_factory_error(const char*,int,const char*,const char*);
void sys_log(int level,const char* format,...) {
    va_list args;char text[2048];const char* name=0;va_start(args,format);
    if(!strcmp(format,"Master %s")) {const char* expression=va_arg(args,const char*);snprintf(text,sizeof text,format,expression);assert(level==0);skill_factory_log(0,format,text,0);}
    else if(!strcmp(format,"#%-3d %-24s type %d flag %u affect %u point_poly: %s")) {
        unsigned long id=va_arg(args,unsigned long);name=va_arg(args,const char*);unsigned long type=va_arg(args,unsigned long);unsigned long flag=va_arg(args,unsigned long);unsigned long affect=va_arg(args,unsigned long);const char* expression=va_arg(args,const char*);
        snprintf(text,sizeof text,format,id,name,type,flag,affect,expression);assert(level==0);skill_factory_log(1,format,text,name);
    } else skill_factory_base_sys_log(level,format);
    va_end(args);
}
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;char text[2048];va_start(args,format);vsnprintf(text,sizeof text,format,args);va_end(args);
    if(!strcmp(function,"Initialize")){skill_factory_error(function,line,format,text);return;}
    fprintf(stderr,"Unexpected full skill factory dependency error %s:%d %s\n",function,line,text);abort();
}
