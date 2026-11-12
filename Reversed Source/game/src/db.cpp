#include <cstdlib>
#include <climits>
#include <cstdio>
#include "matrix_for_runup.h"
#include "input.h"
#include "../../libthecore/include/memcpy.h"
#include "login_data.h"
#include "packet.h"
#include "p2p.h"
#include "../../libthecore/include/utils.h"
#include "locale_service.h"
#include "db.h"
#include "desc_client.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "item.h"
#include "item_manager.h"
#include "locale.hpp"
#include "log.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <ctime>
#include <memory>
#include <sstream>
#include "desc_manager.h"
#include "../../common/tables.h"

// Actual old lifecycle, not a test fixture substitute. All members/base initialize
// and destruct normally in declaration order; old exports show no custom work.
DBManager::DBManager() : m_bIsConnect(false) {}
DBManager::~DBManager() {}

// DBManager08110620. Full DBManager declaration/data must accompany this method.
void DBManager::SendMoneyLog(BYTE type,DWORD vnum,int gold)
{
    if (gold!=0) {
        TPacketMoneyLog p;
        p.type=type;
        p.gold=gold;
        p.vnum=vnum;
        db_clientdesc->DBPacket(50,0,&p,9);
    }
}


// 08110670
void VCardUse(CHARACTER* owner, CHARACTER* taker, CItem* item)
{
    TPacketGDVCard packet;
    packet.dwID = item->GetSockets()[0];
    std::strncpy(packet.szSellCharacter, owner->GetName(), 24);
    packet.szSellCharacter[24] = '\0';
    std::strncpy(packet.szSellAccount, owner->GetDesc()->GetAccountTable().login, 30);
    packet.szSellAccount[30] = '\0';
    std::strncpy(packet.szBuyCharacter, taker->GetName(), 24);
    packet.szBuyCharacter[24] = '\0';
    std::strncpy(packet.szBuyAccount, taker->GetDesc()->GetAccountTable().login, 30);
    packet.szBuyAccount[30] = '\0';
    db_clientdesc->DBPacket(105, 0, &packet, sizeof(packet));

    const long seconds = item->GetSockets()[1];
    const long cardNumber = item->GetSockets()[0];
    const char* format = locale_find("%d\272\320\300\307 \260\341\301\246\275\303\260\243\300\314 "
        "\303\337\260\241 \265\307\276\372\275\300\264\317\264\331. (\260\341\301\246\271\370\310\243 %d)", g_iUseLocale);
    taker->ChatPacket(1, format, static_cast<int>(seconds / 60), static_cast<int>(cardNumber));

    const char* takerIP = taker->GetDesc()->GetHostName();
    const char* takerName = taker->GetName();
    const char* ownerIP = owner->GetDesc()->GetHostName();
    const char* ownerName = owner->GetName();
    LogManager::instance().Query(
        "INSERT DELAYED INTO vcard_log (vcard_id, x, y, hostname, giver_name, giver_ip, taker_name, taker_ip) "
        "VALUES(%lu, %lu, %lu, '%s', '%s', '%s', '%s', '%s')",
        static_cast<unsigned long>(packet.dwID), static_cast<unsigned long>(static_cast<std::uint32_t>(taker->GetX())), static_cast<unsigned long>(static_cast<std::uint32_t>(taker->GetY())),
        g_stHostname.c_str(), ownerName, ownerIP, takerName, takerIP);

    ITEM_MANAGER::instance().RemoveItem(item, 0);
    takerName = taker->GetName();
    ownerName = owner->GetName();
    sys_log(0, "VCARD_TAKE: %lu %s -> %s", static_cast<unsigned long>(packet.dwID), ownerName, takerName);
}

// 0810c760
bool DBManager::Connect(const char* host, const char* user, const char* pwd, const char* db)
{
    if (m_sql.Setup(host, user, pwd, db, g_stLocale.c_str(), false))
        m_bIsConnect = true;
    if (!m_sql_direct.Setup(host, user, pwd, db, g_stLocale.c_str(), true))
        _sys_err("Connect", 0x66, "cannot open direct sql connection to host %s", host);
    if (m_bIsConnect && !g_bAuthServer)
        LoadDBString();
    return m_bIsConnect;
}

// 0810c870
void DBManager::Query(const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);
    m_sql.AsyncQuery(query);
}

// 0810c8c0
SQLMsg* DBManager::DirectQuery(const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);
    return m_sql_direct.DirectQuery(query);
}

// 0810c910, 0810c920
bool DBManager::IsConnected() { return m_bIsConnect; }

unsigned long DBManager::EscapeString(void* output, const void* input, unsigned long length)
{
    return mysql_real_escape_string(m_sql_direct.GetSQLHandle(), static_cast<char*>(output),
                                    static_cast<const char*>(input), length);
}

// 0810c950
void DBManager::ReturnQuery(int type, DWORD ident, void* data, const char* format, ...)
{
    char query[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(query, sizeof(query), format, args);
    va_end(args);
    CReturnQueryInfo* info = new CReturnQueryInfo;
    info->iQueryType = QUERY_TYPE_RETURN;
    info->iType = type;
    info->dwIdent = ident;
    info->pvData = data;
    m_sql.ReturnQuery(query, info);
}

// 0810c9d0
SQLMsg* DBManager::PopResult()
{
    SQLMsg* message;
    return m_sql.PopResult(&message) ? message : 0;
}

// 0810ca00. Original callback branches invoke their holder without freeing it.
void DBManager::Process()
{
    while (SQLMsg* message = PopResult()) {
        CQueryInfo* info = static_cast<CQueryInfo*>(message->pvUserData);
        switch (info->iQueryType) {
        case QUERY_TYPE_RETURN:
            AnalyzeReturnQuery(message);
            break;
        case QUERY_TYPE_FUNCTION:
            static_cast<CFuncQueryInfo*>(info)->f(message);
            break;
        case QUERY_TYPE_AFTER_FUNCTION:
            static_cast<CFuncAfterQueryInfo*>(info)->f();
            break;
        }
        delete message;
    }
}

// 08110540, 08110580, 08110610
void DBManager::LoadDBString()
{
    ReturnQuery(1, 0, 0, "SELECT name, text FROM string%s", get_table_postfix());
}

const std::string& DBManager::GetDBString(const std::string& key)
{
    static const std::string empty;
    const std::map<std::string, std::string>::const_iterator it = m_map_dbstring.find(key);
    return it != m_map_dbstring.end() ? it->second : empty;
}

const std::vector<std::string>& DBManager::GetGreetMessage()
{
    return m_vec_GreetMessage;
}

// 0810cb50
CLoginData* DBManager::GetLoginData(DWORD key)
{
    std::map<DWORD, CLoginData*>::iterator login = m_map_pkLoginData.find(key);
    return login != m_map_pkLoginData.end() ? login->second : 0;
}

// 0810cba0
void DBManager::InsertLoginData(CLoginData* data)
{
    m_map_pkLoginData.insert(std::make_pair(data->GetKey(), data));
}

// 0810cc10. Lookup owns the registered pointer, which can differ from the argument.
void DBManager::DeleteLoginData(CLoginData* data)
{
    std::map<DWORD, CLoginData*>::iterator login = m_map_pkLoginData.find(data->GetKey());
    if (login == m_map_pkLoginData.end())
        return;
    sys_log(0, "DeleteLoginData %s %p", data->GetLogin(), static_cast<void*>(data));
    mapLDBilling.erase(data->GetLogin());
    delete login->second;
    m_map_pkLoginData.erase(login);
}

// 0810d870
void DBManager::SendLoginPing(const char* login)
{
    TPacketGGLoginPing packet;
    packet.bHeader = 20;
    std::strncpy(packet.szLogin, login, 30);
    packet.szLogin[30] = '\0';
    if (g_pkAuthMasterDesc)
        g_pkAuthMasterDesc->Packet(&packet, sizeof(packet));
    else
        P2P_MANAGER::instance().Send(&packet, sizeof(packet));
}

// 0810cd80
void DBManager::SetBilling(DWORD key, bool enabled, bool skipPush)
{
    CLoginData* data = GetLoginData(key);
    if (!data) {
        _sys_err("SetBilling", 0xee, "cannot find login key %lu", key);
        return;
    }
    std::map<std::string, CLoginData*>::iterator billing = mapLDBilling.find(data->GetLogin());
    if (billing != mapLDBilling.end() && billing->second != data)
        DeleteLoginData(billing->second);
    mapLDBilling.insert(std::make_pair(std::string(data->GetLogin()), data));
    if (data->IsBilling() && !enabled && !skipPush)
        PushBilling(data);
    SendLoginPing(data->GetLogin());
    data->SetBilling(enabled);
}

// 0810cfa0
void DBManager::PushBilling(CLoginData* data)
{
    TUseTime use;
    const unsigned int now = static_cast<unsigned int>(get_dword_time());
    use.dwUseSec = (now - static_cast<unsigned int>(data->GetLogonTime())) / 1000u;
    if (!use.dwUseSec)
        return;
    data->SetLogonTime();
    const unsigned int bits = static_cast<unsigned int>(data->GetRemainSecs()) -
                              static_cast<unsigned int>(use.dwUseSec);
    int remain;
    std::memcpy(&remain, &bits, sizeof(remain));
    data->SetRemainSecs(MAX(0, remain));
    use.dwLoginKey = data->GetKey();
    use.bBillType = data->GetBillType();
    sys_log(0, "BILLING: PUSH %s %lu type %u", data->GetLogin(), use.dwUseSec,
            static_cast<unsigned int>(use.bBillType));
    if (use.bBillType == 1 || use.bBillType == 3 || use.bBillType == 4)
        snprintf(use.szLogin, 30, "%lu", data->GetBillID());
    else
        std::strncpy(use.szLogin, data->GetLogin(), 30);
    use.szLogin[30] = '\0';
    std::strncpy(use.szIP, data->GetIP(), 15);
    use.szIP[15] = '\0';
    m_vec_kUseTime.push_back(use);
}

// 08110960
void DBManager::StopAllBilling()
{
    for (std::map<DWORD, CLoginData*>::iterator login = m_map_pkLoginData.begin();
         login != m_map_pkLoginData.end(); ++login)
        SetBilling(login->first, false, false);
}

// 081109c0
void DBManager::RequestBlockException(const char* login, int command)
{
    TPacketBlockException packet;
    packet.cmd = static_cast<BYTE>(command);
    std::strncpy(packet.login, login, 30);
    packet.login[30] = '\0';
    db_clientdesc->DBPacket(128, 0, &packet, sizeof(packet));
}

// 0810e060
// 0810de60
bool GetGameTimeIP(MYSQL_RES* result, BYTE& billType, DWORD& billId, int& seconds, const char* ip)
{
    if (!result)
        return true;
    if (mysql_num_fields(result) < 6)
        return false;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row || !row[0] || !row[1] || !row[2] || !row[3] || !row[4] || !row[5])
        return false;

    billId = static_cast<DWORD>(std::atoi(row[0]));
    const int ipStart = std::atoi(row[1]);
    const int ipEnd = std::atoi(row[2]);
    const int type = std::atoi(row[3]);
    seconds = std::atoi(row[4]);
    const int daySeconds = std::atoi(row[5]);
    billType = 0;

    char address[16] = {};
    if (ip)
        std::strncpy(address, ip, sizeof(address) - 1);
    const char* suffix = std::strrchr(address, '.');
    if (!suffix || !suffix[1])
        return false;
    const int lastOctet = std::atoi(suffix + 1);
    if (lastOctet < ipStart || lastOctet > ipEnd)
        return true;

    if (type == -1)
        return false;
    if (type == 0) {
        billType = 1;
    } else if (daySeconds > 0) {
        billType = 4;
        seconds = daySeconds;
    } else if (seconds > 0) {
        billType = 3;
    }
    return true;
}

// 0810df90
bool GetGameTime(MYSQL_RES* result, BYTE& billType, int& seconds)
{
    if (!result)
        return true;
    if (mysql_num_fields(result) < 3)
        return false;
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row || !row[0] || !row[1] || !row[2])
        return false;

    sys_log(1, "GetGameTime %p %p %p", static_cast<void*>(row[0]),
            static_cast<void*>(row[1]), static_cast<void*>(row[2]));
    const int type = std::atoi(row[0]);
    seconds = std::atoi(row[1]);
    const int daySeconds = std::atoi(row[2]);
    billType = 0;
    if (type == -1)
        return false;
    if (type == 0) {
        billType = 2;
    } else if (daySeconds > 0) {
        billType = 6;
        seconds = daySeconds;
    } else if (seconds > 0) {
        billType = 5;
    }
    if (!g_bBilling)
        billType = 2;
    return true;
}

void SendBillingExpire(const char* login, BYTE type, int seconds, CLoginData* data)
{
    TPacketBillingExpire packet;
    std::strncpy(packet.szLogin, login, 30);
    packet.bBillType = type;
    packet.szLogin[30] = '\0';
    packet.dwRemainSeconds = MAX(0, seconds);
    db_clientdesc->DBPacket(104, 0, &packet, sizeof(packet));
    sys_log(0, "BILLING: EXPIRE %s type %d sec %d ptr %p", login,
            static_cast<int>(type), seconds, static_cast<void*>(data));
}

// 0810d710
void DBManager::CheckBilling()
{
    std::vector<std::uint32_t> keys(1, 0);
    std::map<DWORD, CLoginData*>::iterator next = m_map_pkLoginData.begin();
    while (next != m_map_pkLoginData.end()) {
        CLoginData* data = (next++)->second;
        if (data->IsBilling()) {
            sys_log(0, "BILLING: CHECK %lu", data->GetKey());
            keys.push_back(static_cast<std::uint32_t>(data->GetKey()));
        }
    }
    keys[0] = static_cast<std::uint32_t>(keys.size() - 1);
    db_clientdesc->DBPacket(106, 0, keys.data(), keys.size() * sizeof(keys[0]));
}

// 0810d920
void DBManager::SendAuthLogin(DESC* desc)
{
    CLoginData* data = GetLoginData(desc->GetLoginKey());
    if (!data)
        return;
    TAccountTable& account = desc->GetAccountTable();
    TPacketGDAuthLogin packet{};
    packet.dwID = static_cast<std::uint32_t>(account.id);
    trim_and_lower(packet.szLogin, account.login, 30);
    std::strncpy(packet.szSocialID, account.social_id, 18);
    packet.szSocialID[18] = '\0';
    packet.dwLoginKey = static_cast<std::uint32_t>(desc->GetLoginKey());
    packet.bBillType = data->GetBillType();
    packet.dwBillID = static_cast<std::uint32_t>(data->GetBillID());
    thecore_memcpy(packet.iPremiumTimes, data->GetPremiumPtr(), sizeof(packet.iPremiumTimes));
    const DWORD* clientKey = data->GetClientKey();
    for (int i = 0; i < 4; ++i)
        packet.adwClientKey[i] = static_cast<std::uint32_t>(clientKey[i]);
    db_clientdesc->DBPacket(100, desc->GetHandle(), &packet, sizeof(packet));
    sys_log(0, "SendAuthLogin %s key %lu", packet.szLogin, static_cast<unsigned long>(packet.dwID));
    SendLoginPing(account.login);
}

// 0810d140
void DBManager::FlushBilling(bool force)
{
    if (force) {
        std::map<DWORD, CLoginData*>::iterator next = m_map_pkLoginData.begin();
        while (next != m_map_pkLoginData.end()) {
            CLoginData* data = (next++)->second;
            if (data->IsBilling())
                PushBilling(data);
        }
    }

    if (!m_vec_kUseTime.empty()) {
        DWORD count = 0;
        std::vector<TUseTime>::iterator next = m_vec_kUseTime.begin();
        while (next != m_vec_kUseTime.end()) {
            TUseTime& use = *next++;
            if (!g_bBilling) {
                ++count;
                continue;
            }

            Query("INSERT GameTimeLog (login, type, logon_time, logout_time, use_time, ip, server) "
                  "VALUES('%s', %u, DATE_SUB(NOW(), INTERVAL %lu SECOND), NOW(), %lu, '%s', '%s')",
                  use.szLogin, static_cast<unsigned int>(use.bBillType), use.dwUseSec, use.dwUseSec,
                  use.szIP, g_stHostname.c_str());

            const char* remainingQuery = 0;
            switch (use.bBillType) {
            case 3:
                Query("UPDATE GameTimeIP SET LimitTime=LimitTime-%lu WHERE ipid=%s",
                      use.dwUseSec, use.szLogin);
                // IP time and IP day share the remaining-time lookup.
            case 4:
                remainingQuery = "SELECT UNIX_TIMESTAMP(LimitDt)-UNIX_TIMESTAMP(NOW()),LimitTime "
                                 "FROM GameTimeIP WHERE ipid=%s";
                break;
            case 5:
                Query("UPDATE GameTime SET LimitTime=LimitTime-%lu WHERE UserID='%s'",
                      use.dwUseSec, use.szLogin);
                // Account time and account day share the remaining-time lookup.
            case 6:
                remainingQuery = "SELECT UNIX_TIMESTAMP(LimitDt)-UNIX_TIMESTAMP(NOW()),LimitTime "
                                 "FROM GameTime WHERE UserID='%s'";
                break;
            }
            if (!force) {
                if (remainingQuery) {
                    TUseTime* copy = new TUseTime;
                    std::memcpy(copy, &use, sizeof(*copy));
                    ReturnQuery(8, 0, copy, remainingQuery, use.szLogin);
                }
                if (++count >= 1000)
                    break;
            }
        }

        if (count < m_vec_kUseTime.size()) {
            const std::size_t remaining = m_vec_kUseTime.size() - count;
            std::memmove(&m_vec_kUseTime[0], &m_vec_kUseTime[count], remaining * sizeof(TUseTime));
            m_vec_kUseTime.resize(remaining);
            sys_log(0, "FLUSH_USE_TIME: count %lu", count);
        } else {
            m_vec_kUseTime.clear();
        }
    }

    if (m_vec_kUseTime.size() >= 10240)
        return;

    const unsigned int now = static_cast<unsigned int>(get_dword_time());
    std::map<DWORD, CLoginData*>::iterator next = m_map_pkLoginData.begin();
    while (next != m_map_pkLoginData.end()) {
        CLoginData* data = (next++)->second;
        if (!data->IsBilling())
            continue;
        const BYTE type = data->GetBillType();
        if (type < 3 || type >= 7)
            continue;

        const long remaining = data->GetRemainSecs();
        const DWORD connected = (now - static_cast<unsigned int>(data->GetLogonTime())) / 1000u;
        if (remaining < 0) {
            if (connected % 10 == 0)
                SendBillingExpire(data->GetLogin(), 6, 0, data);
        } else if (remaining <= 600) {
            if (connected >= 60) {
                sys_log(0, "BILLING 1 %s remain %ld connected secs %lu",
                        data->GetLogin(), data->GetRemainSecs(), connected);
                PushBilling(data);
            }
        } else if (connected > static_cast<DWORD>(data->GetRemainSecs() - 600) || connected >= 600) {
            sys_log(0, "BILLING 2 %s remain %ld connected secs %lu",
                    data->GetLogin(), data->GetRemainSecs(), connected);
            PushBilling(data);
        }
    }
}

void MatrixCardRndCoordinate(unsigned long& rows, unsigned long& cols);
// 0810b710, original db.cpp line 26.
BOOL CheckTeleBlock(int account)
{
    char query[1024];
    std::sprintf(query, "select tele_block FROM `tele_block` WHERE account_id='%d'", account);
    SQLMsg* message = DBManager::instance().DirectQuery(query);
    if (!message) {
        sys_log(0, "cannot get the tele_block");
        return -1;
    }
    if (message->Get()->iNumRows != 0) {
        MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
        // The original returns directly from this branch, retaining the SQL message.
        return std::atoi(row[0]) == 1;
    }
    std::puts(query);
    sys_log(0, "[Tele_block]DirectQuery failed( %s )", query);
    delete message;
    return -1;
}

// 0810b910, original db.cpp line 58.
bool CheckPasspod(const char* login)
{
    char query[1024];
    std::sprintf(query, "select  ID from passpod where Login='%s'", login);
    SQLMsg* message = DBManager::instance().DirectQuery(query);
    if (!message) {
        sys_log(0, "cannot get the PASSPOD");
        return false;
    }
    const bool found = message->Get()->iNumRows != 0;
    if (!found) {
        std::puts(query);
        sys_log(0, "[PASSPOD]DirectQuery failed( %s )", query);
    }
    delete message;
    return found;
}

// 0810da80
void DBManager::LoginPrepare(BYTE billType, DWORD billId, long remainSeconds, DESC* desc,
                             DWORD* clientKey, int* premiumTimes)
{
    CLoginData* data = new CLoginData;
    data->SetKey(desc->GetLoginKey());
    TAccountTable& account = desc->GetAccountTable();
    const char* login = account.login;
    data->SetLogin(login);
    data->SetBillType(billType);
    data->SetBillID(billId);
    data->SetRemainSecs(remainSeconds);
    data->SetIP(desc->GetHostName());
    data->SetClientKey(clientKey);
    if (premiumTimes)
        data->SetPremium(premiumTimes);
    InsertLoginData(data);
    desc->SetPasspodPending(false);

    if (*desc->m_stMatrixCode.c_str()) {
        unsigned long rows = 0;
        unsigned long cols = 0;
        MatrixCardRndCoordinate(rows, cols);
        desc->SetMatrixCardRowsAndColumns(rows, cols);
        TPacketGCMatrixCard packet;
        packet.bHeader = 116;
        packet.dwRows = static_cast<std::uint32_t>(rows);
        packet.dwCols = static_cast<std::uint32_t>(cols);
        desc->Packet(&packet, sizeof(packet));
        sys_log(0, "MATRIX_QUERY: %s %c%d %c%d %c%d %c%d %s", login,
                static_cast<int>((rows >> 24) + 'A'), static_cast<int>((cols >> 24) + 1),
                static_cast<int>(((rows >> 16) & 255) + 'A'), static_cast<int>(((cols >> 16) & 255) + 1),
                static_cast<int>(((rows >> 8) & 255) + 'A'), static_cast<int>(((cols >> 8) & 255) + 1),
                static_cast<int>((rows & 255) + 'A'), static_cast<int>((cols & 255) + 1),
                desc->m_stMatrixCode.c_str());
        return;
    }

    if (LC_IsHongKong()) {
        if (!CheckTeleBlock(account.id)) {
            LoginFailure(desc, "NOTELE");
            desc->SetPhase(0);
            sys_log(0, "Tele_Block == 0 %s ", login);
            return;
        }
        if (test_server)
            sys_log(0, "matrix %s", login);

        char cardId[32];
        char quiz[9] = {};
        if (MatrixDBManager::instance().GainMatrix(login, cardId, quiz)) {
            desc->m_stMatrixCardID.assign(cardId);
            desc->m_stMatrixCode.assign(quiz);
            desc->SendRunupMatrixizPacket(quiz);
            if (test_server)
                sys_log(0, "matrix send");
            return;
        }
    } else if (LC_IsNewCIBN() && !g_bNoPasspod && CheckPasspod(login)) {
        desc->SetPasspodPending(true);
        BYTE header = 202;
        desc->Packet(&header, sizeof(header));
        sys_log(0, "%s request passpod", login);
        return;
    }
    SendAuthLogin(desc);
}

CAccountDB::CAccountDB() : m_IsConnect(false) {}
CAccountDB::~CAccountDB() {}

bool CAccountDB::Connect(const char* host, const char* user, const char* password, const char* database)
{
    if (m_sql.Setup(host, user, password, database, "", true))
        m_IsConnect = true;
    return m_IsConnect;
}

bool CAccountDB::IsConnected()
{
    return m_IsConnect;
}

SQLMsg* CAccountDB::DirectQuery(const char* query)
{
    return m_sql.DirectQuery(query);
}

void CAccountDB::SetLocale(const std::string& locale)
{
    m_sql.SetLocale(locale);
    m_sql.QueryLocaleSet();
}

void PCBangPlayTimeWriter_ClearIPList();
void PCBangPlayTimeWriter_InsertIP(const char* id, const char* ip);

void DBManager::AnalyzeReturnQuery(SQLMsg* message)
{
    CReturnQueryInfo* info = static_cast<CReturnQueryInfo*>(message->pvUserData);
    SQLResult* result = message->Get();
    switch (info->iType) {
        case 0: {
            CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(info->dwIdent);
            if (!character || !character->GetDesc() || !result || !result->pSQLResult ||
                result->iNumRows < 1 || mysql_num_fields(result->pSQLResult) < 1)
                break;
            const MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
            if (row && row[0]) {
                const long pages = std::strtol(row[0], nullptr, 10);
                if (pages >= INT_MIN / 9 && pages <= INT_MAX / 9)
                    character->SetSafeboxSize(static_cast<int>(pages * 9));
            }
            break;
        }
        case 1: {
            m_map_dbstring.clear();
            m_vec_GreetMessage.clear();
            if (!result || !result->pSQLResult || mysql_num_fields(result->pSQLResult) < 2)
                break;

            while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult)) {
                if (row[0] && row[1]) {
                    m_map_dbstring.emplace(row[0], row[1]);
                    sys_log(0, "DBSTR '%s' '%s'", row[0], row[1]);
                }
            }
            const auto greet = m_map_dbstring.find("GREET");
            if (greet != m_map_dbstring.end()) {
                std::istringstream input(greet->second);
                while (!input.eof()) {
                    std::string line;
                    std::getline(input, line);
                    m_vec_GreetMessage.push_back(line);
                }
            }
            break;
        }
        case 2: {
            std::unique_ptr<TPacketCGLogin3> request(static_cast<TPacketCGLogin3*>(info->pvData));
            DESC_MANAGER& descriptors = DESC_MANAGER::instance();
            DESC* desc = descriptors.FindByLoginKey(info->dwIdent);
            if (!desc || !request)
                break;

            desc->m_Login.assign(request->login);
            sys_log(0, "QID_AUTH_LOGIN: START %lu %p", info->dwIdent, desc);
            const bool netmarbleVerified = LC_IsJapan() && !test_server &&
                (request->header == NETMARBLE_ACCOUNT_LOOKUP || request->header == NETMARBLE_ACCOUNT_CREATED);
            if (!result || !result->pSQLResult || result->iNumRows < 1) {
                if (netmarbleVerified && request->header == NETMARBLE_ACCOUNT_LOOKUP &&
                    result && result->pSQLResult && result->iNumRows == 0) {
                    char escapedLogin[61];
                    EscapeString(escapedLogin, request->login, std::strlen(request->login));
                    request->header = NETMARBLE_ACCOUNT_CREATED;
                    sys_log(0, "NETMARBLE_JP : Create A new AccountID From NetmarbleDBAgent");
                    ReturnQuery(13, info->dwIdent, request.release(),
                                "INSERT INTO account(login, password, social_id, create_time) "
                                "VALUES('%s', password('...'), '0000000000000', NOW()) ;", escapedLogin);
                    break;
                }
                sys_log(0, "   NOID");
                LoginFailure(desc, "NOID");
                break;
            }

            const unsigned columns = mysql_num_fields(result->pSQLResult);
            MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
            if (!row || columns < 7)
                break;
            constexpr unsigned required[] = {0, 1, 3, 4, 5, 6};
            bool valid = true;
            for (const unsigned column : required) {
                if (row[column])
                    continue;
                _sys_err("AnalyzeReturnQuery", 0x335, "error column %d", static_cast<int>(column));
                valid = false;
                break;
            }
            if (!valid)
                break;

            char calculated[46] = {};
            char stored[46] = {};
            char status[9] = {};
            std::strncpy(calculated, row[0], sizeof(calculated) - 1);
            std::strncpy(stored, row[1], sizeof(stored) - 1);
            std::strncpy(status, row[5], sizeof(status) - 1);
            const char* failure = nullptr;
            if (!netmarbleVerified && std::strcmp(calculated, stored) != 0)
                failure = "WRONGPWD";
            else if (std::atoi(row[6]) != 0)
                failure = "NOTAVAIL";
            else if (descriptors.FindByLoginName(request->login))
                failure = "ALREADY";
            else if (std::strcmp(status, "OK") != 0)
                failure = status;

            if (!failure && (LC_IsGermany() || LC_IsEurope())) {
                char date[9] = "00000000";
                if (columns > 14 && row[14]) {
                    const std::time_t created = std::strtoll(row[14], nullptr, 10);
                    if (const std::tm* local = std::localtime(&created))
                        std::strftime(date, sizeof(date), "%Y%m%d", local);
                }
                if (std::strncmp(date, g_stBlockDate.c_str(), 8) >= 0)
                    failure = "BLKLOGIN";
            }
            if (failure) {
                LoginFailure(desc, failure);
                sys_log(0, "   %s", failure);
                break;
            }

            int premium[9] = {};
            if (LC_IsJapan()) {
                constexpr unsigned slots[] = {0, 1, 2, 4, 5, 6};
                for (unsigned i = 0; i < sizeof(slots) / sizeof(slots[0]); ++i) {
                    if (7 + i < columns && row[7 + i])
                        premium[slots[i]] = std::atoi(row[7 + i]);
                }
            } else {
                for (unsigned i = 0; i < 7; ++i) {
                    if (7 + i < columns && row[7 + i])
                        premium[i] = std::atoi(row[7 + i]);
                }
            }

            TAccountTable& account = desc->GetAccountTable();
            account.id = std::strtoul(row[4], nullptr, 10);
            std::memcpy(account.login, request->login, sizeof(request->login));
            std::memcpy(account.passwd, request->passwd, sizeof(request->passwd));
            std::strncpy(account.social_id, row[3], sizeof(account.social_id) - 1);
            account.social_id[sizeof(account.social_id) - 1] = '\0';
            descriptors.ConnectAccount(request->login, desc);

            char matrix[193] = {};
            if (row[2])
                std::strncpy(matrix, row[2], sizeof(matrix) - 1);
            desc->m_stMatrixCode.assign(matrix);
            if (g_bBilling) {
                sys_log(0, "QID_AUTH_LOGIN: SUCCESS %s", request->login);
                break;
            }
            DWORD key[4];
            for (int i = 0; i < 4; ++i)
                key[i] = request->adwClientKey[i];
            LoginPrepare(2, 0, 0, desc, key, premium);
            break;
        }
        case 3: {
            DWORD* data = static_cast<DWORD*>(info->pvData);
            CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(info->dwIdent);
            if (character && data) {
                if (!result || result->iAffectedRows == 0) {
                    sys_log(0, "GIVE LOTTO FAIL TO pid %lu", character->GetPlayerID());
                } else if (LPITEM item = character->AutoGiveItem(data[0], static_cast<BYTE>(data[1]), -1, true)) {
                    sys_log(0, "GIVE LOTTO SUCCESS TO %s (pid %lu)", character->GetName(), info->dwIdent);
                    item->SetSocket(0, result->iInsertID);
                    item->SetSocket(1, data[2]);
                } else {
                    sys_log(0, "GIVE LOTTO FAIL2 TO pid %lu", character->GetPlayerID());
                }
            }
            delete[] data;
            break;
        }
        case 10: {
            CHARACTER* requester = CHARACTER_MANAGER::instance().FindByPID(info->dwIdent);
            if (!requester)
                break;
            if (!result || !result->pSQLResult || !result->iNumRows)
            {
                requester->ChatPacket(1, "No one currently blocked.");
                break;
            }
            while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult))
                if (row[0] && row[1])
                    requester->ChatPacket(1, "%s %s sec", row[0], row[1]);
            break;
        }
        case 11: {
            if (!result || !result->pSQLResult || result->iNumRows < 1 ||
                mysql_num_fields(result->pSQLResult) < 13) {
                if (test_server)
                    _sys_err("AnalyzeReturnQuery", 0x581, "'%s' mysql table is NOT EXIST", "pcbang_ip");
                break;
            }
            static std::string lastUpdate;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result->pSQLResult))) {
                if (!row[0] || !row[12])
                    continue;
                if (test_server)
                    sys_log(0, "%s:%s", row[0], row[12]);
                if (std::strcmp(row[0], "pcbang_ip") == 0) {
                    if (lastUpdate != row[12]) {
                        lastUpdate = row[12];
                        sys_log(0, "'%s' mysql table is UPDATED(%s)", "pcbang_ip", row[12]);
                        ReturnQuery(12, 0, 0, "SELECT pcbang_id, ip FROM pcbang_ip;");
                    } else {
                        sys_log(0, "'%s' mysql table is NOT updated(%s)", "pcbang_ip", row[12]);
                    }
                    break;
                }
            }
            if (!row)
                _sys_err("AnalyzeReturnQuery", 0x57c, "'%s' mysql table CANNOT FIND", "pcbang_ip");
            break;
        }
        case 4: {
            std::unique_ptr<THighscoreRegisterQueryInfo> request(
                static_cast<THighscoreRegisterQueryInfo*>(info->pvData));
            if (!request)
                break;

            if (result && result->pSQLResult && result->iNumRows > 0) {
                const MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
                if (row && row[0])
                    std::atoi(row[0]);
            }

            char escapedBoard[sizeof(request->board) * 2 + 1];
            EscapeString(escapedBoard, request->board, std::strlen(request->board));
            Query("REPLACE INTO highscore%s VALUES('%s', %lu, %d)",
                  get_table_postfix(), escapedBoard, request->pid, request->value);
            break;
        }
        case 5:
            break;
        case 6:
        case 7: {
            std::unique_ptr<TPacketCGLogin3> request(static_cast<TPacketCGLogin3*>(info->pvData));
            DESC* desc = DESC_MANAGER::instance().FindByLoginKey(info->dwIdent);
            const bool ipBilling = info->iType == 6;
            if (!ipBilling)
                sys_log(0, "QID_BILLING_GET_TIME: START ident %lu d %p", info->dwIdent,
                        static_cast<void*>(desc));
            if (!desc || !request || !desc->GetAccountTable().id || GetLoginData(info->dwIdent))
                break;

            BYTE billType = 0;
            DWORD billId = 0;
            int seconds = 0;
            if (!result || !result->pSQLResult || result->iNumRows < 1) {
                if (ipBilling) {
                    sys_log(0, "QID_BILLING_GET_IP: NO_IP");
                    break;
                }
                if (g_bBilling) {
                    LoginFailure(desc, "NOBILL");
                    break;
                }
                billType = 2;
            } else {
                const bool allowed = ipBilling
                    ? GetGameTimeIP(result->pSQLResult, billType, billId, seconds, desc->GetHostName())
                    : GetGameTime(result->pSQLResult, billType, seconds);
                if (!allowed) {
                    if (!ipBilling)
                        sys_log(0, "QID_BILLING_GET_TIME: BLOCK");
                    LoginFailure(desc, "BLOCK");
                    break;
                }
                if (!billType) {
                    if (!ipBilling) {
                        LoginFailure(desc, "NOBILL");
                        sys_log(0, "QID_BILLING_GET_TIME: NO TIME");
                    }
                    break;
                }
            }

            DWORD clientKeys[4];
            for (int i = 0; i < 4; ++i)
                clientKeys[i] = request->adwClientKey[i];
            LoginPrepare(billType, billId, seconds, desc, clientKeys, nullptr);
            if (!ipBilling && result && result->iNumRows > 0)
                sys_log(0, "QID_BILLING_GET_TIME: SUCCESS");
            break;
        }
        case 9: {
            std::unique_ptr<CLoginData> login(static_cast<CLoginData*>(info->pvData));
            if (!login || info->dwIdent > 1 || GetLoginData(login->GetKey()))
                break;

            const bool ipBilling = info->dwIdent == 0;
            BYTE billType = 0;
            DWORD billId = 0;
            int seconds = 0;
            if (!result || !result->pSQLResult || result->iNumRows < 1) {
                if (ipBilling)
                    break;
            } else {
                const bool allowed = ipBilling
                    ? GetGameTimeIP(result->pSQLResult, billType, billId, seconds, login->GetIP())
                    : GetGameTime(result->pSQLResult, billType, seconds);
                if (allowed && billType) {
                    login->SetBillType(billType);
                    if (ipBilling)
                        login->SetBillID(billId);
                    login->SetRemainSecs(seconds);
                    login->SetBilling(true);
                    InsertLoginData(login.release());
                    break;
                }
                if (allowed && ipBilling)
                    break;
            }

            if (ipBilling)
                sys_log(0, "REPAIR: failed, GameTimeIP %s", login->GetIP());
            else
                sys_log(0, "REPAIR: failed, GameTime %s", login->GetLogin());
            SendBillingExpire(login->GetLogin(), 6, 0, login.get());
            break;
        }
        case 8: {
            std::unique_ptr<TUseTime> use(static_cast<TUseTime*>(info->pvData));
            if (!use)
                break;

            int remaining = 0;
            CLoginData* login = nullptr;
            if (result && result->pSQLResult && result->iNumRows > 0 &&
                mysql_num_fields(result->pSQLResult) >= 2) {
                MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
                if (row && row[0] && row[1]) {
                    const int daySeconds = std::atoi(row[0]);
                    const int timeSeconds = std::atoi(row[1]);
                    login = GetLoginData(use->dwLoginKey);
                    if (login) {
                        const BYTE type = login->GetBillType();
                        if (type == 4 || type == 6) {
                            remaining = daySeconds;
                        } else if (type == 3 || type == 5) {
                            remaining = timeSeconds;
                            if (timeSeconds <= 600 && daySeconds > 0) {
                                use->bBillType = type + 1;
                                login->SetBillType(use->bBillType);
                                remaining = daySeconds;
                            }
                        }
                        login->SetRemainSecs(remaining);
                    }
                }
            }

            SendBillingExpire(use->szLogin, use->bBillType, MAX(0, remaining), login);
            break;
        }
        case 12:
            if (result && result->pSQLResult && result->iNumRows > 0 &&
                mysql_num_fields(result->pSQLResult) >= 2) {
                PCBangPlayTimeWriter_ClearIPList();
                while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult)) {
                    if (row[0] && row[1])
                        PCBangPlayTimeWriter_InsertIP(row[0], row[1]);
                }
            } else if (test_server) {
                sys_log(0, "PCBANG_IP_LIST is EMPTY");
            }
            break;
        case 13: {
            std::unique_ptr<TPacketCGLogin3> request(static_cast<TPacketCGLogin3*>(info->pvData));
            if (!request)
                break;
            if (result && result->iAffectedRows > 0) {
                sys_log(0, "NETMARBLE_JP : Succeed to create a new account %s", request->login);
                NetmarbleJP_QueryAccount(info->dwIdent, request.release());
            } else {
                DESC* desc = DESC_MANAGER::instance().FindByLoginKey(info->dwIdent);
                sys_log(0, "   NOID");
                sys_log(0, "NETMARBLE_JP : Failed to create a new account %s", request->login);
                LoginFailure(desc, "NOID");
            }
            break;
        }
        default:
            _sys_err("AnalyzeReturnQuery", 0x5c1, "FATAL ERROR!!! Unhandled return query id %d", info->iType);
            break;
    }
    delete info;
}
