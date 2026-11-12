#include "sql_connection_fixture.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
SqlRuntime* sql_test_runtime=0;
static SqlRuntime& Runtime() { assert(sql_test_runtime); return *sql_test_runtime; }
extern "C" MYSQL* STDCALL mysql_init(MYSQL* db) { return Runtime().Init(db); }
extern "C" int STDCALL mysql_options(MYSQL* db,mysql_option option,const char* value) { return Runtime().Options(db,option,value); }
extern "C" MYSQL* STDCALL mysql_real_connect(MYSQL* db,const char* host,const char* user,const char* password,const char* name,unsigned int port,const char* socket,unsigned long flags) { return Runtime().Connect(db,host,user,password,name,port,socket,flags); }
extern "C" const char* STDCALL mysql_error(MYSQL* db) { return Runtime().Error(db); }
extern "C" unsigned int STDCALL mysql_errno(MYSQL* db) { return Runtime().Errno(db); }
extern "C" unsigned long STDCALL mysql_thread_id(MYSQL* db) { return Runtime().ThreadID(db); }
extern "C" int STDCALL mysql_real_query(MYSQL* db,const char* query,unsigned long size) { return Runtime().Query(db,query,size); }
extern "C" int STDCALL mysql_set_character_set(MYSQL* db,const char* name) { return Runtime().Charset(db,name); }
extern "C" MYSQL_RES* STDCALL mysql_store_result(MYSQL* db) { return Runtime().Store(db); }
extern "C" my_ulonglong STDCALL mysql_insert_id(MYSQL* db) { return Runtime().InsertID(db); }
extern "C" my_ulonglong STDCALL mysql_affected_rows(MYSQL* db) { return Runtime().Affected(db); }
extern "C" my_ulonglong STDCALL mysql_num_rows(MYSQL_RES* result) { return Runtime().Rows(result); }
extern "C" int STDCALL mysql_next_result(MYSQL* db) { return Runtime().Next(db); }
extern "C" void STDCALL mysql_free_result(MYSQL_RES* result) { Runtime().Free(result); }
extern "C" int sql_test_mutex_init(pthread_mutex_t* mutex,const pthread_mutexattr_t* attr) { return Runtime().MutexInit(mutex,attr); }
extern "C" int sql_test_thread_create(pthread_t* thread,const pthread_attr_t* attr,void*(*entry)(void*),void* argument) { return Runtime().Create(thread,attr,entry,argument); }
// Legacy network suites provide their own logging boundary.
#ifndef MCNCC_TEST_SQL_EXTERNAL_SYS_ERR
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    char message[8192]; va_list args; va_start(args,format); std::vsnprintf(message,sizeof(message),format,args); va_end(args);
    Runtime().LogError(function,line,message);
}
#endif
extern "C" int sql_test_gettimeofday(timeval* value,void* zone) { return Runtime().Time(value,zone); }
