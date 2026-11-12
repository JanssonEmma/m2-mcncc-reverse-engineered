#include <string>
#include <queue>
#include <vector>
#include <map>
#define private public
#define protected public
#include "../../libsql/AsyncSQL.h"
#undef protected
#undef private
#include <cassert>
#include <cstring>
#include <cstdio>
#include <new>
#include <climits>
#include "sql_connection_fixture.h"
static CAsyncSQL* active;
static int lock_depth,initialized,cleared,closed,joins,wakes,deletions;
static size_t before_enqueue;
static bool enqueue_wake;
static std::string trace;
static pthread_t joined_thread;
struct QueueRuntime : SqlRuntime {
    virtual void Free(MYSQL_RES* result) { assert(result); ++deletions; }
};
extern "C" void sys_log(int,const char*,...) { assert(false); }
extern "C" int sem_init(sem_t* handle,int shared,unsigned int count) {
    assert(handle && !shared && !count); *handle=reinterpret_cast<sem_t>(1); ++initialized; return 0;
}
extern "C" int sem_destroy(sem_t* handle) { assert(handle && *handle); ++cleared; trace+='S'; return 0; }
extern "C" int sem_wait(sem_t*) { assert(false); return -1; }
extern "C" int sem_post(sem_t* handle) {
    assert(handle && *handle); ++wakes;
    if(enqueue_wake) assert(lock_depth==1 && active->m_queue_query.size()==before_enqueue+1);
    trace+='W'; return 0;
}
extern "C" int sql_test_mutex_lock(pthread_mutex_t* handle) {
    assert(active && handle && !lock_depth);
    assert(handle==active->m_mtxQuery || handle==active->m_mtxResult);
    before_enqueue=active->m_queue_query.size(); ++lock_depth;
    trace+=handle==active->m_mtxQuery?'Q':'R'; return 0;
}
extern "C" int sql_test_mutex_unlock(pthread_mutex_t* handle) {
    assert(active && lock_depth==1 && (handle==active->m_mtxQuery || handle==active->m_mtxResult));
    --lock_depth; trace+='U'; return 0;
}
extern "C" int sql_test_mutex_destroy(pthread_mutex_t* handle) {
    assert(active && handle);
    if(handle==active->m_mtxQuery) trace+='q';
    else { assert(handle==active->m_mtxResult); trace+='r'; }
    return 0;
}
extern "C" int sql_test_thread_join(pthread_t thread,void** result) {
    assert(active && thread==active->m_hThread && !result && active->m_bEnd && !lock_depth);
    ++joins; joined_thread=thread; trace+='J'; return 0;
}
extern "C" void STDCALL mysql_close(MYSQL* handle) {
    assert(active && handle==active->GetSQLHandle()); ++closed; trace+='C';
}
static void Unchanged(const void* field,size_t size) {
    const unsigned char* bytes=static_cast<const unsigned char*>(field);
    for(size_t i=0;i<size;++i) assert(bytes[i]==0xa5);
}
int main() {
    QueueRuntime runtime; sql_test_runtime=&runtime;
    union Storage { long double alignment; unsigned char bytes[sizeof(CAsyncSQL)]; } storage;
    std::memset(storage.bytes,0xa5,sizeof(storage.bytes));
    CAsyncSQL* sql=new(storage.bytes) CAsyncSQL; active=sql;
    assert(initialized==1 && !sql->m_iMsgCount && !sql->m_hThread && !sql->m_mtxQuery && !sql->m_mtxResult && !sql->m_bEnd);
    assert(!sql->m_iQueryFinished && !sql->m_bConnected && !sql->m_iCopiedQuery);
    assert(sql->m_stHost.empty() && sql->m_stUser.empty() && sql->m_stPassword.empty() && sql->m_stDB.empty() && sql->m_stLocale.empty());
    assert(sql->m_queue_query.empty() && sql->m_queue_query_copy.empty() && sql->m_queue_result.empty() && sql->m_map_kSQLMsgUnfinished.empty());
    Unchanged(&sql->m_hDB,sizeof(sql->m_hDB)); Unchanged(sql->m_aiPipe,sizeof(sql->m_aiPipe)); Unchanged(&sql->m_ulThreadID,sizeof(sql->m_ulThreadID));
    sql->m_mtxQuery=new pthread_mutex_t; sql->m_mtxResult=new pthread_mutex_t;
    SQLMsg* output=reinterpret_cast<SQLMsg*>(1);
    trace.clear(); assert(!sql->PeekQuery(&output) && output==reinterpret_cast<SQLMsg*>(1) && trace=="QU");
    trace.clear(); assert(!sql->PopQuery(999) && trace=="QU");
    trace.clear(); assert(!sql->PopResult(&output) && output==reinterpret_cast<SQLMsg*>(1) && trace=="RU");
    trace.clear(); assert(sql->CopyQuery()==-1 && trace=="QU");
    enqueue_wake=true; trace.clear(); sql->AsyncQuery("first"); assert(trace=="QWU");
    SQLMsg* first=sql->m_queue_query.front();
    int context=7; trace.clear(); sql->ReturnQuery("second",&context); assert(trace=="QWU");
    SQLMsg* second=sql->m_queue_query.back();
    assert(first->iID==1 && second->iID==2 && !first->bReturn && second->bReturn && second->pvUserData==&context && second->stQuery=="second");
    enqueue_wake=false; trace.clear(); assert(sql->CountQuery()==2 && !sql->CountResult() && trace.empty());
    assert(sql->PeekQuery(&output) && output==first && sql->CountQuery()==2);
    // The id is intentionally ignored and no message is freed.
    assert(sql->PopQuery(-1234) && sql->CountQuery()==1 && !deletions);
    assert(sql->CopyQuery()==1 && !sql->CountQuery());
    trace.clear(); assert(sql->PeekQueryFromCopyQueue(&output) && output==second && trace.empty());
    enqueue_wake=true; sql->AsyncQuery("third"); enqueue_wake=false;
    SQLMsg* third=sql->m_queue_query.front();
    assert(sql->CopyQuery()==2 && sql->m_queue_query_copy.front()==second && sql->m_queue_query_copy.back()==third);
    assert(sql->CopyQuery()==-1 && sql->m_queue_query_copy.size()==2 && !sql->GetCopiedQueryCount());
    assert(sql->PopQueryFromCopyQueue() && sql->PeekQueryFromCopyQueue(&output) && output==third);
    assert(sql->PopQueryFromCopyQueue() && !sql->PopQueryFromCopyQueue());
    output=reinterpret_cast<SQLMsg*>(1); assert(!sql->PeekQueryFromCopyQueue(&output) && output==reinterpret_cast<SQLMsg*>(1));
    trace.clear(); sql->PushResult(first); assert(trace=="RU"); sql->PushResult(second);
    assert(sql->CountResult()==2 && sql->PopResult(&output) && output==first && sql->PopResult(&output) && output==second && !sql->CountResult());
    sql->AddCopiedQueryCount(INT_MAX); sql->AddCopiedQueryCount(1); assert(sql->GetCopiedQueryCount()==INT_MIN);
    sql->ResetCopiedQueryCount(); assert(!sql->GetCopiedQueryCount());
    sql->m_iQueryFinished=99; assert(sql->CountQueryFinished()==99); sql->ResetQueryFinished(); assert(!sql->CountQueryFinished());
    trace.clear(); sql->DumpQueryQueue(reinterpret_cast<FILE*>(1)); assert(trace.empty());
    trace.clear(); sql->Quit(); assert(sql->m_bEnd && trace=="W" && !joins);
    sql->m_hThread=static_cast<pthread_t>(77); trace.clear(); sql->Quit(); assert(trace=="WJ" && joins==1 && joined_thread==77 && sql->m_hThread==77);
    // Populate every owning-looking container: the old destructor frees none of these messages.
    SQLMsg* messages[]={first,second,third};
    for(int i=0;i<3;++i) { SQLResult* result=new SQLResult; result->pSQLResult=reinterpret_cast<MYSQL_RES*>(i+1); messages[i]->vec_pkResult.push_back(result); }
    sql->m_queue_query.push(first); sql->m_queue_query_copy.push(second); sql->m_queue_result.push(third); sql->m_map_kSQLMsgUnfinished[third->iID]=third;
    const size_t query_offset=reinterpret_cast<unsigned char*>(&sql->m_mtxQuery)-storage.bytes;
    const size_t result_offset=reinterpret_cast<unsigned char*>(&sql->m_mtxResult)-storage.bytes;
    pthread_mutex_t* old_result=sql->m_mtxResult;
    trace.clear(); sql->~CAsyncSQL();
    assert(trace=="CqrS" && closed==1 && cleared==1 && joins==1 && wakes==5 && !deletions);
    unsigned char zeros[sizeof(pthread_mutex_t*)]; std::memset(zeros,0,sizeof(zeros));
    assert(!std::memcmp(storage.bytes+query_offset,zeros,sizeof(zeros)));
    assert(!std::memcmp(storage.bytes+result_offset,&old_result,sizeof(old_result))); // Proven wrong-pointer clear.
    delete first; delete second; delete third; assert(deletions==3); active=0;
    std::puts("async SQL passed: real lifecycle, retained raw fields, nonowning teardown, mutex bug, explicit join and FIFO/copy/result queues");
}
