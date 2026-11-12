// Scripted external API boundary; all unspecified calls fail immediately.
#ifndef MCNCC_TEST_SQL_CONNECTION_FIXTURE_H
#define MCNCC_TEST_SQL_CONNECTION_FIXTURE_H
#include "../../libsql/AsyncSQL.h"
#include <cstdlib>
struct timeval;
struct SqlRuntime {
    virtual ~SqlRuntime() {}
    virtual MYSQL* Init(MYSQL*) { std::abort(); }
    virtual int Options(MYSQL*,mysql_option,const char*) { std::abort(); }
    virtual MYSQL* Connect(MYSQL*,const char*,const char*,const char*,const char*,unsigned int,const char*,unsigned long) { std::abort(); }
    virtual const char* Error(MYSQL*) { std::abort(); }
    virtual unsigned int Errno(MYSQL*) { std::abort(); }
    virtual unsigned long ThreadID(MYSQL*) { std::abort(); }
    virtual int Query(MYSQL*,const char*,unsigned long) { std::abort(); }
    virtual int Charset(MYSQL*,const char*) { std::abort(); }
    virtual MYSQL_RES* Store(MYSQL*) { std::abort(); }
    virtual my_ulonglong InsertID(MYSQL*) { std::abort(); }
    virtual my_ulonglong Affected(MYSQL*) { std::abort(); }
    virtual my_ulonglong Rows(MYSQL_RES*) { std::abort(); }
    virtual int Next(MYSQL*) { std::abort(); }
    virtual void Free(MYSQL_RES*) { std::abort(); }
    virtual int MutexInit(pthread_mutex_t*,const pthread_mutexattr_t*) { std::abort(); }
    virtual int Create(pthread_t*,const pthread_attr_t*,void*(*)(void*),void*) { std::abort(); }
    virtual void LogError(const char*,int,const char*) { std::abort(); }
    virtual int Time(timeval*,void*) { std::abort(); }
    virtual void Close(MYSQL*) { std::abort(); }
    virtual unsigned long Escape(MYSQL*,char*,const char*,unsigned long) { std::abort(); }
    virtual int MutexLock(pthread_mutex_t*) { std::abort(); }
    virtual int MutexUnlock(pthread_mutex_t*) { std::abort(); }
    virtual int MutexDestroy(pthread_mutex_t*) { std::abort(); }
    virtual int Join(pthread_t,void**) { std::abort(); }
    virtual int SemInit(sem_t*,int,unsigned int) { std::abort(); }
    virtual int SemDestroy(sem_t*) { std::abort(); }
    virtual int SemWait(sem_t*) { std::abort(); }
    virtual int SemPost(sem_t*) { std::abort(); }
};
extern SqlRuntime* sql_test_runtime;
extern void* AsyncSQLThread(void*);
#endif
