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
static bool change_postfix,reenter;
static LogManager* current_logger;
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
    assert(db==escaped_handle); actions+='E'; unsigned long used=0;
    for(unsigned long i=0;i<count;++i) { if(from[i]=='\'' || from[i]=='\\') to[used++]='\\'; to[used++]=from[i]; }
    to[used]=0; return used;
}
extern "C" void sys_log(int level,const char* format,...) {
    assert(!level); char text[8192]; va_list args; va_start(args,format); std::vsnprintf(text,sizeof(text),format,args); va_end(args); last_log=text; actions+='S';
    if(change_postfix) { change_postfix=false; g_table_postfix="_after"; }
    if(reenter) { reenter=false; current_logger->ItemLog(9,8,7,6,"INNER","nested'hint"); }
}
struct AuditRuntime : SqlRuntime {
    virtual int MutexInit(pthread_mutex_t* mutex,const pthread_mutexattr_t* attr) { assert(mutex && !attr); std::memset(mutex,0,sizeof(*mutex)); return 0; }
    virtual int Create(pthread_t*,const pthread_attr_t* attr,void*(*entry)(void*),void* argument) { assert(!attr && entry==AsyncSQLThread && argument==active); return EAGAIN; }
    virtual void Free(MYSQL_RES* result) { assert(result); ++deleted; }
};

int main() {
    AuditRuntime runtime; sql_test_runtime=&runtime;
    {
        LogManager logger; current_logger=&logger;
        escaped_handle=logger.m_sql.GetSQLHandle(); active=&logger.m_sql;
        assert(logger.Connect("host","user","password","database"));
        // Real Setup initializes mutexes; host thread-create boundary fails as in existing fixture.
        logger.m_bIsConnect=false;
        g_table_postfix="_old"; actions.clear();
        logger.CharLog(0xffffffffUL,0x80000000UL,0xffffffffUL,0xffffffffUL,"GOLD_OVERFLOW","a'b\\c");
        assert(actions=="ESLPU" && !locked && logger.m_sql.m_queue_query.size()==1);
        SQLMsg* message=logger.m_sql.m_queue_query.front();
        const std::string prefix="INSERT DELAYED INTO log_old (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 4294967295, 2147483648, 4294967295, 4294967295, 'GOLD_OVERFLOW', '";
        assert(last_log==prefix+"a'b\\c')");
        assert(message->stQuery==prefix+"a\\'b\\\\c')");
        assert(message->iID==1 && !message->bReturn && !message->pvUserData && message->m_pkSQL==escaped_handle);
        assert(!logger.IsConnected());

        // The original does not cache postfix across sys_log.
        change_postfix=true; g_table_postfix="_before"; actions.clear();
        logger.CharLog(1,2,3,4,"T","h");
        assert(actions=="ESLPU");
        assert(last_log.find("INTO log_before ")!=std::string::npos);
        assert(logger.m_sql.m_queue_query.back()->stQuery.find("INTO log_after ")!=std::string::npos);

        // Empty hints replace prior escaped contents; text is not escaped by CharLog.
        actions.clear(); g_table_postfix.clear();
        logger.CharLog(0,0,0,0,"raw'text","");
        assert(actions=="ESLPU");
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 0, 0, 0, 0, 'raw'text', '')");

        // The1024-byte buffer is shared with ItemLog. Preserve real callback ordering.
        // Inner ItemLog queues first and overwrites the escaped buffer used by outer CharLog.
        actions.clear(); reenter=true;
        logger.CharLog(1,2,3,4,"OUTER","outer'hint");
        assert(actions=="ESESLPULPU");
        assert(logger.m_sql.m_queue_query.size()==5);
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 1, 2, 3, 4, 'OUTER', 'nested\\'hint')");

        // Safe boundary size:511 quotes expand to1022 bytes plus terminator.
        actions.clear();
        std::string quotes(511,'\''); logger.CharLog(1,2,3,4,"BOUNDARY",quotes.c_str());
        assert(actions=="ESLPU");
        std::string escaped; for(unsigned i=0;i<511;++i) escaped+="\\'";
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 1, 2, 3, 4, 'BOUNDARY', '"+escaped+"')");
        // No app lifecycle/query/escape wrapper is stubbed. Drain real SQLMsg queue.
        while(!logger.m_sql.m_queue_query.empty()) {
            delete logger.m_sql.m_queue_query.front();logger.m_sql.m_queue_query.pop();
        }
        active=0;current_logger=0;
    }
    assert(!LogManager::instance_ptr() && initialized==1 && destroyed==1);
    std::puts("CharLog passed: exact CHARACTER SQL, raw/escaped hints, uint32, postfix reread, shared buffer and real async queue");
}
