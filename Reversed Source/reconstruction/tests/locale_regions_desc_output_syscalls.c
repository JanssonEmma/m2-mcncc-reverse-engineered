/* TEST ONLY external logger observation; production functions stay intact. */
#define _sys_err locale_regions_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
static unsigned already_errors;
unsigned locale_regions_already_errors(void) {return already_errors;}
void _sys_err(const char* function,int line,const char* format,...) {
 if(!strcmp(function,"LocaleService_Init") && line==856 && !strcmp(format,"ALREADY exist service")) {++already_errors;return;}
 fprintf(stderr,"unexpected locale region dependency error %s:%d %s\n",function,line,format);abort();
}
