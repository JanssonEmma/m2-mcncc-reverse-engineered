#ifndef MCNCC_GAME_DB_H
#define MCNCC_GAME_DB_H
#include <map>
#include <vector>
#include <string>
#include "../../common/singleton.h"
#include "../../libsql/AsyncSQL.h"
#include "any_function.h"
#include <cstdarg>

enum {
    QUERY_TYPE_RETURN = 1,
    QUERY_TYPE_FUNCTION = 2,
    QUERY_TYPE_AFTER_FUNCTION = 3
};

class CQueryInfo {
public:
    int iQueryType;
};

class CReturnQueryInfo : public CQueryInfo {
public:
    int iType;
    DWORD dwIdent;
    void* pvData;
};

struct THighscoreRegisterQueryInfo {
    char board[21];
    DWORD pid;
    int value;
    bool descending;
};

class CFuncQueryInfo : public CQueryInfo {
public:
    any_function f;
};

class CFuncAfterQueryInfo : public CQueryInfo {
public:
    any_void_function f;
};

class CLoginData;
class DESC;
typedef struct SUseTime {
    DWORD dwLoginKey;
    char szLogin[31];
    BYTE bBillType;
    DWORD dwUseSec;
    char szIP[16];
} TUseTime;

// Full original declaration API from db.cpp DWARF; no method bodies.
// Original DBManager sizeof=2372, CAsyncSQL sizeof=1152 (old toolchain).
class DBManager : public singleton<DBManager> {
public:
    // Implicit copy constructor recorded in DWARF.
    DBManager();
    virtual ~DBManager(); // DWARF hidden deleting-destructor int is ABI-only.
    bool IsConnected();
    bool Connect(const char*, const char*, const char*, const char*);
    void Query(const char*, ...);
    SQLMsg* DirectQuery(const char*, ...);
    void ReturnQuery(int, long unsigned int, void*, const char*, ...);
    template<class Functor> void FuncQuery(Functor callback, const char* format, ...);
    template<class Functor> void FuncAfterQuery(Functor callback, const char* format, ...);
    long unsigned int EscapeString(void*, const void*, long unsigned int);
    void Process();
    void AnalyzeReturnQuery(SQLMsg*);
    void SendMoneyLog(unsigned char, long unsigned int, int);
    void LoginPrepare(unsigned char, long unsigned int, long int, DESC*, DWORD*, int*);
    void SendAuthLogin(DESC*);
    void SendLoginPing(const char*);
    void InsertLoginData(CLoginData*);
    void DeleteLoginData(CLoginData*);
    CLoginData* GetLoginData(long unsigned int);
    void SetBilling(long unsigned int, bool, bool);
    void PushBilling(CLoginData*);
    void FlushBilling(bool);
    void CheckBilling();
    void StopAllBilling();
    DWORD CountQuery() { return m_sql.CountQuery(); }
    DWORD CountQueryResult() { return m_sql.CountResult(); }
    void ResetQueryResult() { m_sql.ResetQueryFinished(); }
    void RequestBlockException(const char*, int);
    void LoadDBString();
    const std::string& GetDBString(const std::string&);
    const std::vector<std::string>& GetGreetMessage();
private:
    SQLMsg* PopResult();
    CAsyncSQL m_sql;
    CAsyncSQL m_sql_direct;
    bool m_bIsConnect;
    std::map<std::string,std::string> m_map_dbstring;
    std::vector<std::string> m_vec_GreetMessage;
    std::map<DWORD,CLoginData*> m_map_pkLoginData;
    std::map<std::string,CLoginData*> mapLDBilling;
    std::vector<TUseTime> m_vec_kUseTime;
};

// 080e1d70 is the original messenger callback instantiation.
template<class Functor>
void DBManager::FuncQuery(Functor callback, const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);

    CFuncQueryInfo* info = new CFuncQueryInfo;
    info->iQueryType = QUERY_TYPE_FUNCTION;
    info->f = callback;
    m_sql.ReturnQuery(query, info);
}

template<class Functor>
void DBManager::FuncAfterQuery(Functor callback, const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);

    CFuncAfterQueryInfo* info = new CFuncAfterQueryInfo;
    info->iQueryType = QUERY_TYPE_AFTER_FUNCTION;
    info->f = callback;
    m_sql.ReturnQuery(query, info);
}

class CHARACTER;
class CItem;
void VCardUse(CHARACTER* owner, CHARACTER* taker, CItem* item);
class CLoginData;
void SendBillingExpire(const char* login, BYTE type, int seconds, CLoginData* data);

class CAccountDB : public singleton<CAccountDB> {
public:
    CAccountDB();
    virtual ~CAccountDB();
    bool Connect(const char* host, const char* user, const char* password, const char* database);
    bool IsConnected();
    SQLMsg* DirectQuery(const char* query);
    void SetLocale(const std::string& locale);
private:
    CAsyncSQL2 m_sql;
    bool m_IsConnect;
};

#endif
