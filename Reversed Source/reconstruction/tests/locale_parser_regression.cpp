// Standalone C++03 integration regression. Real locale_add/find/service, external
// stdio/allocator instrumentation only; no replacement parser or map helpers.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>
#include <map>
#include <string>
#include <cstdarg>
static void *arrays[4096];
static FILE* retained_files[4096];
static int retained_count;
static int array_count, array_frees, opens, closes, seeks, tells, reads, exit_calls;
void *operator new[](std::size_t n) throw(std::bad_alloc)
{
    void *p=std::malloc(n ? n : 1); if(!p) throw std::bad_alloc();
    assert(array_count<4096); arrays[array_count++]=p; return p;
}
static bool forget(void *p) {
    for(int i=0;i<array_count;++i) if(arrays[i]==p) {
        arrays[i]=arrays[--array_count]; return true;
    }
    return false;
}
void operator delete[](void *p) throw() { forget(p); std::free(p); }
static void tracked_free(void *p) { if(forget(p)) ++array_frees; std::free(p); }
static FILE *tracked_fopen(const char *p,const char *m) {
    FILE *f=std::fopen(p,m); if(f) { ++opens; assert(retained_count<4096); retained_files[retained_count++]=f; } return f;
}
static int tracked_fclose(FILE *f) { ++closes; return std::fclose(f); }
static int tracked_fseek(FILE *f,long o,int w) { ++seeks; return std::fseek(f,o,w); }
static long tracked_ftell(FILE *f) { ++tells; return std::ftell(f); }
static std::size_t tracked_fread(void *p,std::size_t s,std::size_t n,FILE *f) {
    ++reads; return std::fread(p,s,n,f);
}
static void tracked_exit(int n) { ++exit_calls; std::fprintf(stderr,"tracked_exit(%d)\n",n); std::exit(n); }
// Permit future production spelling std::free/fopen without changing headers.
namespace std {
    using ::tracked_free; using ::tracked_fopen; using ::tracked_fclose;
    using ::tracked_fseek; using ::tracked_ftell; using ::tracked_fread;
    using ::tracked_exit;
}
#include "../../game/src/config.h"
extern "C" void sys_log(int,const char*,...) { assert(!"unused locale diagnostic path"); }
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    assert(std::strcmp(function,"locale_find")==0 && line==61);
    assert(std::strcmp(format,"LOCALE_ERROR: \"%s\";")==0);
    va_list args; va_start(args,format); assert(va_arg(args,const char*)); va_end(args);
}
// All headers loaded before interception; includes resolve genuine production.
#include "../../game/src/locale.hpp"
#include "../../game/src/locale_service.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/log.h"
#define free tracked_free
#define fopen tracked_fopen
#define fclose tracked_fclose
#define fseek tracked_fseek
#define ftell tracked_ftell
#define fread tracked_fread
#define exit tracked_exit
#include "../../game/src/locale.cpp"
#undef free
#undef fopen
#undef fclose
#undef fseek
#undef ftell
#undef fread
#undef exit
static void run(const char *text) {
    FILE *f=std::fopen("locale_parser_input.tmp","wb"); assert(f);
    std::fwrite(text,std::strlen(text),1,f); std::fclose(f);
    int o=opens,c=closes,s=seeks,t=tells,r=reads;
    locale_init("locale_parser_input.tmp");
    assert(opens==o+1 && closes==c && seeks==s+2 && tells==t+1 && reads==r+1);
}
static void value(const char *key,const char *expected) {
    if(std::strcmp(locale_find(key,1),expected)) std::fprintf(stderr,"value mismatch key=[%s] actual=[%s] expected=[%s]\n",key,locale_find(key,1),expected);
    assert(std::strcmp(locale_find(key,1),expected)==0);
}
static void absent(const char *key) {
    assert(std::strncmp(locale_find(key,1),"@0949",5)==0);
}
int main(int argc,char **argv) {
    g_eLocalType=LC_ENGLISH;
    if(argc>1) { locale_init("locale_parser_missing_test_file.tmp"); return 99; }
    int a=array_count,f=array_frees;
    run("// comment\n\"normal\";\"translated\";\r\n\"second\"; \r\n\"two\";");
    value("normal","translated"); value("second","two");
    assert(array_count==a+2 && array_frees==f+4); // map retains values, converter temps freed by free
    run(" \"indented\";\"ignored\";\n\"afterindent\";\"yes\";");
    absent("indented"); value("afterindent","yes");
    run("\"semi;colon\";\"value;inside\"; \"same-line\";\"works\";");
    value("semi;colon","value;inside"); value("same-line","works");
    run("\"escape\";\"a\\\"b\";\n\"newline\";\"a\\nb\";");
    value("escape","a\\\"b"); value("newline","a\nb");
    // Byte c is at offset 5, beyond nominal five-byte span. The escape advances
    // tmp twice but i only once, so c nevertheless becomes output.
    char *p=locale_convert("\"a\\nbc\";",5);
    assert(p && !std::strcmp(p,"a\nbc")); tracked_free(p);
    a=array_count; f=array_frees;
    run("\"normal\";\"replacement\";"); value("normal","replacement");
    assert(array_count==a+1 && array_frees==f+2); // overwritten old value deliberately leaks
    a=array_count;
    run("\"retained\";\"first\";\n\"partial\";\t\"bad\";\n\"unreached\";\"bad\";");
    value("retained","first"); absent("partial"); absent("unreached");
    assert(array_count==a+2); // one retained map value + one leaked partial key
    a=array_count;
    run("\"missingsecond\";\"unterminated\n\"unreached2\";\"bad\";");
    absent("missingsecond"); absent("unreached2"); assert(array_count==a+1);
    a=array_count;
    run("\"emptysecond\";\"\";\n\"unreached3\";\"bad\";");
    absent("emptysecond"); absent("unreached3"); assert(array_count==a+1);
    run("\"\";\"orphan-value\";\n\"unreached4\";\"bad\";");
    absent("unreached4");
    run("");
    std::remove("locale_parser_missing_test_file.tmp");
    std::string command="\""; command+=argv[0]; command+="\" --missing";
    assert(std::system(command.c_str())==1); // Windows process exit code
    std::printf("integration passed: open FILE leaks=%d, new[]->free calls=%d, live arrays=%d\n",opens-closes,array_frees,array_count);
    for(int i=0;i<retained_count;++i) std::fclose(retained_files[i]);
    while(array_count) tracked_free(arrays[0]);
    std::remove("locale_parser_input.tmp");
    return 0;
}
