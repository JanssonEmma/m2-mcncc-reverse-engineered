#include <string>
#include <queue>
#include <vector>
#include <map>
#define private public
#define protected public
#include "../../libsql/AsyncSQL.h"
#undef protected
#undef private
#include "sql_connection_fixture.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <climits>
#include <cerrno>
#include <process.h>
static int closes;
static std::vector<std::string> logs;
extern "C" void sys_log(int level,const char* format,...) {
    assert(!level); char text[8192]; va_list args; va_start(args,format); std::vsnprintf(text,sizeof(text),format,args); va_end(args); logs.push_back(text);
}
extern "C" int sem_init(sem_t* h,int shared,unsigned int count) { assert(h && !shared && !count); *h=reinterpret_cast<sem_t>(1); return 0; }
extern "C" int sem_destroy(sem_t* h) { assert(h && *h); return 0; }
extern "C" int sem_wait(sem_t*) { assert(false); return -1; }
extern "C" int sem_post(sem_t*) { assert(false); return -1; }
extern "C" int sql_test_mutex_lock(pthread_mutex_t* mutex) { assert(mutex); return 0; }
extern "C" int sql_test_mutex_unlock(pthread_mutex_t* mutex) { assert(mutex); return 0; }
extern "C" int sql_test_mutex_destroy(pthread_mutex_t* h) { assert(h); return 0; }
extern "C" int sql_test_thread_join(pthread_t,void**) { assert(false); return -1; }
extern "C" void STDCALL mysql_close(MYSQL* h) { assert(h); ++closes; }
struct Result {
    MYSQL_RES* pointer; my_ulonglong rows,insert,affected; int next;
    Result(unsigned long p,my_ulonglong r,my_ulonglong i,my_ulonglong a,int n) : pointer(reinterpret_cast<MYSQL_RES*>(p)),rows(r),insert(i),affected(a),next(n) {}
};
struct ErrorEntry { std::string function,text; int line; };
struct Runtime : SqlRuntime {
    MYSQL* expected;
    bool init_ok,connect_ok;
    int option_result,query_result,charset_failures,init_count,options_count,connect_count,charset_count,queries,mutex_count,fatal_at,create_count,create_result,worker_count,freed;
    unsigned long server_id;
    unsigned int error_number;
    std::string charset,last_query,last_host,last_user,last_password,last_db,trace;
    void* thread_argument; void*(*thread_entry)(void*);
    std::vector<Result> results; size_t result;
    std::vector<ErrorEntry> errors;
    Runtime() : expected(0),init_ok(true),connect_ok(true),option_result(0),query_result(0),charset_failures(0),init_count(0),options_count(0),connect_count(0),charset_count(0),queries(0),mutex_count(0),fatal_at(0),create_count(0),create_result(EAGAIN),worker_count(0),freed(0),server_id(42),error_number(1234),thread_argument(0),thread_entry(0),result(0) {}
    void Check(MYSQL* db) { assert(db==expected); }
    virtual MYSQL* Init(MYSQL* db) { Check(db); ++init_count; trace+='I'; return init_ok?db:0; }
    virtual int Options(MYSQL* db,mysql_option option,const char* value) { Check(db); assert(option==MYSQL_SET_CHARSET_NAME); charset=value; ++options_count; trace+='O'; return option_result; }
    virtual MYSQL* Connect(MYSQL* db,const char* host,const char* user,const char* password,const char* name,unsigned int port,const char* socket,unsigned long flags) {
        Check(db); assert(!port && !socket && flags==0x10000); ++connect_count; trace+='C'; last_host=host; last_user=user; last_password=password; last_db=name; return connect_ok?db:0;
    }
    virtual const char* Error(MYSQL* db) { Check(db); trace+='E'; return "server-error"; }
    virtual unsigned int Errno(MYSQL* db) { Check(db); trace+='e'; return error_number; }
    virtual unsigned long ThreadID(MYSQL* db) { Check(db); trace+='T'; return server_id; }
    virtual int Query(MYSQL* db,const char* text,unsigned long size) { Check(db); assert(size==std::strlen(text)); ++queries; trace+='Q'; last_query.assign(text,size); return query_result; }
    virtual int Charset(MYSQL* db,const char* name) { Check(db); charset=name; ++charset_count; trace+='c'; if(charset_failures) { --charset_failures; return 1; } return 0; }
    virtual MYSQL_RES* Store(MYSQL* db) { Check(db); assert(result<results.size()); trace+='S'; return results[result].pointer; }
    virtual my_ulonglong InsertID(MYSQL* db) { Check(db); trace+='i'; return results[result].insert; }
    virtual my_ulonglong Affected(MYSQL* db) { Check(db); trace+='a'; return results[result].affected; }
    virtual my_ulonglong Rows(MYSQL_RES* pointer) { assert(pointer && pointer==results[result].pointer); trace+='r'; return results[result].rows; }
    virtual int Next(MYSQL* db) { Check(db); trace+='N'; return results[result++].next; }
    virtual void Free(MYSQL_RES* pointer) { assert(pointer); ++freed; trace+='F'; }
    virtual int MutexInit(pthread_mutex_t* mutex,const pthread_mutexattr_t* attr) {
        assert(mutex && !attr); ++mutex_count; if(mutex_count==fatal_at) { errno=EINVAL; return EINVAL; } std::memset(mutex,0,sizeof(*mutex)); return 0;
    }
    virtual int Create(pthread_t* thread,const pthread_attr_t* attr,void*(*entry)(void*),void* argument) {
        assert(thread && !attr && entry==AsyncSQLThread); ++create_count; thread_argument=argument; thread_entry=entry; if(!create_result) *thread=static_cast<pthread_t>(73); return create_result;
    }
    virtual void LogError(const char* function,int line,const char* text) { ErrorEntry e; e.function=function; e.line=line; e.text=text; errors.push_back(e); }
    virtual int Time(timeval* value,void* zone) { assert(value && !zone); value->tv_sec=0; value->tv_usec=0; ++worker_count; return 0; }
};
int main(int argc,char** argv) {
    Runtime runtime; sql_test_runtime=&runtime;
    for(int i=1;i+1<argc;++i) if(std::string(argv[i])=="--mutex-fail") {
        runtime.fatal_at=std::atoi(argv[i+1]); CAsyncSQL sql; runtime.expected=sql.GetSQLHandle(); sql.Setup("h","u","p","d","ascii",false); return 99;
    }
    {
        CAsyncSQL sql; runtime.expected=sql.GetSQLHandle();
        assert(sql.QueryLocaleSet() && runtime.errors.back().line==56 && runtime.queries==0 && runtime.charset_count==0);
        sql.m_stLocale="ascii"; assert(sql.QueryLocaleSet() && runtime.errors.back().line==61 && runtime.charset_count==0);
        sql.m_stLocale="latin5"; runtime.query_result=1;
        assert(!sql.QueryLocaleSet() && runtime.last_query=="SET NAMES \"latin5\"" && runtime.errors.back().line==74);
        runtime.query_result=0; assert(sql.QueryLocaleSet() && runtime.charset_count==0);
        sql.m_stLocale="ASCII"; assert(sql.QueryLocaleSet() && runtime.charset=="ASCII");
        sql.m_stLocale="euckr"; runtime.charset_failures=1; assert(!sql.QueryLocaleSet() && runtime.errors.back().line==82); assert(sql.QueryLocaleSet());
        runtime.init_ok=false; assert(!sql.Connect() && !sql.m_bConnected);
        runtime.init_ok=true; runtime.option_result=1; runtime.connect_ok=false; assert(!sql.Connect() && runtime.options_count==1 && runtime.connect_count==1);
        runtime.option_result=0; runtime.connect_ok=true; runtime.charset_failures=2;
        int before=runtime.charset_count; assert(sql.Connect() && runtime.charset_count==before+3 && sql.m_bConnected && sql.m_ulThreadID==42);
        runtime.init_ok=false; assert(!sql.Connect() && sql.m_bConnected); runtime.init_ok=true;
        sql.m_stLocale="ascii";
        assert(sql.Setup("host","user","password","db",0,true) && sql.m_stLocale=="ascii" && !sql.m_mtxQuery && !sql.m_mtxResult && !runtime.create_count);
        assert(runtime.last_host=="host" && runtime.last_user=="user" && runtime.last_password=="password" && runtime.last_db=="db");
        {
            CAsyncSQL clone; runtime.expected=clone.GetSQLHandle(); assert(clone.Setup(&sql,true));
            assert(clone.m_stHost==sql.m_stHost && clone.m_stLocale==sql.m_stLocale && !clone.m_mtxQuery);
        }
        runtime.expected=sql.GetSQLHandle(); sql.m_ulThreadID=41; sql.m_iMsgCount=INT_MAX; sql.m_iQueryFinished=77;
        runtime.results.clear(); runtime.result=0;
        runtime.results.push_back(Result(1,0x100000005ULL,0x1234567887654321ULL,0xffffffffffffffffULL,0));
        runtime.results.push_back(Result(0,99,7,0x80000000ULL,0));
        runtime.results.push_back(Result(2,10,0xffffffffULL,2,-1));
        runtime.trace.clear(); SQLMsg* message=sql.DirectQuery("select one");
        assert(message->iID==INT_MIN && message->m_pkSQL==sql.GetSQLHandle() && message->stQuery=="select one" && !message->bReturn && !message->uiSQLErrno && message->vec_pkResult.size()==3 && sql.m_iQueryFinished==77 && sql.m_ulThreadID==42);
        assert(runtime.trace=="TTQSiarNSiaNSiarN");
        assert(message->Get()->iNumRows==5 && message->Get()->iInsertID==-2023406815 && message->Get()->iAffectedRows==-1);
        assert(message->Next() && !message->Get()->pSQLResult && !message->Get()->iNumRows && message->Get()->iAffectedRows==INT_MIN);
        assert(message->Next() && message->Get()->iInsertID==-1 && !message->Next() && message->uiResultPos==2);
        message->uiResultPos=UINT_MAX; assert(!message->Get() && message->Next() && message->uiResultPos==0);
        message->uiResultPos=2; runtime.results.clear(); runtime.results.push_back(Result(0,0,3,4,1)); runtime.result=0; message->Store();
        assert(message->vec_pkResult.size()==4 && message->uiResultPos==2); delete message; assert(runtime.freed==2);
        runtime.query_result=1; runtime.result=0; runtime.error_number=4321; runtime.trace.clear();
        message=sql.DirectQuery("bad select"); assert(message->uiSQLErrno==4321 && message->vec_pkResult.size()==1 && !message->Get()->pSQLResult && runtime.errors.back().line==219 && runtime.trace=="TQEeSiaN"); delete message;
        runtime.result=0; message=sql.DirectQuery(std::string(1500,'Z').c_str()); assert(message->stQuery.size()==1500 && runtime.errors.back().text.size()==998); delete message;
        runtime.query_result=0;
    }
    {
        CAsyncSQL threaded; runtime.expected=threaded.GetSQLHandle(); int connects=runtime.connect_count;
        assert(threaded.Setup("h","u","p","d","ascii",false) && runtime.mutex_count==2 && runtime.create_count==1 && runtime.connect_count==connects && !threaded.m_hThread && !threaded.m_bConnected);
        assert(runtime.thread_argument==&threaded && runtime.thread_entry==AsyncSQLThread && threaded.m_mtxQuery && threaded.m_mtxResult);
        runtime.connect_ok=false; assert(!AsyncSQLThread(&threaded) && !runtime.worker_count);
        runtime.connect_ok=true; threaded.m_bEnd=true; assert(!AsyncSQLThread(&threaded) && runtime.worker_count==1);
    }
    const std::string quoted_executable="\""+std::string(argv[0])+"\"";
    const char* first[]={quoted_executable.c_str(),"--mutex-fail","1",0}; const char* second[]={quoted_executable.c_str(),"--mutex-fail","2",0};
    assert(_spawnv(_P_WAIT,argv[0],first)==0 && _spawnv(_P_WAIT,argv[0],second)==0);
    assert(closes==3);
    std::puts("SQL connection/results passed: old locale branches/retries, setup/copy/thread entry, fatal mutex paths, reconnect, multi-results, narrowing and real reclamation");
}
