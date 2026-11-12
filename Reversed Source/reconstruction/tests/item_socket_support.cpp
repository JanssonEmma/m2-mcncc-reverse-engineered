// External API boundary only. Unused calls abort unless a test supplies a runtime.
#include "item_socket_support.h"
#include "dev_log_hooks.h"
#include <cassert>
#include <cstdarg>
static SqlRuntime& Runtime() { assert(sql_test_runtime); return *sql_test_runtime; }
extern "C" void STDCALL mysql_close(MYSQL* db) { Runtime().Close(db); }
extern "C" unsigned long STDCALL mysql_real_escape_string(MYSQL* db,char* to,const char* from,unsigned long count) { return Runtime().Escape(db,to,from,count); }
extern "C" int sql_test_mutex_lock(pthread_mutex_t* mutex) { return Runtime().MutexLock(mutex); }
extern "C" int sql_test_mutex_unlock(pthread_mutex_t* mutex) { return Runtime().MutexUnlock(mutex); }
extern "C" int sql_test_mutex_destroy(pthread_mutex_t* mutex) { return Runtime().MutexDestroy(mutex); }
extern "C" int sql_test_thread_join(pthread_t thread,void** result) { return Runtime().Join(thread,result); }
extern "C" int sem_init(sem_t* sem,int shared,unsigned int count) { return Runtime().SemInit(sem,shared,count); }
extern "C" int sem_destroy(sem_t* sem) { return Runtime().SemDestroy(sem); }
extern "C" int sem_wait(sem_t* sem) { return Runtime().SemWait(sem); }
extern "C" int sem_post(sem_t* sem) { return Runtime().SemPost(sem); }
ItemDevIo* item_dev_io=0;
extern "C" int dev_test_open(const char* file,int flags,...) { assert(item_dev_io); va_list args; va_start(args,flags); int mode=va_arg(args,int); va_end(args); return item_dev_io->Open(file,flags,mode); }
extern "C" int dev_test_gettimeofday(timeval* value,void* zone) { assert(item_dev_io); return item_dev_io->Time(value,zone); }
extern "C" tm* dev_test_localtime_r(const time_t* epoch,tm* value) { assert(item_dev_io); return item_dev_io->Local(epoch,value); }
extern "C" ssize_t dev_test_write(int fd,const void* data,size_t size) { assert(item_dev_io); return static_cast<ssize_t>(item_dev_io->Write(fd,data,size)); }
extern "C" int dev_test_close(int fd) { assert(item_dev_io); return item_dev_io->Close(fd); }
