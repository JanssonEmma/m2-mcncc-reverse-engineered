// Test-only POSIX call redirects; production source is included unchanged.
#ifndef MCNCC_TEST_DEV_LOG_HOOKS_H
#define MCNCC_TEST_DEV_LOG_HOOKS_H
#include <cstddef>
#include <ctime>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
extern "C" int dev_test_open(const char*,int,...);
extern "C" ssize_t dev_test_write(int,const void*,size_t);
extern "C" int dev_test_close(int);
extern "C" int dev_test_gettimeofday(timeval*,void*);
extern "C" tm* dev_test_localtime_r(const time_t*,tm*);
#undef O_WRONLY
#undef O_APPEND
#undef O_CREAT
#define O_WRONLY 0x1
#define O_APPEND 0x8
#define O_CREAT 0x200
#undef gettimeofday
#define gettimeofday dev_test_gettimeofday
#define localtime_r dev_test_localtime_r
#define open dev_test_open
#define write dev_test_write
#define close dev_test_close
#endif
