#include <string>
#include <queue>
#include <vector>
#include <map>
#define private public
#define protected public
#include "../../game/src/log.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "../../game/src/locale_service.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <climits>
#include <cerrno>
#include "sql_connection_fixture.h"
static int initialized,destroyed,waited,posted,init_result,wait_result,post_result,locked,deleted;

static size_t queue_size_before_lock;
static CAsyncSQL* active;
static MYSQL* escaped_handle;
static std::string actions,last_log,last_locale;
extern "C" int sem_init(sem_t* h,int shared,unsigned int value) {
    assert(h && !shared && !value); *h=reinterpret_cast<sem_t>(1); ++initialized;
    if(init_result) errno=EINVAL;
    return init_result;
}
extern "C" int sem_destroy(sem_t* h) { assert(h && *h); ++destroyed; return 0; }
extern "C" int sem_wait(sem_t* h) { assert(h && *h); ++waited; if(wait_result) errno=EINTR; return wait_result; }
extern "C" int sem_post(sem_t* h) {
    assert(h && *h); ++posted;
    if(active) { assert(locked==1 && active->m_queue_query.size()==queue_size_before_lock+1); assert(active->m_queue_query.back()->m_pkSQL==&active->m_hDB && active->m_queue_query.back()->iID==active->m_iMsgCount); actions+='P'; }
    return post_result;
}
extern "C" int sql_test_mutex_destroy(pthread_mutex_t* h) { assert(h); return 0; }
extern "C" int sql_test_thread_join(pthread_t,void**) { assert(false); return 0; }
extern "C" void STDCALL mysql_close(MYSQL* db) { assert(db==escaped_handle); }
extern "C" int sql_test_mutex_lock(pthread_mutex_t* h) { assert(active && h==active->m_mtxQuery && !locked); queue_size_before_lock=active->m_queue_query.size(); ++locked; actions+='L'; return 0; }
extern "C" int sql_test_mutex_unlock(pthread_mutex_t* h) { assert(active && h==active->m_mtxQuery && locked==1); --locked; actions+='U'; return 0; }
extern "C" unsigned long STDCALL mysql_real_escape_string(MYSQL* db,char* to,const char* from,unsigned long count) {
    assert(db==escaped_handle); unsigned long used=0;
    for(unsigned long i=0;i<count;++i) { if(from[i]=='\'' || from[i]=='\\') to[used++]='\\'; to[used++]=from[i]; }
    to[used]=0; return used;
}
extern "C" void sys_log(int level,const char* format,...) {
    assert(!level); char text[8192]; va_list args; va_start(args,format); std::vsnprintf(text,sizeof(text),format,args); va_end(args); last_log=text;
}
struct AuditRuntime : SqlRuntime {
    virtual int MutexInit(pthread_mutex_t* mutex,const pthread_mutexattr_t* attr) { assert(mutex && !attr); std::memset(mutex,0,sizeof(*mutex)); return 0; }
    virtual int Create(pthread_t*,const pthread_attr_t* attr,void*(*entry)(void*),void* argument) { assert(!attr && entry==AsyncSQLThread && argument==active); return EAGAIN; }
    virtual void Free(MYSQL_RES* result) { assert(result); ++deleted; }
};
int main() {
    AuditRuntime runtime; sql_test_runtime=&runtime;
    assert(g_stLocale=="euckr" && g_table_postfix.empty());
    {
        CSemaphore semaphore; assert(initialized==1 && !destroyed);
        assert(semaphore.Wait()==1 && waited==1);
        wait_result=-1; assert(!semaphore.Wait() && waited==2); // EINTR is not retried.
        wait_result=0; assert(semaphore.Release(3) && posted==3);
        post_result=-1; assert(semaphore.Release() && posted==4); post_result=0;
        assert(semaphore.Release(0) && semaphore.Release(-2) && posted==4);
        semaphore.Clear(); assert(destroyed==1 && semaphore.Wait() && waited==2 && !semaphore.Release());
        semaphore.Destroy(); assert(destroyed==1);
        init_result=-1; assert(!semaphore.Initialize() && initialized==2);
        init_result=0; assert(semaphore.Initialize() && initialized==3 && destroyed==2);
    }
    assert(destroyed==3);
    {
        LogManager logger; assert(LogManager::instance_ptr()==&logger && !logger.IsConnected());
        escaped_handle=logger.m_sql.GetSQLHandle(); assert(escaped_handle==&logger.m_sql.m_hDB);
        active=&logger.m_sql;
        assert(logger.Connect("host","user","password","database"));
        assert(logger.IsConnected() && !logger.m_sql.m_bConnected && !logger.m_sql.m_hThread && logger.m_sql.m_stLocale=="euckr");
        // Thread creation failure is ignored; expose the logger's unguarded query path.
        logger.m_bIsConnect=false; g_table_postfix="_old";
        logger.ItemLog(0xffffffffUL,17,29,1234,"SET_SOCKET","a'b\\c");
        assert(actions=="LPU" && !locked && logger.m_sql.m_iMsgCount==1);
        SQLMsg* message=logger.m_sql.m_queue_query.front();
        assert(message->m_pkSQL==escaped_handle && message->iID==1 && !message->bReturn && !message->pvUserData && !message->uiResultPos && !message->uiSQLErrno && message->vec_pkResult.empty());
        const std::string prefix="INSERT DELAYED INTO log_old (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), 4294967295, 17, 29, 1234, 'SET_SOCKET', '";
        assert(last_log==prefix+"a'b\\c')");
        assert(message->stQuery==prefix+"a\\'b\\\\c')");
        assert(!logger.IsConnected()); // The old query path has no connection guard.
        actions.clear(); logger.Query("%s",std::string(6000,'Q').c_str());
        assert(actions=="LPU" && logger.m_sql.m_queue_query.back()->stQuery==std::string(4095,'Q') && logger.m_sql.m_queue_query.back()->iID==2);
        actions.clear(); logger.m_sql.m_iMsgCount=INT_MAX; logger.m_sql.AsyncQuery("wrapped");
        assert(actions=="LPU" && logger.m_sql.m_iMsgCount==INT_MIN && logger.m_sql.m_queue_query.back()->iID==INT_MIN);
        assert(logger.m_sql.m_queue_query.front()==message && logger.m_sql.m_queue_query.size()==3);
        char output[16]; const char bytes[]={'x',0,'y'}; assert(logger.EscapeString(output,bytes,3)==3 && !std::memcmp(output,bytes,3));
        // Old SQL teardown does not own/drain queued messages.
        while(!logger.m_sql.m_queue_query.empty()) {
            SQLResult* result=new SQLResult; result->pSQLResult=reinterpret_cast<MYSQL_RES*>(1);
            logger.m_sql.m_queue_query.front()->vec_pkResult.push_back(result);
            delete logger.m_sql.m_queue_query.front(); logger.m_sql.m_queue_query.pop();
        }
        active=0;
    }
    assert(!LogManager::instance_ptr() && deleted==3 && initialized==4 && destroyed==4);
    std::puts("item audit passed: old SQL format, raw/escaped hints, retained connection state, real queue/wake ordering, ID wrap and semaphore failures");
}
