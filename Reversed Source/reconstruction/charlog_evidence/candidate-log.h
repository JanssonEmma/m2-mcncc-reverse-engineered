// Complete old logger data; nonvirtual logging API recovery is partial.
#ifndef MCNCC_GAME_LOG_H
#define MCNCC_GAME_LOG_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include "../../libsql/AsyncSQL.h"
class LogManager : public singleton<LogManager> {
public:
    LogManager();
    virtual ~LogManager();
    bool Connect(const char* host,const char* user,const char* password,const char* db);
    bool IsConnected();
    unsigned long EscapeString(void* to,const void* from,unsigned long length);
    void ItemLog(DWORD pid,DWORD x,DWORD y,DWORD itemID,const char* text,const char* hint);
    void Query(const char* format,...);
    void CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint);
private:
    CAsyncSQL m_sql;
    bool m_bIsConnect;
};
#endif
