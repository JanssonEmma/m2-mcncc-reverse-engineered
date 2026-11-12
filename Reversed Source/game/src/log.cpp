#include "log.h"
#include "config.h"
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
// STB_LOCAL __escape_hint: 1024 bytes, shared by old logger calls.
static char __escape_hint[1024];
LogManager::LogManager() : m_bIsConnect(false) {}
LogManager::~LogManager() {}
bool LogManager::Connect(const char* host,const char* user,const char* password,const char* db) {
    if(m_sql.Setup(host,user,password,db,g_stLocale.c_str(),false)) m_bIsConnect=true;
    return m_bIsConnect;
}
bool LogManager::IsConnected() { return m_bIsConnect; }
void LogManager::Query(const char* format,...) {
    char query[4096]; va_list args; va_start(args,format);
    std::vsnprintf(query,sizeof(query),format,args); va_end(args);
    m_sql.AsyncQuery(query);
}
unsigned long LogManager::EscapeString(void* to,const void* from,unsigned long length) {
    return mysql_real_escape_string(m_sql.GetSQLHandle(),static_cast<char*>(to),static_cast<const char*>(from),length);
}
void LogManager::ItemLog(DWORD pid,DWORD x,DWORD y,DWORD itemID,const char* text,const char* hint) {
    pid = static_cast<std::uint32_t>(pid);
    x = static_cast<std::uint32_t>(x);
    y = static_cast<std::uint32_t>(y);
    itemID = static_cast<std::uint32_t>(itemID);
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,itemID,text,hint);
    Query(format,get_table_postfix(),pid,x,y,itemID,text,__escape_hint);
}


//080785a0: exact old six-argument CHARACTER audit path.
void LogManager::CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint) {
    pid = static_cast<std::uint32_t>(pid);
    x = static_cast<std::uint32_t>(x);
    y = static_cast<std::uint32_t>(y);
    value = static_cast<std::uint32_t>(value);
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,value,text,hint);
    Query(format,get_table_postfix(),pid,x,y,value,text,__escape_hint);
}


// Original 080789f0.
void LogManager::GoldBarLog(DWORD pid, DWORD itemID, GOLDBAR_HOW how, const char* hint) {
    pid = static_cast<std::uint32_t>(pid);
    itemID = static_cast<std::uint32_t>(itemID);
    const char* label;
    switch (how) {
        case PERSONAL_SHOP_BUY: label = "'BUY'"; break;
        case PERSONAL_SHOP_SELL: label = "'SELL'"; break;
        case SHOP_BUY: label = "'SHOP_BUY'"; break;
        case SHOP_SELL: label = "'SHOP_SELL'"; break;
        case EXCHANGE_TAKE: label = "'EXCHANGE_TAKE'"; break;
        case EXCHANGE_GIVE: label = "'EXCHANGE_GIVE'"; break;
        case QUEST: label = "'QUEST'"; break;
        default: label = "''"; break;
    }
    const char* format = "INSERT DELAYED INTO goldlog%s (date, time, pid, what, how, hint) VALUES( CURDATE(), CURTIME(), %lu, %lu, %s, '%s');";
    sys_log(0, format, get_table_postfix(), pid, itemID, label, hint);
    Query(format, get_table_postfix(), pid, itemID, label, hint);
}

void LogManager::LoginLog(bool login, DWORD account, DWORD pid, BYTE level, BYTE job, DWORD playTime)
{
    account = static_cast<std::uint32_t>(account);
    pid = static_cast<std::uint32_t>(pid);
    playTime = static_cast<std::uint32_t>(playTime);
    const char* sql="INSERT DELAYED INTO loginlog%s (type, time, channel, account_id, pid, level, job, playtime) VALUES (%s, NOW(), %d, %lu, %lu, %d, %d, %lu)";
    const char* action=login ? "'LOGIN'" : "'LOGOUT'";
    sys_log(0,sql,get_table_postfix(),action,g_bChannel,account,pid,level,job,playTime);
    Query(sql,get_table_postfix(),action,g_bChannel,account,pid,level,job,playTime);
}

// 08078b70
void LogManager::HackLog(const char* reason, const char* login, const char* name, const char* ip)
{
    const char query[] = "INSERT INTO hack_log (time, login, name, ip, server, why) VALUES(NOW(), '%s', '%s', '%s', '%s', '%s')";
    sys_log(0, query, login, name, ip, g_stHostname.c_str(), reason);

    const auto escape = [this](const char* text) {
        const auto length = std::strlen(text);
        std::string result(length * 2 + 1, '\0');
        result.resize(EscapeString(&result[0], text, length));
        return result;
    };
    const auto account = escape(login);
    const auto character = escape(name);
    const auto host = escape(ip);
    const auto server = escape(g_stHostname.c_str());
    const auto why = escape(reason);
    Query(query, account.c_str(), character.c_str(), host.c_str(), server.c_str(), why.c_str());
}

// 080788c0
void LogManager::HackCRCLog(const char* reason, const char* login, const char* name, const char* ip, DWORD crc)
{
    const char query[] = "INSERT INTO hack_crc_log (time, login, name, ip, server, why, crc) "
                         "VALUES(NOW(), '%s', '%s', '%s', '%s', '%s', %lu)";
    const auto escape = [this](const char* text) {
        const auto length = std::strlen(text);
        std::string result(length * 2 + 1, '\0');
        result.resize(EscapeString(&result[0], text, length));
        return result;
    };
    sys_log(0, query, login, name, ip, g_stHostname.c_str(), reason, static_cast<unsigned long>(crc));
    const auto account = escape(login);
    const auto character = escape(name);
    const auto host = escape(ip);
    const auto server = escape(g_stHostname.c_str());
    const auto why = escape(reason);
    Query(query, account.c_str(), character.c_str(), host.c_str(), server.c_str(), why.c_str(),
          static_cast<unsigned long>(crc));
}

void LogManager::CubeLog(DWORD pid, DWORD x, DWORD y, DWORD vnum, DWORD id, int count, bool success)
{
    Query("INSERT DELAYED INTO cube%s (pid, time, x, y, item_vnum, item_uid, item_count, success) "
          "VALUES(%lu, NOW(), %lu, %lu, %lu, %lu, %d, %d)",
          get_table_postfix(), static_cast<unsigned long>(static_cast<std::uint32_t>(pid)),
          static_cast<unsigned long>(static_cast<std::uint32_t>(x)),
          static_cast<unsigned long>(static_cast<std::uint32_t>(y)),
          static_cast<unsigned long>(static_cast<std::uint32_t>(vnum)),
          static_cast<unsigned long>(static_cast<std::uint32_t>(id)),
          count, static_cast<int>(success));
}

void LogManager::SpeedHackLog(DWORD pid, DWORD x, DWORD y, int hackCount)
{
    Query("INSERT INTO speed_hack%s (pid, time, x, y, hack_count) VALUES(%u, NOW(), %u, %u, %d)",
          get_table_postfix(), static_cast<unsigned>(pid), static_cast<unsigned>(x),
          static_cast<unsigned>(y), hackCount);
}

void LogManager::MoneyLog(BYTE type, DWORD vnum, int gold)
{
    const int itemVnum = static_cast<int>(vnum);
    if (type < 1 || type > 8) {
        _sys_err("MoneyLog", 0x55, "type error : type %d vnum %d gold %d", type, itemVnum, gold);
        return;
    }

    const char* sql = "INSERT DELAYED INTO money_log%s VALUES (NOW(), %d, %d, %d)";
    sys_log(0, sql, get_table_postfix(), type, itemVnum, gold);
    Query(sql, get_table_postfix(), type, itemVnum, gold);
}

void LogManager::PCBangLoginLog(DWORD cafeId, const char* ip, DWORD pid, DWORD playTime)
{
    cafeId = static_cast<std::uint32_t>(cafeId);
    pid = static_cast<std::uint32_t>(pid);
    playTime = static_cast<std::uint32_t>(playTime);
    const char* query = "INSERT INTO pcbang_loginlog (time, pcbang_id, ip, pid, play_time) VALUES (NOW(), %lu, '%s', %lu, %lu);";
    sys_log(0, query, cafeId, ip, pid, playTime);
    Query(query, cafeId, ip, pid, playTime);
}
