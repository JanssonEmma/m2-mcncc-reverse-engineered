#include "log.h"
#include "config.h"
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include <cstdarg>
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
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('ITEM', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,itemID,text,hint);
    Query(format,get_table_postfix(),pid,x,y,itemID,text,__escape_hint);
}

//080785a0: exact old six-argument CHARACTER audit path.
void LogManager::CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint) {
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,value,text,hint);
    Query(format,get_table_postfix(),pid,x,y,value,text,__escape_hint);
}
