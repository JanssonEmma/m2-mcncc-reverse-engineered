/* TEST ONLY external syscall/logger observer; all locale application methods real. */
#define sys_log locale_foundation_unused_base_sys_log
#define _sys_err locale_foundation_unused_base_sys_err
#include "desc_output_syscalls.c"
#undef _sys_err
#undef sys_log
extern void locale_foundation_observe_big5(int,unsigned,unsigned);
void sys_log(int level,const char* format,...) {
    if(!strcmp(format,"twobyte_big5 %x %x")) {
        unsigned first,second;va_list args;
        va_start(args,format);first=va_arg(args,unsigned);second=va_arg(args,unsigned);va_end(args);
        locale_foundation_observe_big5(level,first,second);
        return;
    }
    locale_foundation_unused_base_sys_log(level,format);
}
void _sys_err(const char* function,int line,const char* format,...) {
    fprintf(stderr,"unexpected locale foundation dependency error %s:%d %s\n",function,line,format);
    abort();
}
static int locale_io_active,locale_io_step,locale_io_opens,locale_io_closes;
static const char* locale_io_filename;
static FILE* locale_io_file;
static long locale_io_length;
void locale_test_stdio_begin(const char* filename) {
    assert(!locale_io_active && !locale_io_file);
    locale_io_active=1;locale_io_step=locale_io_opens=locale_io_closes=0;
    locale_io_filename=filename;locale_io_length=-1;
}
FILE* locale_test_fopen(const char* filename,const char* mode) {
    assert(locale_io_active && locale_io_step==0 && !locale_io_file);
    assert(!strcmp(filename,locale_io_filename) && !strcmp(mode,"rb"));
    locale_io_file=fopen(filename,mode);assert(locale_io_file);
    ++locale_io_step;++locale_io_opens;return locale_io_file;
}
int locale_test_fseek(FILE* file,long offset,int origin) {
    assert(locale_io_active && file==locale_io_file && offset==0);
    assert((locale_io_step==1 && origin==SEEK_END) || (locale_io_step==3 && origin==SEEK_SET));
    ++locale_io_step;return fseek(file,offset,origin);
}
long locale_test_ftell(FILE* file) {
    assert(locale_io_active && file==locale_io_file && locale_io_step==2);
    ++locale_io_step;locale_io_length=ftell(file);assert(locale_io_length>0);
    return locale_io_length;
}
size_t locale_test_fread(void* data,size_t size,size_t count,FILE* file) {
    assert(locale_io_active && file==locale_io_file && locale_io_step==4);
    assert(size==(size_t)locale_io_length && count==1);
    ++locale_io_step;return fread(data,size,count,file);
}
int locale_test_fclose(FILE* file) {
    assert(locale_io_active && file==locale_io_file);++locale_io_closes;
    locale_io_file=0;return fclose(file);
}
void locale_test_stdio_finish(void) {
    assert(locale_io_active && locale_io_step==5 && locale_io_opens==1 && locale_io_closes==0);
    assert(locale_io_file);
    /* Original parser leaked this stream. External test cleanup AFTER verifying that fact. */
    assert(!fclose(locale_io_file));locale_io_file=0;locale_io_active=0;
}
int locale_test_stdio_active(void) { return locale_io_active; }
