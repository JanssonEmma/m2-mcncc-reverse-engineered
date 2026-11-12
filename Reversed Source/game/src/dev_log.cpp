// Target-backed developer log; native Windows support remains deferred.
#include "dev_log.h"
#include "config.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
static int s_log_mask=-1;
void dev_log(const char* file,int line,const char* function,int level,char* format,...) {
    if(!test_server || !(s_log_mask & level)) return;
    const int fd=open("DEV_LOG.log",O_WRONLY|O_APPEND|O_CREAT,0666);
    if(fd<0) return;
    static char buf[1024*1024];
    timeval tv; tm local; char strtime[64];
    gettimeofday(&tv,0);
    localtime_r(reinterpret_cast<const time_t*>(&tv.tv_sec),&local);
    int length=std::sprintf(strtime,"%02d%02d %02d:%02d.%02d.%06d",local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec,static_cast<int>(tv.tv_usec));
    strtime[length-2]=0;
    const char* label;
    switch(level) {
    case L_WARN: label="WARN"; break;
    case L_ERR: label="ERR"; break;
    case L_CRIT: label="CRIT"; break;
    case L_INFO: label="INFO"; break;
    case L_MIN: label="MIN"; break;
    case L_MAX: label="MAX"; break;
    case L_LIB0: label="LIB0"; break;
    case L_LIB1: label="LIB1"; break;
    case L_LIB2: label="LIB2"; break;
    case L_LIB3: label="LIB3"; break;
    case L_DEB0: label="DEB0"; break;
    case L_DEB1: label="DEB1"; break;
    case L_DEB2: label="DEB2"; break;
    case L_DEB3: label="DEB3"; break;
    case L_USR0: label="USR0"; break;
    case L_USR1: label="USR1"; break;
    case L_USR2: label="USR2"; break;
    case L_USR3: label="USR3"; break;
    default: label="UNKNOWN"; break;
    }
    length=std::sprintf(buf,"%s %-4s (%-15s,%4d,%-24s) ",strtime,label,file,line,function);
    va_list args; va_start(args,format);
    length+=std::vsprintf(buf+length,format,args); va_end(args);
    buf[length]='\n'; buf[length+1]=0;
    write(fd,buf,length+1);
    close(fd);
}
void dev_log_add_level(int level) { s_log_mask|=level; }
void dev_log_del_level(int level) { s_log_mask&=~level; }
void dev_log_set_level(int mask) { s_log_mask=mask; }
