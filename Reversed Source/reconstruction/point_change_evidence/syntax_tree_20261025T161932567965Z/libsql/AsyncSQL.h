// Complete old SQL data/API with addressed lifecycle, connection, queue and worker behavior.
#ifndef MCNCC_SQL_ASYNCSQL_H
#define MCNCC_SQL_ASYNCSQL_H
#include <cstdio>
#include "../libthecore/include/typedef.h"
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <pthread.h>
#include <mysql/mysql.h>
#include "Semaphore.h"
typedef struct _SQLResult {
    MYSQL_RES* pSQLResult;
    int iNumRows,iAffectedRows,iInsertID;
    _SQLResult() : pSQLResult(0),iNumRows(0),iAffectedRows(0),iInsertID(0) {}
    ~_SQLResult() { if(pSQLResult) { mysql_free_result(pSQLResult); pSQLResult=0; } }
} SQLResult;
typedef struct _SQLMsg {
    MYSQL* m_pkSQL;
    int iID;
    std::string stQuery;
    std::vector<SQLResult*> vec_pkResult;
    unsigned int uiResultPos;
    void* pvUserData;
    bool bReturn;
    unsigned int uiSQLErrno;
    _SQLMsg() : m_pkSQL(0),iID(0),uiResultPos(0),pvUserData(0),bReturn(false),uiSQLErrno(0) {}
    ~_SQLMsg() {
        for(std::vector<SQLResult*>::iterator it=vec_pkResult.begin();it!=vec_pkResult.end();++it) delete *it;
        vec_pkResult.clear();
    }
    void Store() {
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
    bool Next() {
        if(uiResultPos+1>=vec_pkResult.size()) return false;
        ++uiResultPos; return true;
    }
} SQLMsg;
class CAsyncSQL {
public:
    CAsyncSQL();
    ~CAsyncSQL();
    bool Setup(const char* host,const char* user,const char* password,const char* db,const char* locale,bool noThread=false);
    void Quit();
    bool Setup(CAsyncSQL* source,bool noThread=false);
    bool Connect();
    bool QueryLocaleSet();
    void AsyncQuery(const char* query);
    void ReturnQuery(const char* query,void* userData);
    SQLMsg* DirectQuery(const char* query);
    DWORD CountQuery();
    DWORD CountResult();
    void PushResult(SQLMsg* message);
    bool PopResult(SQLMsg** message);
    void ChildLoop();
    int CountQueryFinished();
    void ResetQueryFinished();
    void DumpQueryQueue(FILE* output);
    int GetCopiedQueryCount();
    void ResetCopiedQueryCount();
    void AddCopiedQueryCount(int count);
    MYSQL* GetSQLHandle();
protected:
    void Destroy();
    void PushQuery(SQLMsg* message);
    bool PeekQuery(SQLMsg** message);
    bool PopQuery(int id);
    bool PeekQueryFromCopyQueue(SQLMsg** message);
    INT CopyQuery();
    bool PopQueryFromCopyQueue();
    MYSQL m_hDB;
    std::string m_stHost,m_stUser,m_stPassword,m_stDB,m_stLocale;
    int m_iMsgCount;
    int m_aiPipe[2];
    std::queue<SQLMsg*> m_queue_query;
    std::queue<SQLMsg*> m_queue_query_copy;
    std::map<int,SQLMsg*> m_map_kSQLMsgUnfinished;
    std::queue<SQLMsg*> m_queue_result;
    bool m_bEnd;
    pthread_t m_hThread;
    pthread_mutex_t* m_mtxQuery;
    pthread_mutex_t* m_mtxResult;
    CSemaphore m_sem;
    int m_iQueryFinished;
    unsigned long m_ulThreadID;
    bool m_bConnected;
    int m_iCopiedQuery;
};
#endif
