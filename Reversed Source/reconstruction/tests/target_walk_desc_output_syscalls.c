/* External observation only; real descriptor/transport/clock helpers are retained. */
#define _sys_err target_walk_unused_base_sys_err
#define gettimeofday target_walk_base_gettimeofday
#include "desc_output_syscalls.c"
#undef gettimeofday
#undef _sys_err
int target_walk_clock_reads;
int gettimeofday(struct timeval* value,struct timezone* zone) {
    ++target_walk_clock_reads;
    return target_walk_base_gettimeofday(value,zone);
}
extern void target_walk_observe_missing_desc(const char*,int,const char*,const char*,void*);
void _sys_err(const char* function,int line,const char* format,...) {
    va_list args;
    const char* name;
    void* recipient;
    if(!function || !format || strcmp(function,"BroadcastTargetPacket") || line!=4801 ||
       strcmp(format,"%s %p does not have desc")) {
        fprintf(stderr,"unexpected target/walking dependency error %s:%d %s\n",
                function?function:"<null>",line,format?format:"<null>");
        abort();
    }
    va_start(args,format);
    name=va_arg(args,const char*);
    recipient=va_arg(args,void*);
    va_end(args);
    target_walk_observe_missing_desc(function,line,format,name,recipient);
    /* Return normally: the actual production helper must invoke abort next. */
}
