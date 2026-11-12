// Complete old SQL data/API with addressed lifecycle, connection, queue and worker behavior.
#ifndef MCNCC_SQL_ASYNCSQL_H
#define MCNCC_SQL_ASYNCSQL_H
#include "../libthecore/include/typedef.h"
#include "../libthecore/include/log.h"

#include <cstdio>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <cstdlib>

#ifndef _WIN32
#include <pthread.h>
#include <mysql.h>
#include <errmsg.h>
#include <mysqld_error.h>
#else
#include "../../extern/include/mysql/mysql.h"
#include "../../extern/include/mysql/errmsg.h"
#include "../../extern/include/mysql/mysqld_error.h"
#endif

#include "Semaphore.h"

#include "../common/features.h"

using AsyncSQLMutex = std::mutex;

typedef struct _SQLResult
{
    MYSQL_RES* pSQLResult;
    int iNumRows,iAffectedRows,iInsertID;
    _SQLResult() : pSQLResult(0),iNumRows(0),iAffectedRows(0),iInsertID(0) {}
    ~_SQLResult() { if(pSQLResult) { mysql_free_result(pSQLResult); pSQLResult=0; } }
} SQLResult;

typedef struct _SQLMsg
{
    MYSQL* m_pkSQL;
    int iID;
    std::string stQuery;
    std::vector<SQLResult*> vec_pkResult;
    unsigned int uiResultPos;
    void* pvUserData;
    bool bReturn;
    unsigned int uiSQLErrno;
    _SQLMsg() : m_pkSQL(0),iID(0),uiResultPos(0),pvUserData(0),bReturn(false),uiSQLErrno(0) {}
    ~_SQLMsg()
    {
        for(std::vector<SQLResult*>::iterator it=vec_pkResult.begin();it!=vec_pkResult.end();++it) delete *it;
        vec_pkResult.clear();
    }
    void Store()
    {
        do {
            SQLResult* result=new SQLResult;
            result->pSQLResult=mysql_store_result(m_pkSQL);
            result->iInsertID=static_cast<int>(static_cast<unsigned int>(mysql_insert_id(m_pkSQL)));
            result->iAffectedRows=static_cast<int>(static_cast<unsigned int>(mysql_affected_rows(m_pkSQL)));
            result->iNumRows=result->pSQLResult ? static_cast<int>(static_cast<unsigned int>(mysql_num_rows(result->pSQLResult))) : 0;
            vec_pkResult.push_back(result);
        } while(mysql_next_result(m_pkSQL)==0);
    }
    SQLResult* Get() { return uiResultPos<vec_pkResult.size() ? vec_pkResult[uiResultPos] : 0; }
    bool Next()
    {
        if(uiResultPos+1>=vec_pkResult.size()) return false;
        ++uiResultPos; return true;
    }
} SQLMsg;

class CAsyncSQL
{
    public:
        CAsyncSQL();
        ~CAsyncSQL();

        bool    Setup(const char* host, const char* user, const char* password, const char* db, const char* locale, bool noThread = false, unsigned int port = 0);
        void    Quit();
        bool    Setup(CAsyncSQL* source,bool noThread=false);
        bool    Connect();
        bool    QueryLocaleSet();
        void    AsyncQuery(const char* query);
        void    ReturnQuery(const char* query,void* userData);
        SQLMsg* DirectQuery(const char* query);
        DWORD   CountQuery();
        DWORD   CountResult();
        void    PushResult(SQLMsg* message);
        bool    PopResult(SQLMsg** message);
        void    ChildLoop();
        int     CountQueryFinished();
        void    ResetQueryFinished();
        void    DumpQueryQueue(FILE* output);
        int     GetCopiedQueryCount();
        void    ResetCopiedQueryCount();
        void    AddCopiedQueryCount(int count);
        MYSQL*  GetSQLHandle();
    protected:
        void    Destroy();
        void    PushQuery(SQLMsg* message);
        bool    PeekQuery(SQLMsg** message);
        bool    PopQuery(int id);
        bool    PeekQueryFromCopyQueue(SQLMsg** message);
        INT     CopyQuery();
        bool    PopQueryFromCopyQueue();
        MYSQL   m_hDB;
        std::string m_stHost,m_stUser,m_stPassword,m_stDB,m_stLocale;
        int     m_iMsgCount;
        int     m_aiPipe[2];
        std::queue<SQLMsg*> m_queue_query;
        std::queue<SQLMsg*> m_queue_query_copy;
        std::map<int,SQLMsg*> m_map_kSQLMsgUnfinished;
        std::queue<SQLMsg*> m_queue_result;

        std::atomic<bool> m_bEnd;

    #ifdef _WIN32
        HANDLE m_hThread;
    #else
        pthread_t m_hThread;
    #endif

        AsyncSQLMutex* m_mtxQuery;
        AsyncSQLMutex* m_mtxResult;
        CSemaphore m_sem;

        std::atomic<int> m_iQueryFinished;

        unsigned long m_ulThreadID;
        bool m_bConnected;

        std::atomic<int> m_iCopiedQuery;

        bool RequestLocale(const std::string& locale);

    private:
    #ifdef _WIN32
        friend unsigned int __stdcall AsyncSQLThread(void*);
    #else
        friend void* AsyncSQLThread(void*);
    #endif
        void CloseClientHandle();
        bool ApplyRequestedLocale();
        bool ExecuteQuery(SQLMsg& message);
        void DrainQueryResults();
        unsigned int m_port = 0;
        bool m_clientInitialized = false;
        bool m_threaded = false;
        std::mutex m_localeMutex;
        std::string m_requestedLocale;
        bool m_hasRequestedLocale = false;
};

class CAsyncSQL2 : public CAsyncSQL
{
    public:
        bool SetLocale(const std::string& locale);
};

#endif
