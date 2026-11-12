#include <string>
#include <queue>
#include <vector>
#include <map>
#include <set>
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
struct WorkerRuntime;
static WorkerRuntime* current;
struct Attempt {
    std::string text; int result; unsigned int error;
    Attempt(const char* t,int r=0,unsigned int e=0) : text(t),result(r),error(e) {}
};
struct Stamp {
    long seconds,micros; int result;
    Stamp(long s,long u,int r=0) : seconds(s),micros(u),result(r) {}
};
struct WorkerRuntime : SqlRuntime {
    CAsyncSQL* sql;
    int depth,waits,stop_at,wait_result,retry_posts,producer_posts,time_calls,errno_calls,thread_reads,charset_calls,charset_failures,store_calls,next_calls,freed,closed;
    bool running,inject_wait,inject_query;
    unsigned long server_id; unsigned int error_number;
    std::vector<Attempt> attempts; size_t attempt;
    std::vector<Stamp> stamps;
    std::vector<std::string> logs,errors,queries,events;
    WorkerRuntime() : sql(0),depth(0),waits(0),stop_at(1),wait_result(0),retry_posts(0),producer_posts(0),time_calls(0),errno_calls(0),thread_reads(0),charset_calls(0),charset_failures(0),store_calls(0),next_calls(0),freed(0),closed(0),running(false),inject_wait(false),inject_query(false),server_id(42),error_number(0),attempt(0) { current=this; sql_test_runtime=this; }
    void Check(MYSQL* db) { assert(sql && db==sql->GetSQLHandle()); }
    virtual unsigned long ThreadID(MYSQL* db) { Check(db); ++thread_reads; return server_id; }
    virtual const char* Error(MYSQL* db) { Check(db); return "worker-error"; }
    virtual unsigned int Errno(MYSQL* db) { Check(db); ++errno_calls; return error_number; }
    virtual int Charset(MYSQL* db,const char* name) { Check(db); assert(std::string(name)=="euckr"); ++charset_calls; if(charset_failures) { --charset_failures; return 1; } return 0; }
    virtual int Query(MYSQL* db,const char* text,unsigned long size) {
        Check(db); assert(!depth && size==std::strlen(text) && attempt<attempts.size());
        Attempt a=attempts[attempt++]; assert(a.text==std::string(text,size)); queries.push_back(a.text); error_number=a.error;
        if(inject_query && attempt==1) sql->ReturnQuery("flush",reinterpret_cast<void*>(3));
        return a.result;
    }
    virtual MYSQL_RES* Store(MYSQL* db) { Check(db); ++store_calls; return 0; }
    virtual my_ulonglong InsertID(MYSQL* db) { Check(db); return 7; }
    virtual my_ulonglong Affected(MYSQL* db) { Check(db); return 9; }
    virtual int Next(MYSQL* db) { Check(db); ++next_calls; return -1; }
    virtual void Free(MYSQL_RES* result) { assert(result); ++freed; }
    virtual void LogError(const char* function,int line,const char* text) { char prefix[80]; std::snprintf(prefix,sizeof(prefix),"%s:%d:",function,line); errors.push_back(std::string(prefix)+text); }
    virtual int Time(timeval* value,void* zone) {
        assert(value && !zone); size_t index=static_cast<size_t>(time_calls++);
        Stamp stamp=index<stamps.size()?stamps[index]:Stamp(10,0);
        if(!stamp.result) { value->tv_sec=stamp.seconds; value->tv_usec=stamp.micros; }
        return stamp.result;
    }
    int Wait() {
        assert(sql && !depth && ++waits<=4);
        if(inject_wait && waits==2) sql->ReturnQuery("new",reinterpret_cast<void*>(3));
        if(waits==stop_at) sql->m_bEnd=true;
        if(wait_result) errno=EINTR;
        return wait_result;
    }
};
extern "C" void sys_log(int level,const char* format,...) {
    assert(current && !level); char text[8192]; va_list args; va_start(args,format); std::vsnprintf(text,sizeof(text),format,args); va_end(args); current->logs.push_back(text);
}
extern "C" int sem_init(sem_t* h,int shared,unsigned int value) { assert(h && !shared && !value); *h=reinterpret_cast<sem_t>(1); return 0; }
extern "C" int sem_destroy(sem_t* h) { assert(h && *h); return 0; }
extern "C" int sem_wait(sem_t* h) { assert(h && *h && current); return current->Wait(); }
extern "C" int sem_post(sem_t* h) {
    assert(h && *h && current);
    if(current->depth) ++current->producer_posts; else { assert(current->running); ++current->retry_posts; }
    return 0;
}
extern "C" int sql_test_mutex_lock(pthread_mutex_t* h) {
    assert(current && current->sql && h && !current->depth);
    assert(h==current->sql->m_mtxQuery || h==current->sql->m_mtxResult); ++current->depth;
    current->events.push_back(h==current->sql->m_mtxQuery?"Q+":"R+"); return 0;
}
extern "C" int sql_test_mutex_unlock(pthread_mutex_t* h) {
    assert(current && current->depth==1 && (h==current->sql->m_mtxQuery || h==current->sql->m_mtxResult)); --current->depth; current->events.push_back("-"); return 0;
}
extern "C" int sql_test_mutex_destroy(pthread_mutex_t* h) { assert(h); return 0; }
extern "C" int sql_test_thread_join(pthread_t,void**) { assert(false); return -1; }
extern "C" void STDCALL mysql_close(MYSQL* db) { assert(current); current->Check(db); ++current->closed; }
struct Case {
    WorkerRuntime api;
    CAsyncSQL sql;
    Case() {
        api.sql=&sql; sql.m_mtxQuery=new pthread_mutex_t; sql.m_mtxResult=new pthread_mutex_t;
        sql.m_ulThreadID=42; sql.m_stLocale="ascii";
    }
    ~Case() {
        // Explicit owner cleanup for pointers retained by the original nonowning teardown.
        std::set<SQLMsg*> owned;
        while(!sql.m_queue_query.empty()) { owned.insert(sql.m_queue_query.front()); sql.m_queue_query.pop(); }
        while(!sql.m_queue_query_copy.empty()) { owned.insert(sql.m_queue_query_copy.front()); sql.m_queue_query_copy.pop(); }
        while(!sql.m_queue_result.empty()) { owned.insert(sql.m_queue_result.front()); sql.m_queue_result.pop(); }
        for(std::map<int,SQLMsg*>::iterator it=sql.m_map_kSQLMsgUnfinished.begin();it!=sql.m_map_kSQLMsgUnfinished.end();++it) owned.insert(it->second);
        sql.m_map_kSQLMsgUnfinished.clear();
        for(std::set<SQLMsg*>::iterator it=owned.begin();it!=owned.end();++it) delete *it;
    }
    SQLMsg* Enqueue(const char* query,unsigned int cached=0,bool result=true) {
        if(result) sql.ReturnQuery(query,reinterpret_cast<void*>(1)); else sql.AsyncQuery(query);
        SQLMsg* message=sql.m_queue_query.back(); message->uiSQLErrno=cached; return message;
    }
    void Run() {
        api.running=true; sql.ChildLoop(); api.running=false;
        assert(!api.depth && api.attempt==api.attempts.size());
    }
};
static void Timing(Stamp begin,Stamp end,const char* expected) {
    Case c; c.Enqueue("timed"); c.api.attempts.push_back(Attempt("timed"));
    c.api.stamps.push_back(Stamp(0,0)); c.api.stamps.push_back(begin); c.api.stamps.push_back(end); c.Run();
    assert(c.sql.m_iQueryFinished==1 && c.api.time_calls==3);
    if(expected) assert(c.api.logs.size()==1 && c.api.logs[0]==expected); else assert(c.api.logs.empty());
}
int main() {
    {
        Case c; SQLMsg* returned=c.Enqueue("ok"); SQLMsg* fire=c.Enqueue("fire",0,false);
        SQLResult* payload=new SQLResult; payload->pSQLResult=reinterpret_cast<MYSQL_RES*>(7); fire->vec_pkResult.push_back(payload);
        c.api.attempts.push_back(Attempt("ok")); c.api.attempts.push_back(Attempt("fire"));
        c.sql.m_iQueryFinished=INT_MAX; c.sql.m_iCopiedQuery=INT_MAX; c.Run();
        assert(c.sql.CountQuery()==0 && c.sql.m_queue_query_copy.empty() && c.sql.CountResult()==1 && c.sql.m_queue_result.front()==returned && returned->Get()->iInsertID==7 && returned->Get()->iAffectedRows==9);
        assert(c.sql.CountQueryFinished()==INT_MIN+1 && c.sql.GetCopiedQueryCount()==INT_MIN+1 && c.api.store_calls==1 && c.api.freed==1 && c.api.time_calls==5);
    }
    struct ErrorCase { unsigned int code; bool retry; } codes[]={
        {0,false},{1015,false},{1016,true},{1017,false},{1033,false},{1034,true},{1035,false},
        {1128,false},{1129,true},{1130,true},{1131,false},{1132,true},{1133,true},{1134,false},
        {1135,true},{1136,false},{1137,false},{1138,true},{1139,false},{1193,false},{1194,true},{1195,false},
        {2000,false},{2001,true},{2002,true},{2003,true},{2004,true},{2005,true},{2006,true},{2007,false},{UINT_MAX,false}
    };
    for(unsigned int i=0;i<sizeof(codes)/sizeof(*codes);++i) {
        Case c; SQLMsg* message=c.Enqueue("error",codes[i].code); c.api.attempts.push_back(Attempt("error",1,2006)); c.Run();
        assert(c.sql.GetCopiedQueryCount()==1);
        if(codes[i].retry) {
            assert(c.sql.m_queue_query_copy.size()==1 && c.sql.m_queue_query_copy.front()==message && !c.sql.CountResult() && !c.sql.CountQueryFinished());
            assert(message->uiSQLErrno==codes[i].code && c.api.retry_posts==1 && !c.api.errno_calls && !c.api.store_calls && c.api.time_calls==2);
        } else {
            assert(c.sql.m_queue_query_copy.empty() && c.sql.m_queue_result.front()==message && c.sql.CountQueryFinished()==1);
            assert(message->uiSQLErrno==2006 && !c.api.retry_posts && c.api.errno_calls==1 && c.api.store_calls==1 && c.api.time_calls==3);
        }
    }
    {
        Case c; SQLMsg* pin=c.Enqueue("pin",2006); c.Enqueue("later"); c.api.attempts.push_back(Attempt("pin",1,1064)); c.api.attempts.push_back(Attempt("pin",1,1064)); c.Run();
        assert(c.api.retry_posts==2 && c.sql.m_queue_query_copy.size()==2 && c.sql.m_queue_query_copy.front()==pin && !c.sql.CountQueryFinished() && !c.api.errno_calls);
    }
    {
        Case c; c.Enqueue("pin",2006); c.api.stop_at=2; c.api.attempts.push_back(Attempt("pin",1,1064)); c.Run();
        assert(c.api.waits==2 && c.sql.m_queue_query_copy.size()==1 && c.api.time_calls==2 && !c.sql.CountQueryFinished());
    }
    {
        Case c; c.Enqueue("pin",2006); c.Enqueue("later"); c.api.stop_at=2; c.api.inject_wait=true;
        c.api.attempts.push_back(Attempt("pin",1,1064)); c.api.attempts.push_back(Attempt("pin",1,1064));
        c.api.attempts.push_back(Attempt("pin")); c.api.attempts.push_back(Attempt("later")); c.api.attempts.push_back(Attempt("new")); c.Run();
        assert(c.api.waits==2 && c.api.retry_posts==2 && c.sql.GetCopiedQueryCount()==5 && c.sql.CountQueryFinished()==3 && c.sql.CountResult()==3 && c.sql.m_queue_query_copy.empty() && c.api.time_calls==9);
    }
    {
        Case c; SQLMsg* message=c.Enqueue("flush-error"); c.sql.m_bEnd=true; c.api.attempts.push_back(Attempt("flush-error",1,2006)); c.Run();
        assert(!c.api.waits && c.api.time_calls==1 && !c.api.errno_calls && message->uiSQLErrno==0 && c.sql.CountResult()==1 && c.sql.CountQueryFinished()==1 && !c.sql.GetCopiedQueryCount());
        assert(c.api.logs.size()==1 && c.api.logs[0]=="QUERY_FLUSH: flush-error");
    }
    for(unsigned int i=0;i<sizeof(codes)/sizeof(*codes);++i) if(codes[i].retry) {
        Case c; SQLMsg* message=c.Enqueue("flush-retry",codes[i].code); c.sql.m_bEnd=true;
        c.api.attempts.push_back(Attempt("flush-retry",1,1064)); c.api.attempts.push_back(Attempt("flush-retry")); c.Run();
        assert(c.api.queries.size()==2 && !c.api.waits && !c.api.retry_posts && !c.api.errno_calls && message->uiSQLErrno==codes[i].code && c.sql.CountResult()==1 && c.sql.CountQueryFinished()==1 && c.api.time_calls==1);
    }
    {
        Case c; c.Enqueue("copy-left"); assert(c.sql.CopyQuery()==1); c.sql.m_bEnd=true; c.Run();
        assert(c.sql.m_queue_query_copy.size()==1 && !c.sql.CountQueryFinished() && c.api.time_calls==1);
    }
    {
        Case c; c.Enqueue("active"); c.api.inject_query=true; c.api.attempts.push_back(Attempt("active")); c.api.attempts.push_back(Attempt("flush")); c.Run();
        assert(c.sql.CountResult()==2 && c.sql.CountQueryFinished()==2 && c.sql.GetCopiedQueryCount()==1 && c.api.time_calls==3 && c.api.logs.size()==1 && c.api.logs[0]=="QUERY_FLUSH: flush");
    }
    {
        Case c; c.Enqueue("interrupted"); c.api.wait_result=-1; c.api.attempts.push_back(Attempt("interrupted")); c.Run(); assert(c.sql.CountQueryFinished()==1);
    }
    {
        Case c; c.Enqueue("reconnect"); c.sql.m_ulThreadID=41; c.sql.m_stLocale="euckr"; c.api.charset_failures=1; c.api.attempts.push_back(Attempt("reconnect")); c.Run();
        assert(c.sql.m_ulThreadID==42 && c.api.thread_reads==2 && c.api.charset_calls==2 && c.api.errno_calls==1 && c.sql.CountQueryFinished()==1 && c.api.errors[0].find("ChildLoop:476:")==0);
    }
    Timing(Stamp(10,0),Stamp(10,200000),0);
    Timing(Stamp(10,0),Stamp(10,200001),"[QUERY : LONG INTERVAL(OverSec 0 uSec200001)] : timed");
    Timing(Stamp(10,999999),Stamp(11,0),"[QUERY : LONG INTERVAL(OverSec 1 uSec-999999)] : timed");
    Timing(Stamp(11,0),Stamp(10,300001),"[QUERY : LONG INTERVAL(OverSec -1 uSec300001)] : timed");
    Timing(Stamp(INT_MAX,0),Stamp(INT_MIN,0),"[QUERY : LONG INTERVAL(OverSec 1 uSec0)] : timed");
    Timing(Stamp(10,INT_MAX),Stamp(10,INT_MIN),0);
    Timing(Stamp(77,77,-1),Stamp(10,200001),"[QUERY : LONG INTERVAL(OverSec 10 uSec200001)] : timed");
    std::puts("SQL worker passed: real batching/results/reclamation, cached-error retry boundaries, pinned copy queues, producer-only flush and old timing/wrap");
}
