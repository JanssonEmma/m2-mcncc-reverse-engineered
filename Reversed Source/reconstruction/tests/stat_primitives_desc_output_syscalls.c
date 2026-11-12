/* Reuse the real integration syscall seam, specializing ONLY external logging. */
#define _sys_err stat_primitives_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err

extern void stat_primitives_observe_job_error(const char*,int,const char*,const char*,unsigned);
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;
    const char* name;
    unsigned race;
    if(!function || !format || strcmp(function,"GetJob") || line!=1569 ||
       strcmp(format,"CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE")) {
        fprintf(stderr,"unexpected stat primitive dependency diagnostic %s:%d %s\n",
                function?function:"<null>",line,format?format:"<null>");
        abort();
    }
    va_start(args,format);
    name=va_arg(args,const char*);
    race=va_arg(args,unsigned);
    va_end(args);
    stat_primitives_observe_job_error(function,line,format,name,race);
}
