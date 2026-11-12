#include "dev_log_hooks.h"
#include "../../game/src/dev_log.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <climits>
int test_server=0;
static std::string trace,record;
static int open_result=3,time_result,close_result,written_fd,closed_fd;
static bool local_failure,write_override;
static ssize_t write_result;
static long micros=123456;
static timeval* sampled;
extern "C" int dev_test_open(const char* file,int flags,...) {
    assert(trace.empty() && std::string(file)=="DEV_LOG.log" && flags==0x209);
    va_list args; va_start(args,flags); int mode=va_arg(args,int); va_end(args); assert(mode==0666);
    trace+='O'; return open_result;
}
extern "C" int dev_test_gettimeofday(timeval* value,void* zone) {
    assert(trace=="O" && value && !zone); trace+='T'; sampled=value;
    value->tv_sec=123456789; value->tv_usec=micros; return time_result;
}
extern "C" tm* dev_test_localtime_r(const time_t* epoch,tm* value) {
    assert(trace=="OT" && sampled && epoch==reinterpret_cast<const time_t*>(&sampled->tv_sec));
    assert(sampled->tv_sec==123456789); trace+='L'; std::memset(value,0,sizeof(*value));
    value->tm_mon=9; value->tm_mday=7; value->tm_hour=8; value->tm_min=9; value->tm_sec=10; value->tm_year=126;
    return local_failure?0:value;
}
extern "C" ssize_t dev_test_write(int fd,const void* data,size_t size) {
    assert(trace=="OTL" && fd==open_result && data && size);
    trace+='W'; written_fd=fd; const char* bytes=static_cast<const char*>(data);
    assert(bytes[size-1]=='\n' && bytes[size]==0); record.assign(bytes,size);
    return write_override?write_result:static_cast<ssize_t>(size);
}
extern "C" int dev_test_close(int fd) { assert(trace=="OTLW" && fd==open_result); trace+='C'; closed_fd=fd; return close_result; }
static void Reset() {
    trace.clear(); record.clear(); open_result=3; time_result=0; close_result=0;
    local_failure=false; write_override=false; write_result=0; micros=123456;
    sampled=0; written_fd=closed_fd=-1;
}
static std::string Prefix(const char* label,const char* stamp="1007 08:09.10.1234") {
    std::string level=label; if(level.size()<4) level+=std::string(4-level.size(),' ');
    return std::string(stamp)+" "+level+" (file.cpp       ,  42,Run                     ) ";
}
static void Call(int level) { char format[]="value=%d"; dev_log("file.cpp",42,"Run",level,format,7); assert(std::string(format)=="value=%d"); }
static void Emit(int level,const char* label) { Reset(); Call(level); assert(trace=="OTLWC" && record==Prefix(label)+"value=7\n"); }
static void MacroCall() { char format[]="macro=%d"; dev_log(LOG_DEB1,format,42); }
int main() {
    // Disabled paths must not inspect any caller string, including the format.
    Reset(); dev_log(reinterpret_cast<const char*>(1),0,reinterpret_cast<const char*>(1),L_INFO,reinterpret_cast<char*>(1)); assert(trace.empty());
    test_server=1;
    // Initial ELF mask is all32 bits, including levels without a known label.
    Emit(1,"UNKNOWN"); Emit(INT_MIN,"UNKNOWN");
    struct Level { int flag; const char* name; } levels[]={
        {L_WARN,"WARN"},{L_ERR,"ERR"},{L_CRIT,"CRIT"},{L_INFO,"INFO"},{L_MIN,"MIN"},{L_MAX,"MAX"},
        {L_LIB0,"LIB0"},{L_LIB1,"LIB1"},{L_LIB2,"LIB2"},{L_LIB3,"LIB3"},
        {L_DEB0,"DEB0"},{L_DEB1,"DEB1"},{L_DEB2,"DEB2"},{L_DEB3,"DEB3"},
        {L_USR0,"USR0"},{L_USR1,"USR1"},{L_USR2,"USR2"},{L_USR3,"USR3"}
    };
    for(unsigned i=0;i<sizeof(levels)/sizeof(*levels);++i) Emit(levels[i].flag,levels[i].name);
    Emit(L_INFO|L_ERR,"UNKNOWN");
    Reset(); Call(0); assert(trace.empty());
    dev_log_set_level(0); Reset(); dev_log(0,0,0,L_INFO,reinterpret_cast<char*>(1)); assert(trace.empty());
    dev_log_add_level(L_INFO); Emit(L_INFO,"INFO"); Reset(); Call(L_ERR); assert(trace.empty());
    dev_log_add_level(L_ERR); Emit(L_INFO|L_ERR,"UNKNOWN"); dev_log_del_level(L_INFO);
    Reset(); Call(L_INFO); assert(trace.empty()); Emit(L_ERR,"ERR");
    dev_log_set_level(INT_MIN); dev_log_add_level(L_INFO); Emit(INT_MIN,"UNKNOWN"); Emit(L_INFO,"INFO");
    dev_log_del_level(INT_MIN); Reset(); Call(INT_MIN); assert(trace.empty());
    dev_log_set_level(-1); test_server=-7; Emit(L_INFO,"INFO"); test_server=1;
    Reset(); open_result=-1; dev_log(reinterpret_cast<const char*>(1),1,reinterpret_cast<const char*>(1),L_INFO,reinterpret_cast<char*>(1)); assert(trace=="O" && record.empty() && !sampled);
    Reset(); open_result=0; Call(L_INFO); assert(trace=="OTLWC" && written_fd==0 && closed_fd==0 && record==Prefix("INFO")+"value=7\n");
    const ssize_t writes[]={-1,0,1};
    for(unsigned i=0;i<sizeof(writes)/sizeof(*writes);++i) { Reset(); write_override=true; write_result=writes[i]; close_result=-1; Call(L_INFO); assert(trace=="OTLWC" && record==Prefix("INFO")+"value=7\n"); }
    Reset(); time_result=-1; local_failure=true; Call(L_INFO); assert(trace=="OTLWC" && record==Prefix("INFO")+"value=7\n");
    Reset(); micros=0; Call(L_INFO); assert(record==Prefix("INFO","1007 08:09.10.0000")+"value=7\n");
    Reset(); micros=999999; Call(L_INFO); assert(record==Prefix("INFO","1007 08:09.10.9999")+"value=7\n");
    Reset(); char binary[]="%cX"; dev_log("file.cpp",42,"Run",L_INFO,binary,0);
    std::string binary_expected=Prefix("INFO"); binary_expected.push_back(0); binary_expected+="X\n"; assert(record==binary_expected);
    Reset(); char text[]="%s"; std::string payload(5000,'A'); payload+="%n\n"; dev_log("file.cpp",42,"Run",L_INFO,text,payload.c_str());
    assert(record==Prefix("INFO")+payload+"\n");
    Reset(); Call(L_INFO); assert(record==Prefix("INFO")+"value=7\n"); // No trailing bytes from the larger prior record.
    Reset(); const std::string file(70,'F'),function(80,'G'); char wide[]="wide"; dev_log(file.c_str(),-12345,function.c_str(),L_INFO,wide);
    assert(record=="1007 08:09.10.1234 INFO ("+file+",-12345,"+function+") wide\n");
    Reset(); MacroCall(); assert(trace=="OTLWC" && record.find(" DEB1 (")!=std::string::npos && record.find(__FILE__)!=std::string::npos && record.find("MacroCall")!=std::string::npos && record.substr(record.size()-9)=="macro=42\n");
    std::puts("developer log passed: old masks/labels, truncated timestamp, exact record framing, guard order and ignored OS failures");
}
