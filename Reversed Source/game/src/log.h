// Complete old logger data; nonvirtual logging API recovery is partial.
#ifndef MCNCC_GAME_LOG_H
#define MCNCC_GAME_LOG_H
#include "../../common/singleton.h"
#include "../../libthecore/include/typedef.h"
#include "../../libsql/AsyncSQL.h"
enum GOLDBAR_HOW {
    PERSONAL_SHOP_BUY = 1, PERSONAL_SHOP_SELL, SHOP_BUY, SHOP_SELL,
    EXCHANGE_TAKE, EXCHANGE_GIVE, QUEST
};

class LogManager : public singleton<LogManager> {
public:
    LogManager();
    virtual ~LogManager();
    bool Connect(const char* host,const char* user,const char* password,const char* db);
    bool IsConnected();
    unsigned long EscapeString(void* to,const void* from,unsigned long length);
    void ItemLog(DWORD pid,DWORD x,DWORD y,DWORD itemID,const char* text,const char* hint);
    void CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint);
    void LoginLog(bool login, DWORD account, DWORD pid, BYTE level, BYTE job, DWORD playTime);
    void HackLog(const char* reason, const char* login, const char* name, const char* ip);
    void HackCRCLog(const char* reason, const char* login, const char* name, const char* ip, DWORD crc);
    void SpeedHackLog(DWORD pid, DWORD x, DWORD y, int hackCount);
    void MoneyLog(BYTE type, DWORD vnum, int gold);
    void CubeLog(DWORD pid, DWORD x, DWORD y, DWORD vnum, DWORD id, int count, bool success);
    void PCBangLoginLog(DWORD cafeId, const char* ip, DWORD pid, DWORD playTime);
    void Query(const char* format,...);
    void GoldBarLog(DWORD pid, DWORD itemID, GOLDBAR_HOW how, const char* hint);
private:
    CAsyncSQL m_sql;
    bool m_bIsConnect;
};
#endif
