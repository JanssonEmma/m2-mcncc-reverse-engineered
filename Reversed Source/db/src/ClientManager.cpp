#include "stdafx.h"
#include "NumericInput.h"
#include "ClientManager.h"
#include "Peer.h"
#include "Config.h"
#include "GuildManager.h"
#include "Main.h"
#include "Cache.h"
#include "BlockCountry.h"
#include "PrivManager.h"
#include "Marriage.h"
#include "MoneyLog.h"
#include "QID.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "../../libgame/include/grid.h"
#include "ItemAwardManager.h"

#include <memory>
#include <sstream>
#include <stdarg.h>
#ifndef _WIN32
#include <strings.h>
#endif
#include <utility>

CPacketInfo g_query_info;
CPacketInfo g_item_info;

void CPacketInfo::Add(int header)
{
    ++m_map_info[header];
}

void CPacketInfo::Reset()
{
    m_map_info.clear();
}

namespace
{
bool IsIPv4Address(const char* text)
{
    in_addr address{};
#ifdef _WIN32
    return InetPtonA(AF_INET, text, &address) == 1;
#else
    return inet_pton(AF_INET, text, &address) == 1;
#endif
}
}

void CClientManager::SetCacheFlushCountLimit(int limit)
{
    m_iCacheFlushCountLimit = MAX(10, limit);
    sys_log(0, "CACHE_FLUSH_LIMIT_PER_SECOND: %d", m_iCacheFlushCountLimit);
}

CPeer* CClientManager::GetAnyPeer()
{
    if (m_peerList.empty())
        return NULL;

    return m_peerList.front();
}

void CClientManager::ForwardPacket(BYTE header, const void* data, int size, BYTE channel, CPeer* except)
{
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        CPeer* peer = *it;
        if (peer == except || !peer->GetChannel())
            continue;

        if (channel && peer->GetChannel() != channel)
            continue;

        peer->EncodeHeader(header, 0, size);
        if (size > 0 && data)
            peer->Encode(data, size);
    }
}

time_t CClientManager::GetCurrentTime()
{
    return time(0);
}

void CClientManager::SetPlayerIDStart(int start)
{
    m_iPlayerIDStart = start;
}

CPeer* CClientManager::GetPeer(DWORD handle)
{
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        if ((*it)->GetHandle() == handle)
            return *it;
    }
    return NULL;
}

int g_query_count[2];
int g_item_count;

CClientManager::CClientManager()
    : m_looping(0),
      m_fdAccept(static_cast<socket_t>(-1)),
      m_pkAuthPeer(NULL),
      m_iPlayerIDStart(0),
      m_iPlayerDeleteLevelLimit(0),
      m_bChinaEventServer(false),
      m_iShopTableSize(0),
      m_pShopTable(NULL),
      m_iRefineTableSize(0),
      m_pRefineTable(NULL),
      m_bShutdowned(false),
      m_bLastHeader(0),
      m_iCacheFlushCount(0),
      m_iCacheFlushCountLimit(200),
      m_dwItemIDStart(0),
      m_dwItemIDStop(0),
      m_dwNowItemID(0)
{
    memset(g_query_count, 0, sizeof(g_query_count));
}

CClientManager::~CClientManager()
{
    Destroy();
}

void CClientManager::Destroy()
{
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
        delete *it;

    m_peerList.clear();

    if (m_fdAccept != static_cast<socket_t>(-1))
    {
        fdwatch_forget_closed_fd(m_fdWatcher, m_fdAccept);
        socket_close(m_fdAccept);
        m_fdAccept = static_cast<socket_t>(-1);
    }
}

void CClientManager::Quit()
{
    m_bShutdowned = true;
}

bool CClientManager::Initialize()
{
    if (!InitializeLocalization())
    {
        fprintf(stderr, "Failed Localization Infomation so exit\n");
        return false;
    }

    if (!InitializeNowItemID())
    {
        fprintf(stderr, " Item range Initialize Failed. Exit DBCache Server\n");
        return false;
    }

    if (!InitializeTables())
    {
        sys_err("Table Initialize FAILED");
        return false;
    }

    CGuildManager::instance().BootReserveWar();

    int value;
    if (!CConfig::instance().GetValue("BIND_PORT", &value))
        value = 5300;

    if (value <= 0 || value > 65535)
    {
        sys_err("Invalid BIND_PORT");
        return false;
    }

    char bindAddress[16] = "127.0.0.1";
    CConfig::instance().GetValue("BIND_IP", bindAddress, sizeof(bindAddress));
    if (!IsIPv4Address(bindAddress))
    {
        sys_err("BIND_IP must be an IPv4 address");
        return false;
    }

    m_allowedPeerAddresses = {"127.0.0.1"};
    char allowed[1024];
    if (CConfig::instance().GetValue("DB_PEER_ALLOWLIST", allowed, sizeof(allowed)))
    {
        std::istringstream addresses(allowed);
        for (std::string address; addresses >> address;)
        {
            if (!IsIPv4Address(address.c_str()))
            {
                sys_err("DB_PEER_ALLOWLIST must contain IPv4 addresses");
                return false;
            }
            m_allowedPeerAddresses.insert(address);
        }
    }

    char authAddress[16] = "127.0.0.1";
    CConfig::instance().GetValue("DB_AUTH_PEER_IP", authAddress, sizeof(authAddress));
    if (!IsIPv4Address(authAddress) || !m_allowedPeerAddresses.contains(authAddress))
    {
        sys_err("DB_AUTH_PEER_IP must be an IPv4 address on the DB peer allowlist");
        return false;
    }
    m_authPeerAddress = authAddress;

    m_fdAccept = socket_tcp_bind(bindAddress, value);
    if (m_fdAccept == static_cast<socket_t>(-1))
    {
        perror("socket");
        return false;
    }
    if (m_fdAccept >= static_cast<socket_t>(m_fdWatcher->nfiles))
    {
        sys_err("DB listener descriptor exceeds the poller capacity");
        socket_close(m_fdAccept);
        m_fdAccept = static_cast<socket_t>(-1);
        return false;
    }

    fdwatch_add_fd(m_fdWatcher, m_fdAccept, NULL, FDW_READ, false);

    if (!CConfig::instance().GetValue("BACKUP_LIMIT_SEC", &value))
        value = 600;

    m_looping = true;
    if (!CConfig::instance().GetValue("PLAYER_DELETE_LEVEL_LIMIT", &m_iPlayerDeleteLevelLimit))
    {
        sys_err("conf.txt: Cannot find PLAYER_DELETE_LEVEL_LIMIT, use default level %d", 100);
        m_iPlayerDeleteLevelLimit = 100;
    }

    m_bChinaEventServer = false;
    value = 0;
    if (CConfig::instance().GetValue("CHINA_EVENT_SERVER", &value))
        m_bChinaEventServer = (value != 0);

    LoadEventFlag();
    return true;
}

CLoginData* CClientManager::GetLoginData(DWORD key)
{
    std::map<DWORD, CLoginData*>::iterator it = m_map_pkLoginData.find(key);
    return it != m_map_pkLoginData.end() ? it->second : NULL;
}

CLoginData* CClientManager::GetLoginDataByLogin(const char* login)
{
    char normalized[LOGIN_MAX_LEN + 1];
    trim_and_lower(normalized, login, LOGIN_MAX_LEN);
    std::map<std::string, CLoginData*>::iterator it = m_map_pkLoginDataByLogin.find(normalized);
    return it != m_map_pkLoginDataByLogin.end() ? it->second : NULL;
}

CLoginData* CClientManager::GetLoginDataByAID(DWORD accountID)
{
    std::map<DWORD, CLoginData*>::iterator it = m_map_pkLoginDataByAID.find(accountID);
    return it != m_map_pkLoginDataByAID.end() ? it->second : NULL;
}

bool CClientManager::InitializeNowItemID()
{
    if (!CConfig::instance().GetTwoValue("ITEM_ID_RANGE", &m_dwItemIDStart, &m_dwItemIDStop))
    {
        sys_err("conf.txt: Cannot find ITEM_ID_RANGE [start_item_id] [end_item_id]");
        return false;
    }

    sys_log(0, "ItemRange From File %d ,%d ",
            static_cast<int>(m_dwItemIDStart), static_cast<int>(m_dwItemIDStop));

    char query[256];
    dbsql::snprintf(query, sizeof(query), "SELECT MAX(id) from item%s WHERE id >= %lu AND id <= %lu", GetTablePostfix(), m_dwItemIDStart, m_dwItemIDStop);
    SQLMsg* message = CDBManager::instance().DirectQuery(query);
    if (message->Get()->iNumRows == 0)
    {
        sys_err("ClientManager::InitializeNowItemID DirectQuery Failed");
        delete message;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
    const DWORD lastID = row[0] ? static_cast<std::uint32_t>(atoi(row[0])) : m_dwItemIDStart;
    if (lastID == m_dwItemIDStop)
    {
        sys_err("Clientmanager::InitializeNowItemID() : Not Enough ID ");
        delete message;
        return false;
    }

    m_dwNowItemID = lastID + 1;
    delete message;

    sys_log(0, " Init Success Start %lu End %lu Now %lu\n",
            m_dwItemIDStart, m_dwItemIDStop, m_dwNowItemID);
    return true;
}

DWORD CClientManager::GainItemID()
{
    return m_dwNowItemID++;
}

void CClientManager::MainLoop()
{
    while (!m_bShutdowned)
    {
        SQLMsg* message;
        while ((message = CDBManager::instance().PopResult()))
        {
            AnalyzeQueryResult(message);
            delete message;
        }

        if (!Process())
            break;

        log_rotate();
    }

    sys_log(0, "MainLoop exited, Starting cache flushing");
    signal_timer_disable();

    for (TPlayerTableCacheMap::iterator it = m_map_playerCache.begin(); it != m_map_playerCache.end(); ++it)
    {
        it->second->Flush();
        delete it->second;
    }
    m_map_playerCache.clear();

    for (TItemCacheMap::iterator it = m_map_itemCache.begin(); it != m_map_itemCache.end(); ++it)
    {
        it->second->Flush();
        delete it->second;
    }
    m_map_itemCache.clear();

    for (TItemPriceListCacheMap::iterator it = m_mapItemPriceListCache.begin(); it != m_mapItemPriceListCache.end(); ++it)
    {
        it->second->Flush();
        delete it->second;
    }
    m_mapItemPriceListCache.clear();
}

extern std::string g_stLocale;
extern std::string g_stLocaleNameColumn;

namespace
{
struct LocaleRule
{
    const char* region;
    const char* charset;
    const char* nameColumn;
    const char* loggedCharset;
};

const LocaleRule localeRules[] =
{
    {"cibn", "gb2312", "gb2312name", "gb2312"},
    {"ymir", "euckr", "name", "euckr"},
    {"japan", "sjis", "locale_name", "sjis"},
    {"english", "", "locale_name", "euckr"},
    {"germany", "latin1", "locale_name", "euckr"},
    {"france", "latin1", "locale_name", "euckr"},
    {"italy", "latin1", "locale_name", "euckr"},
    {"spain", "latin1", "locale_name", "euckr"},
    {"uk", "latin1", "locale_name", "euckr"},
    {"turkey", "latin5", "locale_name", "euckr"},
    {"poland", "latin2", "locale_name", "euckr"},
    {"portugal", "latin1", "locale_name", "euckr"},
    {"hongkong", "big5", "gb2312name", "big5"},
    {"newcibn", "gb2312", "gb2312name", "gb2312"},
    {"korea", "euckr", "name", "euckr"},
    {"canada", "latin1", "gb2312name", "latin1"},
    {"brazil", "latin1", "locale_name", "latin1"}
};

void __CheckPlayerSlot()
{
}
}

bool CClientManager::InitializeLocalization()
{
    char query[512];
    dbsql::sprintf(query, "SELECT mValue , mKey FROM locale");

    SQLMsg* message = CDBManager::instance().DirectQuery(query, SQL_COMMON);
    if (message->Get()->iNumRows == 0)
    {
        sys_err("InitializeLocalization() ==> DirectQuery failed( %s )", query);
        delete message;
        return false;
    }
    sys_log(0, "InitializeLocalization() - LoadLocaleTable(count:%d)", static_cast<int>(message->Get()->iNumRows));

    m_vec_Locale.clear();

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(message->Get()->pSQLResult)) != NULL)
    {
        tLocale locale;
        memcpy(locale.szValue, row[0], sizeof(locale.szValue));
        memcpy(locale.szKey, row[1], sizeof(locale.szKey));

        if (strcmp(locale.szKey, "LOCALE") == 0)
        {
            const LocaleRule* end = localeRules + sizeof(localeRules) / sizeof(localeRules[0]);
            const LocaleRule* rule = localeRules;
            while (rule != end && strcmp(locale.szValue, rule->region) != 0)
                ++rule;

            if (rule == end)
            {
                sys_err("locale[LOCALE] = UNKNOWN(%s)", locale.szValue);
                exit(0);
            }

            sys_log(0, "locale[LOCALE] = %s", locale.szValue);
            g_stLocale = rule->charset;
            g_stLocaleNameColumn = rule->nameColumn;
            if (!CDBManager::instance().SetLocale(g_stLocale.c_str()))
            {
                delete message;
                return false;
            }
        }
        else if (strcmp(locale.szKey, "DB_NAME_COLUMN") == 0)
        {
            if (!dbsql::IsIdentifier(locale.szValue))
            {
                sys_err("Invalid locale DB_NAME_COLUMN");
                delete message;
                return false;
            }
            g_stLocaleNameColumn = locale.szValue;
            sys_log(0, "locale[DB_NAME_COLUMN] = %s", g_stLocaleNameColumn.c_str());
        }

        m_vec_Locale.push_back(locale);
    }

    delete message;
    __CheckPlayerSlot();
    return true;
}

extern int g_log;
extern BOOL g_test_server;

CClientManager::TItemCacheSet* CClientManager::GetItemCacheSet(DWORD owner)
{
    TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(owner);
    return it != m_map_pkItemCacheSetPtr.end() ? it->second : NULL;
}

void CClientManager::CreateItemCacheSet(DWORD owner)
{
    if (m_map_pkItemCacheSetPtr.find(owner) != m_map_pkItemCacheSetPtr.end())
        return;

    m_map_pkItemCacheSetPtr.insert(std::make_pair(owner, new TItemCacheSet));
    if (g_log)
        sys_log(0, "ITEM_CACHE: new cache %lu", owner);
}

CItemCache* CClientManager::GetItemCache(DWORD id)
{
    TItemCacheMap::iterator it = m_map_itemCache.find(id);
    return it != m_map_itemCache.end() ? it->second : NULL;
}

void CClientManager::PutItemCache(TPlayerItem* item, bool skipQuery)
{
    CItemCache* entry = GetItemCache(item->id);
    if (!entry)
    {
        if (g_log)
            sys_log(0, "ITEM_CACHE: PutItemCache ==> New CItemCache id%d ,vnum %d new owner %d",
                    static_cast<int>(item->id), static_cast<int>(item->vnum), static_cast<int>(item->owner));
        entry = new CItemCache;
        m_map_itemCache.insert(std::make_pair(item->id, entry));
    }
    else
    {
        if (g_log)
            sys_log(0, "ITEM_CACHE: PutItemCache ==> Have Cache");
        if (item->owner != entry->Get()->owner)
        {
            TItemCacheSet* previous = GetItemCacheSet(entry->Get()->owner);
            if (previous)
            {
                if (g_log)
                    sys_log(0, "ITEM_CACHE: delete owner %lu id %lu new owner %lu",
                            static_cast<DWORD>(entry->Get()->owner), static_cast<DWORD>(entry->Get()->id), static_cast<DWORD>(item->owner));
                previous->erase(entry);
            }
        }
    }

    entry->Put(item, skipQuery);
    TItemCacheSet* ownerSet = GetItemCacheSet(entry->Get()->owner);
    const DWORD id = entry->Get()->id;
    const DWORD owner = entry->Get()->owner;
    if (ownerSet)
    {
        sys_log(g_log ? 0 : 1, "ITEM_CACHE: save %lu id %lu", owner, id);
        ownerSet->insert(entry);
    }
    else
    {
        sys_log(g_log ? 0 : 1, "ITEM_CACHE: direct save %lu id %lu", owner, id);
        entry->OnFlush();
    }
}

int CClientManager::AnalyzeQueryResult(SQLMsg* message)
{
    CQueryInfo* query = static_cast<CQueryInfo*>(message->pvUserData);
    CPeer* peer = GetPeer(query->dwIdent);

    switch (query->iType)
    {
        case QID_ITEM_AWARD_LOAD:
            ItemAwardManager::instance().Load(message);
            delete query;
            return 1;

        case QID_GUILD_RANKING:
            CGuildManager::instance().ResultRanking(message->Get()->pSQLResult);
            break;

        case QID_ITEMPRICE_LOAD_FOR_UPDATE:
            RESULT_PRICELIST_LOAD_FOR_UPDATE(message);
            break;
    }

    if (peer)
    {
        switch (query->iType)
        {
            case QID_PLAYER:
            case QID_ITEM:
            case QID_QUEST:
            case QID_AFFECT:
                RESULT_COMPOSITE_PLAYER(peer, message, query->iType);
                break;

            case QID_LOGIN:
                RESULT_LOGIN(peer, message);
                break;

            case QID_SAFEBOX_LOAD:
                RESULT_SAFEBOX_LOAD(peer, message);
                break;

            case QID_SAFEBOX_CHANGE_SIZE:
                RESULT_SAFEBOX_CHANGE_SIZE(peer, message);
                break;

            case QID_SAFEBOX_CHANGE_PASSWORD:
                RESULT_SAFEBOX_CHANGE_PASSWORD(peer, message);
                break;

            case QID_SAFEBOX_CHANGE_PASSWORD_SECOND:
                RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(peer, message);
                break;

            case QID_SAFEBOX_SAVE:
            case QID_ITEM_SAVE:
            case QID_ITEM_DESTROY:
            case QID_QUEST_SAVE:
            case QID_PLAYER_SAVE:
            case QID_ITEM_AWARD_TAKEN:
                break;

            case QID_HIGHSCORE_REGISTER:
                RESULT_HIGHSCORE_REGISTER(peer, message);
                break;

            case QID_PLAYER_DELETE:
                __RESULT_PLAYER_DELETE(peer, message);
                break;

            case QID_LOGIN_BY_KEY:
                RESULT_LOGIN_BY_KEY(peer, message);
                break;

            case QID_PLAYER_INDEX_CREATE:
                RESULT_PLAYER_INDEX_CREATE(peer, message);
                break;

            case QID_ITEMPRICE_LOAD:
                RESULT_PRICELIST_LOAD(peer, message);
                break;

            default:
                sys_err("CClientManager::AnalyzeQueryResult unknown query result type: %d", query->iType);
                break;
        }
    }

    delete query;
    return 1;
}

std::uint32_t g_dwUsageMax = 0;
std::uint32_t g_dwUsageAvg = 0;

void UsageLog()
{
    const int average = static_cast<int>(g_dwUsageAvg / 3600);
    const time_t current = time(nullptr);
    const tm* local = localtime(&current);
    if (!local)
        return;

    const char* text = asctime(local);
    if (!text)
        return;

    FILE* file = fopen("usage.txt", "a+");
    if (!file)
        return;

    fprintf(file, "| %4d %-15.15s | %5d | %5d |", local->tm_year + 1900, text + 4, average, static_cast<int>(g_dwUsageMax));
    fprintf(file, "\n");
    fclose(file);

    g_dwUsageAvg = 0;
    g_dwUsageMax = 0;
}

int CClientManager::Process()
{
    int pulses = thecore_idle();
    if (!pulses)
        return 0;

    while (pulses--)
    {
        ++thecore_heart->pulse;
        if (thecore_heart->pulse % thecore_heart->passes_per_sec == 0)
        {
            CDBManager& db = CDBManager::instance();
            const char* statsFormat = g_test_server
                ? "[%9d] return %d/%d/%d/%d async %d/%d/%d/%d"
                : "[%9d] return %d/%d/%d/%d async %d/%d/%d%/%d";
            pt_log(statsFormat,
                   thecore_heart->pulse,
                   static_cast<int>(db.CountReturnQuery(SQL_PLAYER)),
                   static_cast<int>(db.CountReturnResult(SQL_PLAYER)),
                   static_cast<int>(db.CountReturnQueryFinished(SQL_PLAYER)),
                   static_cast<int>(db.CountReturnCopiedQuery(SQL_PLAYER)),
                   static_cast<int>(db.CountAsyncQuery(SQL_PLAYER)),
                   static_cast<int>(db.CountAsyncResult(SQL_PLAYER)),
                   static_cast<int>(db.CountAsyncQueryFinished(SQL_PLAYER)),
                   static_cast<int>(db.CountAsyncCopiedQuery(SQL_PLAYER)));

            db.ResetCounter();
            const auto userCount = static_cast<std::uint32_t>(CClientManager::instance().GetUserCount());
            g_dwUsageAvg += userCount;
            g_dwUsageMax = MAX(g_dwUsageMax, userCount);
            memset(thecore_profiler, 0, sizeof(thecore_profiler));

            if (thecore_heart->pulse % (thecore_heart->passes_per_sec * 3600) == 0)
                UsageLog();

            m_iCacheFlushCount = 0;
            UpdatePlayerCache();
            UpdateItemCache();
            UpdateLogoutPlayer();
            UpdateItemPriceListCache();
            CGuildManager::instance().Update();
            CPrivManager::instance().Update();
            marriage::CManager::instance().Update();
        }

        if (thecore_heart->pulse % (thecore_heart->passes_per_sec * 5) == 0)
            ItemAwardManager::instance().RequestLoad();

        if (thecore_heart->pulse % (thecore_heart->passes_per_sec * 10) == 0)
        {
            pt_log("QUERY: MAIN[%d] ASYNC[%d]", g_query_count[0], g_query_count[1]);
            g_query_count[0] = 0;
            g_query_count[1] = 0;
            pt_log("ITEM:%d\n", g_item_count);
            g_item_count = 0;
        }

        if (thecore_heart->pulse % (thecore_heart->passes_per_sec * 60) == 0)
            CClientManager::instance().SendTime();

        if (thecore_heart->pulse % (thecore_heart->passes_per_sec * 3600) == 0)
            CMoneyLog::instance().Save();
    }

    const int eventCount = fdwatch(m_fdWatcher, NULL);
    for (int index = 0; index < eventCount; ++index)
    {
        CPeer* peer = static_cast<CPeer*>(fdwatch_get_client_data(m_fdWatcher, index));
        if (!peer)
        {
            if (fdwatch_check_event(m_fdWatcher, m_fdAccept, index) == FDW_READ)
            {
                AddPeer(m_fdAccept);
                fdwatch_clear_event(m_fdWatcher, m_fdAccept, index);
            }
            else if (fdwatch_get_ident(m_fdWatcher, index) >= 0)
            {
                sys_err("FDWATCH: peer null in event: ident %d", fdwatch_get_ident(m_fdWatcher, index));
            }
            continue;
        }

        switch (fdwatch_check_event(m_fdWatcher, peer->GetFd(), index))
        {
            case FDW_READ:
                if (peer->Recv() < 0)
                {
                    sys_err("Recv failed");
                    RemovePeer(peer);
                }
                else
                {
                    ProcessPackets(peer);
                    if (peer->HasIOError())
                        RemovePeer(peer);
                }
                break;

            case FDW_WRITE:
                if (peer->Send() < 0)
                {
                    sys_err("Send failed");
                    RemovePeer(peer);
                }
                break;

            case FDW_EOF:
                RemovePeer(peer);
                break;

            default:
                sys_err("fdwatch_check_fd returned unknown result");
                RemovePeer(peer);
                break;
        }
    }

    VCardProcess();
    return 1;
}

DWORD CClientManager::GetUserCount()
{
    return m_map_kLogonAccount.size();
}

void CClientManager::SendTime()
{
    const int now = static_cast<int>(GetCurrentTime());
    ForwardPacket(90, &now, sizeof(now));
}

void CClientManager::SendNotice(const char* format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    const int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (length >= 0)
        ForwardPacket(1, buffer, std::min(static_cast<size_t>(length), sizeof(buffer) - 1) + 1);
}

void CClientManager::AddPeer(socket_t fd)
{
    CPeer* peer = new CPeer;
    if (peer->Accept(fd, m_allowedPeerAddresses))
        m_peerList.push_front(peer);
    else
        delete peer;
}

void CClientManager::RemovePeer(CPeer* peer)
{
    if (m_pkAuthPeer == peer)
    {
        m_pkAuthPeer = NULL;
    }
    else
    {
        for (TLogonAccountMap::iterator it = m_map_kLogonAccount.begin();
             it != m_map_kLogonAccount.end();)
        {
            TLogonAccountMap::iterator current = it++;
            CLoginData* login = current->second;
            if (login->GetConnectedPeerHandle() != peer->GetHandle())
                continue;

            if (login->IsPlay())
            {
                login->SetPlay(false);
                SendLoginToBilling(login, false);
            }

            login->SetConnectedPeerHandle(0);
            if (login->IsDeleted())
                delete login;
            m_map_kLogonAccount.erase(current);
        }
    }
    m_peerList.remove(peer);
    delete peer;
}

void CClientManager::SendLoginToBilling(CLoginData* login, bool playing)
{
    if (!m_pkAuthPeer)
        return;

    const DWORD key = login->GetKey();
    const BYTE state = playing;
    m_pkAuthPeer->EncodeHeader(130, 0, 9);
    m_pkAuthPeer->EncodeDWORD(1);
    m_pkAuthPeer->EncodeDWORD(key);
    m_pkAuthPeer->Encode(&state, 1);
}

bool CClientManager::DeleteItemCache(DWORD id)
{
    CItemCache* cache = GetItemCache(id);
    if (!cache)
        return false;

    cache->Delete();
    return true;
}

void CClientManager::FlushItemCacheSet(DWORD owner)
{
    TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(owner);
    if (it == m_map_pkItemCacheSetPtr.end())
    {
        sys_log(0, "FLUSH_ITEMCACHESET : No ItemCacheSet pid(%d)", static_cast<int>(owner));
        return;
    }

    TItemCacheSet* set = it->second;
    for (TItemCacheSet::iterator item = set->begin(); item != set->end();)
    {
        CItemCache* cache = *item++;
        cache->Flush();
        m_map_itemCache.erase(cache->Get()->id);
        delete cache;
    }

    delete set;
    m_map_pkItemCacheSetPtr.erase(it);
    if (g_log)
        sys_log(0, "FLUSH_ITEMCACHESET : Deleted pid(%d)", static_cast<int>(owner));
}

void CClientManager::UpdateItemCacheSet(int pid)
{
    TItemCacheSet* set = GetItemCacheSet(static_cast<std::uint32_t>(pid));
    if (!set)
        return;

    for (TItemCacheSet::iterator it = set->begin(); it != set->end();)
        (*it++)->Flush();
}

void CClientManager::UpdatePlayerCache()
{
    for (TPlayerTableCacheMap::iterator it = m_map_playerCache.begin();
         it != m_map_playerCache.end();)
    {
        CPlayerTableCache* cache = (it++)->second;
        if (cache->CheckTimeout())
        {
            if (g_log)
            {
                const auto* player = cache->Get(false);
                sys_log(0, "UPDATE : UpdatePlayerCache() ==> FlushPlayerCache %d %s ",
                        static_cast<int>(player->id), player->name);
            }
            cache->Flush();
            UpdateItemCacheSet(cache->Get()->id);
        }
        else if (cache->CheckFlushTimeout())
            cache->Flush();
    }
}

void CClientManager::UpdateItemCache()
{
    if (m_iCacheFlushCount >= m_iCacheFlushCountLimit)
        return;

    for (TItemCacheMap::iterator it = m_map_itemCache.begin(); it != m_map_itemCache.end();)
    {
        CItemCache* cache = (it++)->second;
        if (!cache->CheckFlushTimeout())
            continue;

        if (g_test_server)
        {
            const int owner = static_cast<int>(cache->Get()->owner);
            const int id = static_cast<int>(cache->Get()->id);
            const int vnum = static_cast<int>(cache->Get()->vnum);
            sys_log(0, "UpdateItemCache ==> Flush() vnum %d id owner %d", vnum, id, owner);
        }

        cache->Flush();
        if (++m_iCacheFlushCount >= m_iCacheFlushCountLimit)
            break;
    }
}

void CClientManager::UpdateItemPriceListCache()
{
    for (TItemPriceListCacheMap::iterator it = m_mapItemPriceListCache.begin();
         it != m_mapItemPriceListCache.end();)
    {
        TItemPriceListCacheMap::iterator current = it++;
        CItemPriceListTableCache* cache = current->second;
        if (cache->CheckFlushTimeout())
        {
            cache->Flush();
            m_mapItemPriceListCache.erase(current);
        }
    }
}

namespace
{
template <size_t N>
bool HasTerminator(const char (&text)[N])
{
    return memchr(text, '\0', N) != nullptr;
}

template <class Packet, class... Fields>
bool IsTextPacket(const char* data, DWORD length, Fields... fields)
{
    if (length != sizeof(Packet))
        return false;

    const auto& packet = *reinterpret_cast<const Packet*>(data);
    return (HasTerminator(packet.*fields) && ...);
}

std::uint32_t ReadPacketDWORD(const char* data)
{
    std::uint32_t value;
    memcpy(&value, data, sizeof(value));
    return value;
}

bool HasPacketRecords(DWORD bodySize, size_t recordSize, DWORD count)
{
    return bodySize % recordSize == 0 && bodySize / recordSize == count;
}

bool IsValidGamePacket(BYTE header, const char* data, DWORD length)
{
    switch (header)
    {
        case 2:
            return IsTextPacket<TLogoutPacket>(data, length, &TLogoutPacket::login);
        case 3:
            return length == sizeof(TPlayerLoadPacket) &&
                   reinterpret_cast<const TPlayerLoadPacket*>(data)->account_index < PLAYER_PER_ACCOUNT;
        case 4:
            return IsTextPacket<TPlayerTable>(data, length,
                    &TPlayerTable::name, &TPlayerTable::ip);
        case 5:
        {
            if (!IsTextPacket<TPlayerCreatePacket>(data, length, &TPlayerCreatePacket::login))
                return false;
            const auto& packet = *reinterpret_cast<const TPlayerCreatePacket*>(data);
            return packet.account_index < PLAYER_PER_ACCOUNT && HasTerminator(packet.player_table.name);
        }
        case 6:
            return IsTextPacket<TPlayerDeletePacket>(data, length, &TPlayerDeletePacket::login) &&
                   reinterpret_cast<const TPlayerDeletePacket*>(data)->account_index < PLAYER_PER_ACCOUNT;
        case 9:
            return IsTextPacket<TPacketGDBoot>(data, length, &TPacketGDBoot::szIP);
        case 10:
            return length == sizeof(TPlayerCountPacket);
        case 11:
            if (length % sizeof(TQuestTable) != 0)
                return false;
            for (DWORD offset = 0; offset < length; offset += sizeof(TQuestTable))
            {
                if (!IsTextPacket<TQuestTable>(data + offset, sizeof(TQuestTable),
                                              &TQuestTable::szName, &TQuestTable::szState))
                    return false;
            }
            return true;
        case 12:
        case 107:
            return IsTextPacket<TSafeboxLoadPacket>(data, length,
                    &TSafeboxLoadPacket::szLogin, &TSafeboxLoadPacket::szPassword);
        case 13:
            return length == sizeof(TSafeboxTable);
        case 14:
            return length == sizeof(TSafeboxChangeSizePacket);
        case 15:
            return length == sizeof(TEmpireSelectPacket);
        case 16:
            return IsTextPacket<TSafeboxChangePasswordPacket>(data, length,
                    &TSafeboxChangePasswordPacket::szOldPassword, &TSafeboxChangePasswordPacket::szNewPassword);
        case 19:
            return length == sizeof(TPacketGuildSkillUpdate);
        case 20:
            return length == sizeof(TPacketGuildExpUpdate);
        case 21:
            return length == sizeof(TPacketGDGuildAddMember);
        case 22:
        case 23:
        case 25:
            return length == sizeof(TPacketGuild);
        case 24:
            return length == sizeof(TPacketGuildChangeMemberData);
        case 26:
            return length == sizeof(TPacketGuildWar);
        case 27:
            return length == sizeof(TPacketGuildWarScore);
        case 28:
            return length == sizeof(std::uint32_t);
        case 35:
        case 42:
        case 109:
            return length == sizeof(std::uint32_t);
        case 30:
            return length == sizeof(TPlayerItem);
        case 31:
            return length == sizeof(std::uint32_t[2]);
        case 61:
            return length == sizeof(std::uint32_t);
        case 62:
            return length == sizeof(std::uint32_t[2]);
        case 116:
            return length == sizeof(std::uint32_t[2]);
        case 32:
            return length == sizeof(TPacketGDAddAffect);
        case 33:
            return length == sizeof(TPacketGDRemoveAffect);
        case 34:
            return IsTextPacket<TPacketGDHighscore>(data, length, &TPacketGDHighscore::szBoard);
        case 36:
            return length == sizeof(TPacketPartyCreate);
        case 37:
            return length == sizeof(TPacketPartyDelete);
        case 38:
            return length == sizeof(TPacketPartyAdd);
        case 39:
            return length == sizeof(TPacketPartyRemove);
        case 40:
            return length == sizeof(TPacketPartyStateChange);
        case 43:
        case 127:
            return length == 0;
        case 44:
            return IsTextPacket<TPacketGDChangeName>(data, length, &TPacketGDChangeName::name);
        case 45:
            return IsTextPacket<TPacketGDSMS>(data, length,
                    &TPacketGDSMS::szFrom, &TPacketGDSMS::szTo, &TPacketGDSMS::szMobile, &TPacketGDSMS::szMsg);
        case 46:
            return length == sizeof(TPacketGuildLadderPoint);
        case 47:
            return length == sizeof(TPacketGuildUseSkill);
        case 48:
            return length == sizeof(TPacketGiveEmpirePriv);
        case 49:
            return length == sizeof(TPacketGiveGuildPriv);
        case 50:
            return length == sizeof(TPacketMoneyLog);
        case 51:
        case 52:
            return length == sizeof(TPacketGDGuildMoney);
        case 53:
            return length == sizeof(TPacketGDGuildMoneyWithdrawGiveReply);
        case 54:
            return length == sizeof(TPacketGiveCharacterPriv);
        case 55:
            return IsTextPacket<TPacketSetEventFlag>(data, length, &TPacketSetEventFlag::szFlagName);
        case 56:
            return length == sizeof(TPacketPartySetMemberLevel);
        case 57:
            return IsTextPacket<TPacketGDGuildWarBet>(data, length, &TPacketGDGuildWarBet::szLogin);
        case 60:
            return length == sizeof(TPacketGDCreateObject);
        case 70:
            return IsTextPacket<TPacketMarriageAdd>(data, length,
                    &TPacketMarriageAdd::szName1, &TPacketMarriageAdd::szName2);
        case 71:
            return length == sizeof(TPacketMarriageUpdate);
        case 72:
            return length == sizeof(TPacketMarriageRemove);
        case 73:
            return length == sizeof(TPacketWeddingRequest);
        case 74:
            return length == sizeof(TPacketWeddingReady);
        case 75:
            return length == sizeof(TPacketWeddingEnd);
        case 100:
            return IsTextPacket<TPacketGDAuthLogin>(data, length,
                    &TPacketGDAuthLogin::szLogin, &TPacketGDAuthLogin::szSocialID);
        case 101:
            return IsTextPacket<TPacketGDLoginByKey>(data, length,
                    &TPacketGDLoginByKey::szLogin, &TPacketGDLoginByKey::szIP);
        case 104:
            return IsTextPacket<TPacketBillingExpire>(data, length, &TPacketBillingExpire::szLogin);
        case 105:
            return IsTextPacket<TPacketGDVCard>(data, length,
                    &TPacketGDVCard::szSellCharacter, &TPacketGDVCard::szSellAccount, &TPacketGDVCard::szBuyCharacter, &TPacketGDVCard::szBuyAccount);
        case 106:
            return length >= sizeof(std::uint32_t) &&
                   HasPacketRecords(length - sizeof(std::uint32_t), sizeof(std::uint32_t), ReadPacketDWORD(data));
        case 108:
        {
            if (length < sizeof(TPacketMyshopPricelistHeader))
                return false;
            const auto& packet = *reinterpret_cast<const TPacketMyshopPricelistHeader*>(data);
            return packet.byCount <= SHOP_PRICELIST_MAX_NUM &&
                   HasPacketRecords(length - sizeof(packet), sizeof(TItemPriceInfo), packet.byCount);
        }
        case 110:
            return IsTextPacket<TPacketBlockChat>(data, length, &TPacketBlockChat::szName);
        case 115:
            return length == 0 || IsTextPacket<TPacketReloadAdmin>(data, length, &TPacketReloadAdmin::szIP);
        case 128:
            return IsTextPacket<TPacketBlockException>(data, length, &TPacketBlockException::login);
        case 255:
        {
            if (length < sizeof(TPacketGDSetup))
                return false;
            const auto& packet = *reinterpret_cast<const TPacketGDSetup*>(data);
            if (!HasTerminator(packet.szPublicIP) || packet.bAuthServer > 1 ||
                (packet.bAuthServer && packet.dwLoginCount != 0) ||
                !HasPacketRecords(length - sizeof(packet), sizeof(TPacketLoginOnSetup), packet.dwLoginCount))
                return false;

            const char* login = data + sizeof(packet);
            for (DWORD i = 0; i < packet.dwLoginCount; ++i, login += sizeof(TPacketLoginOnSetup))
            {
                if (!IsTextPacket<TPacketLoginOnSetup>(login, sizeof(TPacketLoginOnSetup),
                        &TPacketLoginOnSetup::szLogin, &TPacketLoginOnSetup::szSocialID, &TPacketLoginOnSetup::szHost))
                    return false;
            }
            return true;
        }
    default:
        // Unknown and disabled opcodes are never decoded by the dispatch.
        return true;
    }
}
}

void CClientManager::ProcessPackets(CPeer* peer)
{
    BYTE header;
    DWORD handle;
    DWORD length;
    const char* payload = NULL;
    int processed = 0;
    int packetCount = 0;
    while (peer->PeekPacket(processed, header, handle, length, &payload))
    {
        if (!IsValidGamePacket(header, payload, length))
        {
            sys_err("Malformed DB packet header %u length %lu from peer %lu",
                    static_cast<unsigned>(header), length, peer->GetHandle());
            continue;
        }

        if (peer != m_pkAuthPeer && !peer->GetChannel() && header != 9 && header != 255)
            continue;
        if ((header == 100 || header == 106) && peer != m_pkAuthPeer)
            continue;

        char* data = const_cast<char*>(payload);
        ++packetCount;
        m_bLastHeader = header;

        switch (header)
        {
            case 2:
                QUERY_LOGOUT(peer, handle, data);
                break;
            case 3:
                sys_log(1, "HEADER_GD_PLAYER_LOAD (handle: %d length: %d)",
                        static_cast<int>(handle), static_cast<int>(length));
                QUERY_PLAYER_LOAD(peer, handle, reinterpret_cast<TPlayerLoadPacket*>(data));
                break;
            case 4:
                sys_log(1, "HEADER_GD_PLAYER_SAVE (handle: %d length: %d)",
                        static_cast<int>(handle), static_cast<int>(length));
                QUERY_PLAYER_SAVE(peer, handle, reinterpret_cast<TPlayerTable*>(data));
                break;
            case 5:
                sys_log(0, "HEADER_GD_PLAYER_CREATE (handle: %d length: %d)",
                        static_cast<int>(handle), static_cast<int>(length));
                __QUERY_PLAYER_CREATE(peer, handle, reinterpret_cast<TPlayerCreatePacket*>(data));
                sys_log(0, "END");
                break;
            case 6:
                sys_log(1, "HEADER_GD_PLAYER_DELETE (handle: %d length: %d)",
                        static_cast<int>(handle), static_cast<int>(length));
                __QUERY_PLAYER_DELETE(peer, handle, reinterpret_cast<TPlayerDeletePacket*>(data));
                break;
            case 9:
                QUERY_BOOT(peer, reinterpret_cast<TPacketGDBoot*>(data));
                break;
            case 10:
                QUERY_PLAYER_COUNT(peer, reinterpret_cast<TPlayerCountPacket*>(data));
                break;
            case 11:
                QUERY_QUEST_SAVE(peer, reinterpret_cast<TQuestTable*>(data), length);
                break;
            case 12:
                QUERY_SAFEBOX_LOAD(peer, handle, reinterpret_cast<TSafeboxLoadPacket*>(data), false);
                break;
            case 13:
                QUERY_SAFEBOX_SAVE(peer, reinterpret_cast<TSafeboxTable*>(data));
                break;
            case 14:
                QUERY_SAFEBOX_CHANGE_SIZE(peer, handle, reinterpret_cast<TSafeboxChangeSizePacket*>(data));
                break;
            case 15:
                QUERY_EMPIRE_SELECT(peer, handle, reinterpret_cast<TEmpireSelectPacket*>(data));
                break;
            case 16:
                QUERY_SAFEBOX_CHANGE_PASSWORD(peer, handle, reinterpret_cast<TSafeboxChangePasswordPacket*>(data));
                break;
            case 19:
                GuildSkillUpdate(peer, reinterpret_cast<TPacketGuildSkillUpdate*>(data));
                break;
            case 20:
                GuildExpUpdate(peer, reinterpret_cast<TPacketGuildExpUpdate*>(data));
                break;
            case 21:
                GuildAddMember(peer, reinterpret_cast<TPacketGDGuildAddMember*>(data));
                break;
            case 22:
                GuildRemoveMember(peer, reinterpret_cast<TPacketGuild*>(data));
                break;
            case 23:
                GuildChangeGrade(peer, reinterpret_cast<TPacketGuild*>(data));
                break;
            case 24:
                GuildChangeMemberData(peer, reinterpret_cast<TPacketGuildChangeMemberData*>(data));
                break;
            case 25:
                GuildDisband(peer, reinterpret_cast<TPacketGuild*>(data));
                break;
            case 26:
                GuildWar(peer, reinterpret_cast<TPacketGuildWar*>(data));
                break;
            case 27:
                GuildWarScore(peer, reinterpret_cast<TPacketGuildWarScore*>(data));
                break;
            case 28:
            {
                std::uint32_t guildId;
                memcpy(&guildId, data, sizeof(guildId));
                GuildCreate(peer, guildId);
                break;
            }
            case 30:
                QUERY_ITEM_SAVE(peer, data);
                break;
            case 31:
                QUERY_ITEM_DESTROY(peer, data);
                break;
            case 32:
                QUERY_ADD_AFFECT(peer, reinterpret_cast<TPacketGDAddAffect*>(data));
                break;
            case 33:
                QUERY_REMOVE_AFFECT(peer, reinterpret_cast<TPacketGDRemoveAffect*>(data));
                break;
            case 34:
                QUERY_HIGHSCORE_REGISTER(peer, reinterpret_cast<TPacketGDHighscore*>(data));
                break;
            case 35:
                QUERY_ITEM_FLUSH(peer, data);
                break;
            case 36:
                QUERY_PARTY_CREATE(peer, reinterpret_cast<TPacketPartyCreate*>(data));
                break;
            case 37:
                QUERY_PARTY_DELETE(peer, reinterpret_cast<TPacketPartyDelete*>(data));
                break;
            case 38:
                QUERY_PARTY_ADD(peer, reinterpret_cast<TPacketPartyAdd*>(data));
                break;
            case 39:
                QUERY_PARTY_REMOVE(peer, reinterpret_cast<TPacketPartyRemove*>(data));
                break;
            case 40:
                QUERY_PARTY_STATE_CHANGE(peer, reinterpret_cast<TPacketPartyStateChange*>(data));
                break;
            case 42:
                QUERY_FLUSH_CACHE(peer, data);
                break;
            case 43:
                QUERY_RELOAD_PROTO();
                break;
            case 44:
                QUERY_CHANGE_NAME(peer, handle, reinterpret_cast<TPacketGDChangeName*>(data));
                break;
            case 45:
                QUERY_SMS(peer, reinterpret_cast<TPacketGDSMS*>(data));
                break;
            case 46:
                GuildChangeLadderPoint(reinterpret_cast<TPacketGuildLadderPoint*>(data));
                break;
            case 47:
                GuildUseSkill(reinterpret_cast<TPacketGuildUseSkill*>(data));
                break;
            case 48:
                AddEmpirePriv(reinterpret_cast<TPacketGiveEmpirePriv*>(data));
                break;
            case 49:
                AddGuildPriv(reinterpret_cast<TPacketGiveGuildPriv*>(data));
                break;
            case 50:
                MoneyLog(reinterpret_cast<TPacketMoneyLog*>(data));
                break;
            case 51:
                GuildDepositMoney(reinterpret_cast<TPacketGDGuildMoney*>(data));
                break;
            case 52:
                GuildWithdrawMoney(peer, reinterpret_cast<TPacketGDGuildMoney*>(data));
                break;
            case 53:
                GuildWithdrawMoneyGiveReply(reinterpret_cast<TPacketGDGuildMoneyWithdrawGiveReply*>(data));
                break;
            case 54:
                AddCharacterPriv(reinterpret_cast<TPacketGiveCharacterPriv*>(data));
                break;
            case 55:
                SetEventFlag(reinterpret_cast<TPacketSetEventFlag*>(data));
                break;
            case 56:
                QUERY_PARTY_SET_MEMBER_LEVEL(peer, reinterpret_cast<TPacketPartySetMemberLevel*>(data));
                break;
            case 57:
                GuildWarBet(reinterpret_cast<TPacketGDGuildWarBet*>(data));
                break;
            case 60:
                CreateObject(reinterpret_cast<TPacketGDCreateObject*>(data));
                break;
            case 61:
            {
                std::uint32_t id;
                memcpy(&id, data, sizeof(id));
                DeleteObject(id);
                break;
            }
            case 62:
            {
                std::uint32_t fields[2];
                memcpy(fields, data, sizeof(fields));
                UpdateLand(fields);
                break;
            }
            case 70:
                MarriageAdd(reinterpret_cast<TPacketMarriageAdd*>(data));
                break;
            case 71:
                MarriageUpdate(reinterpret_cast<TPacketMarriageUpdate*>(data));
                break;
            case 72:
                MarriageRemove(reinterpret_cast<TPacketMarriageRemove*>(data));
                break;
            case 73:
                WeddingRequest(reinterpret_cast<TPacketWeddingRequest*>(data));
                break;
            case 74:
                WeddingReady(reinterpret_cast<TPacketWeddingReady*>(data));
                break;
            case 75:
                WeddingEnd(reinterpret_cast<TPacketWeddingEnd*>(data));
                break;
            case 100:
                QUERY_AUTH_LOGIN(peer, handle, reinterpret_cast<TPacketGDAuthLogin*>(data));
                break;
            case 101:
                QUERY_LOGIN_BY_KEY(peer, handle, reinterpret_cast<TPacketGDLoginByKey*>(data));
                break;
            case 104:
                BillingExpire(reinterpret_cast<TPacketBillingExpire*>(data));
                break;
            case 105:
                VCard(reinterpret_cast<TPacketGDVCard*>(data));
                break;
            case 106:
                BillingCheck(data);
                break;
            case 107:
                QUERY_SAFEBOX_LOAD(peer, handle, reinterpret_cast<TSafeboxLoadPacket*>(data), true);
                break;
            case 108:
                MyshopPricelistUpdate(reinterpret_cast<TPacketMyshopPricelistHeader*>(data));
                break;
            case 109:
                MyshopPricelistRequest(peer, handle, ReadPacketDWORD(data));
                break;
            case 110:
                BlockChat(reinterpret_cast<TPacketBlockChat*>(data));
                break;
            case 114:
                // The legacy timing-control packet has no legitimate game operation.
                break;
            case 115:
                ReloadAdmin(peer, length ? reinterpret_cast<TPacketReloadAdmin*>(data) : nullptr);
                break;
            case 116:
                BreakMarriage(peer, data);
                break;
            case 117:
            case 118:
            case 119:
            case 120:
            case 121:
            case 122:
            case 123:
            case 124:
            case 125:
            case 126:
                // Monarch requests are outside the reconstruction scope.
                break;
            case 127:
                CBlockCountry::instance().SendBlockedCountryIp(peer);
                CBlockCountry::instance().SendBlockException(peer);
                break;
            case 128:
                BlockException(reinterpret_cast<TPacketBlockException*>(data));
                break;
            case 255:
                QUERY_SETUP(peer, handle, data);
                break;
            default:
                sys_err("Unknown header (header: %d handle: %d length: %d)",
                        header, static_cast<int>(handle), static_cast<int>(length));
                break;
        }
    }

    peer->RecvEnd(processed);
}


void CClientManager::AddGuildPriv(TPacketGiveGuildPriv* packet)
{
    CPrivManager::instance().AddGuildPriv(packet->guild_id, packet->type,
                                          packet->value, packet->duration_sec);
}

void CClientManager::AddEmpirePriv(TPacketGiveEmpirePriv* packet)
{
    CPrivManager::instance().AddEmpirePriv(packet->empire, packet->type,
                                           packet->value, packet->duration_sec);
}

void CClientManager::AddCharacterPriv(TPacketGiveCharacterPriv* packet)
{
    CPrivManager::instance().AddCharPriv(packet->pid, packet->type, packet->value);
}

void CClientManager::MoneyLog(TPacketMoneyLog* packet)
{
    CMoneyLog::instance().AddLog(packet->type, packet->vnum, packet->gold);
}

void CClientManager::GuildDepositMoney(TPacketGDGuildMoney* packet)
{
    CGuildManager::instance().DepositMoney(packet->dwGuild, packet->lGold);
}

void CClientManager::GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* packet)
{
    CGuildManager::instance().WithdrawMoney(peer, packet->dwGuild, packet->lGold);
}

void CClientManager::GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* packet)
{
    CGuildManager::instance().WithdrawMoneyReply(packet->dwGuild, packet->bGiveSuccess,
                                                 packet->lChangeGold);
}

void CClientManager::GuildWarBet(TPacketGDGuildWarBet* packet)
{
    CGuildManager::instance().Bet(packet->dwWarID, packet->szLogin,
                                   packet->dwGold, packet->dwGuild);
}

void CClientManager::MarriageAdd(TPacketMarriageAdd* packet)
{
    marriage::CManager::instance().Add(packet->dwPID1, packet->dwPID2, packet->szName1, packet->szName2);
}

void CClientManager::MarriageUpdate(TPacketMarriageUpdate* packet)
{
    marriage::CManager::instance().Update(packet->dwPID1, packet->dwPID2, packet->lLovePoint, packet->byMarried);
}

void CClientManager::MarriageRemove(TPacketMarriageRemove* packet)
{
    marriage::CManager::instance().Remove(packet->dwPID1, packet->dwPID2);
}

void CClientManager::WeddingRequest(TPacketWeddingRequest* packet)
{
    ForwardPacket(153, packet, sizeof(*packet));
}

void CClientManager::WeddingReady(TPacketWeddingReady* packet)
{
    ForwardPacket(154, packet, sizeof(*packet));
    marriage::CManager::instance().ReadyWedding(packet->dwMapIndex, packet->dwPID1, packet->dwPID2);
}

void CClientManager::WeddingEnd(TPacketWeddingEnd* packet)
{
    marriage::CManager::instance().EndWedding(packet->dwPID1, packet->dwPID2);
}

void CClientManager::BreakMarriage(CPeer*, void* data)
{
    std::uint32_t ids[2];
    memcpy(ids, data, sizeof(ids));
    marriage::CManager::instance().Remove(ids[0], ids[1]);
}

void CClientManager::BlockException(TPacketBlockException* packet)
{
    const auto escaped = dbsql::EscapeField(packet->login, SQL_ACCOUNT);
    if (!escaped)
        return;

    const char* login = packet->login;

    char query[1024] = {};
    CBlockCountry& blocks = CBlockCountry::instance();
    if (packet->cmd == 1)
    {
        dbsql::sprintf(query, "INSERT INTO block_exception VALUES('%s')", escaped->c_str());
        CDBManager::instance().AsyncQuery(query, SQL_ACCOUNT);
        blocks.AddBlockException(login);
    }
    else if (packet->cmd == 2)
    {
        dbsql::sprintf(query, "DELETE FROM block_exception WHERE login='%s'", escaped->c_str());
        CDBManager::instance().AsyncQuery(query, SQL_ACCOUNT);
        blocks.DelBlockException(login);
    }
    else
        return;

    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        if ((*it)->GetChannel())
            blocks.SendBlockExceptionOne(*it, login, packet->cmd);
    }
}

void CClientManager::QUERY_FLUSH_CACHE(CPeer*, const char* data)
{
    const DWORD pid = ReadPacketDWORD(data);
    CPlayerTableCache* cache = GetPlayerCache(pid);
    if (!cache)
        return;

    cache->Flush();
    FlushItemCacheSet(pid);
    m_map_playerCache.erase(pid);
    delete cache;
}

void CClientManager::QUERY_ITEM_FLUSH(CPeer*, const char* data)
{
    const DWORD id = ReadPacketDWORD(data);

    CItemCache* cache = GetItemCache(id);
    if (cache)
        cache->Flush();
}

void CClientManager::QUERY_ITEM_SAVE(CPeer* peer, const char* data)
{
    TPlayerItem* item = reinterpret_cast<TPlayerItem*>(const_cast<char*>(data));
    if (item->window != SAFEBOX && item->window != MALL)
    {
        if (g_test_server)
            sys_log(0, "QUERY_ITEM_SAVE => PutItemCache() owner %d id %d vnum %d ",
                    static_cast<int>(item->owner), static_cast<int>(item->id), static_cast<int>(item->vnum));
        PutItemCache(item, false);
        return;
    }

    CItemCache* cache = GetItemCache(item->id);
    if (cache)
    {
        TItemCacheSet* set = GetItemCacheSet(cache->Get()->owner);
        if (set)
            set->erase(cache);
        m_map_itemCache.erase(item->id);
        delete cache;
    }

    char query[512];
    dbsql::sprintf(query,
            "REPLACE INTO item%s (id, owner_id, `window`, pos, count, vnum, socket0, socket1, socket2,"
            "attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, "
            "attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6) "
            "VALUES(%u, %u, %d, %d, %u, %u, %u, %u, %u, %d, %d, %d, %d, "
            "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
            GetTablePostfix(), item->id, item->owner, item->window, item->pos, item->count, item->vnum,
            static_cast<std::uint32_t>(item->alSockets[0]),
            static_cast<std::uint32_t>(item->alSockets[1]),
            static_cast<std::uint32_t>(item->alSockets[2]),
            item->aAttr[0].bType, item->aAttr[0].sValue, item->aAttr[1].bType, item->aAttr[1].sValue,
            item->aAttr[2].bType, item->aAttr[2].sValue, item->aAttr[3].bType, item->aAttr[3].sValue,
            item->aAttr[4].bType, item->aAttr[4].sValue, item->aAttr[5].bType, item->aAttr[5].sValue,
            item->aAttr[6].bType, item->aAttr[6].sValue);
    CDBManager::instance().ReturnQuery(query, QID_ITEM_SAVE, peer->GetHandle(), NULL);
}

void CClientManager::QUERY_ITEM_DESTROY(CPeer* peer, const char* data)
{
    std::uint32_t fields[2];
    memcpy(fields, data, sizeof(fields));
    if (DeleteItemCache(fields[0]))
        return;

    char query[64];
    dbsql::sprintf(query, "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), fields[0]);

    if (fields[1] == 0)
        CDBManager::instance().AsyncQuery(query);
    else
        CDBManager::instance().ReturnQuery(query, QID_ITEM_DESTROY, peer->GetHandle(), NULL);
}

void CClientManager::QUERY_QUEST_SAVE(CPeer* peer, TQuestTable* quest, DWORD length)
{
    if (length % sizeof(*quest) != 0)
    {
        sys_err("invalid packet size %d, sizeof(TQuestTable) == %d",
                static_cast<int>(length), static_cast<int>(sizeof(*quest)));
        return;
    }

    for (DWORD count = length / sizeof(*quest); count != 0; --count, ++quest)
    {
        const auto name = dbsql::EscapeField(quest->szName);
        const auto state = dbsql::EscapeField(quest->szState);
        if (!name || !state)
            return;

        char query[1025];
        if (quest->lValue == 0)
        {
            dbsql::snprintf(query, 1024, "DELETE FROM quest%s WHERE dwPID=%ld AND szName='%s' AND szState='%s'",
                     GetTablePostfix(), static_cast<long>(quest->dwPID), name->c_str(), state->c_str());
        }
        else
        {
            dbsql::snprintf(query, 1024,
                     "REPLACE INTO quest%s (dwPID, szName, szState, lValue) VALUES(%ld, '%s', '%s', %ld)",
                     GetTablePostfix(), static_cast<long>(quest->dwPID), name->c_str(), state->c_str(),
                     static_cast<long>(quest->lValue));
        }
        query[1024] = '\0';
        CDBManager::instance().ReturnQuery(query, QID_QUEST_SAVE, peer->GetHandle(), NULL);
    }
}

void CClientManager::SendAllGuildSkillRechargePacket()
{
    ForwardPacket(57, NULL, 0);
}

void CClientManager::InsertLoginData(CLoginData* loginData)
{
    char login[LOGIN_MAX_LEN + 1];
    trim_and_lower(login, loginData->GetRef().login, LOGIN_MAX_LEN);

    m_map_pkLoginData.insert(std::make_pair(loginData->GetKey(), loginData));
    m_map_pkLoginDataByLogin.insert(std::make_pair(login, loginData));
    m_map_pkLoginDataByAID.insert(std::make_pair(loginData->GetRef().id, loginData));
}

void CClientManager::DeleteLoginData(CLoginData* loginData)
{
    m_map_pkLoginData.erase(loginData->GetKey());
    m_map_pkLoginDataByLogin.erase(loginData->GetRef().login);
    m_map_pkLoginDataByAID.erase(loginData->GetRef().id);

    if (m_map_kLogonAccount.find(loginData->GetRef().login) == m_map_kLogonAccount.end())
        delete loginData;
    else
        loginData->SetDeleted(true);
}

void CClientManager::QUERY_AUTH_LOGIN(CPeer* peer, DWORD handle, TPacketGDAuthLogin* packet)
{
    CLoginData* previous = GetLoginDataByLogin(packet->szLogin);
    CLoginData* byAccount = GetLoginDataByAID(packet->dwID);

    BYTE result = 0;
    if (!packet->dwID || !packet->dwLoginKey || !*packet->szLogin ||
        GetLoginData(packet->dwLoginKey) || (byAccount && byAccount != previous) ||
        (previous && previous->GetRef().id != packet->dwID))
    {
        sys_err("Duplicate auth login key for account %lu", static_cast<unsigned long>(packet->dwID));
    }
    else
    {
        if (previous)
            DeleteLoginData(previous);
        CLoginData* loginData = new CLoginData;
        loginData->SetKey(packet->dwLoginKey);
        DWORD clientKey[4];
        for (int i = 0; i < 4; ++i)
            clientKey[i] = packet->adwClientKey[i];
        loginData->SetClientKey(clientKey);
        loginData->SetBillType(packet->bBillType);
        loginData->SetBillID(packet->dwBillID);
        loginData->SetPremium(packet->iPremiumTimes);

        TAccountTable& account = loginData->GetRef();
        account.id = packet->dwID;
        trim_and_lower(account.login, packet->szLogin, LOGIN_MAX_LEN);
        strncpy(account.social_id, packet->szSocialID, 18);
        account.social_id[18] = '\0';
        strncpy(account.passwd, "TEMP", 17);

        result = 1;
        InsertLoginData(loginData);
    }

    peer->EncodeHeader(122, handle, sizeof(result));
    peer->EncodeBYTE(result);
}

void CClientManager::BillingExpire(TPacketBillingExpire* packet)
{
    char key[LOGIN_MAX_LEN + 1];
    trim_and_lower(key, packet->szLogin, LOGIN_MAX_LEN);

    switch (packet->bBillType)
    {
    case 3: // BILLING_IP_TIME
    case 4: // BILLING_IP_DAY
        {
            const DWORD ipID = static_cast<DWORD>(atoi(packet->szLogin));
            TLogonAccountMap::iterator it = m_map_kLogonAccount.begin();
            while (it != m_map_kLogonAccount.end())
            {
                CLoginData* loginData = (it++)->second;
                if (loginData->GetBillID() != ipID)
                    continue;

                CPeer* peer = GetPeer(loginData->GetConnectedPeerHandle());
                if (!peer)
                    continue;

                strncpy(packet->szLogin, loginData->GetRef().login, LOGIN_MAX_LEN);
                packet->szLogin[LOGIN_MAX_LEN] = '\0';
                peer->EncodeHeader(129, 0, sizeof(*packet));
                peer->Encode(packet, sizeof(*packet));
            }
        }
        break;
    case 5: // BILLING_TIME
    case 6: // BILLING_DAY
        {
            TLogonAccountMap::iterator it = m_map_kLogonAccount.find(key);
            if (it != m_map_kLogonAccount.end())
            {
                CPeer* peer = GetPeer(it->second->GetConnectedPeerHandle());
                if (peer)
                {
                    peer->EncodeHeader(129, 0, sizeof(*packet));
                    peer->Encode(packet, sizeof(*packet));
                }
            }
        }
        break;
    }
}

void CClientManager::BillingCheck(const char* data)
{
    if (!m_pkAuthPeer)
        return;

    const time_t now = GetCurrentTime();
    std::uint32_t count;
    memcpy(&count, data, sizeof(count));
    data += sizeof(count);

    std::vector<std::uint32_t> expired;

    for (DWORD i = 0; i < count; ++i)
    {
        std::uint32_t key;
        memcpy(&key, data, sizeof(key));
        data += sizeof(key);

        std::map<DWORD, CLoginData*>::iterator it = m_map_pkLoginData.find(key);
        if (it == m_map_pkLoginData.end())
        {
            expired.push_back(key);
        }
        else if (!it->second->IsPlay() && now - it->second->GetLastPlayTime() > 180)
        {
            expired.push_back(key);
        }
    }

    const DWORD bodySize = static_cast<DWORD>(sizeof(std::uint32_t) * expired.size());
    m_pkAuthPeer->EncodeHeader(132, 0, sizeof(std::uint32_t) + bodySize);
    const auto expiredCount = static_cast<std::uint32_t>(expired.size());
    m_pkAuthPeer->Encode(&expiredCount, sizeof(expiredCount));
    if (!expired.empty())
        m_pkAuthPeer->Encode(&expired[0], bodySize);
}

namespace
{
#pragma pack(push, 1)
    struct BillingRepairRecord
    {
        std::uint32_t loginKey;
        char login[31];
        char host[16];
    };
#pragma pack(pop)
    static_assert(sizeof(BillingRepairRecord) == 51, "2009 billing repair record");
}

void CClientManager::SendAllLoginToBilling()
{
    if (!m_pkAuthPeer)
        return;

    std::vector<BillingRepairRecord> records;
    TLogonAccountMap::iterator it = m_map_kLogonAccount.begin();
    while (it != m_map_kLogonAccount.end())
    {
        CLoginData* loginData = (it++)->second;
        BillingRepairRecord record;
        record.loginKey = loginData->GetKey();
        strncpy(record.login, loginData->GetRef().login, sizeof(record.login) - 1);
        record.login[sizeof(record.login) - 1] = '\0';
        strncpy(record.host, loginData->GetIP(), sizeof(record.host) - 1);
        record.host[sizeof(record.host) - 1] = '\0';
        records.push_back(record);
    }

    if (!records.empty())
    {
        const DWORD bodySize = static_cast<DWORD>(sizeof(BillingRepairRecord) * records.size());
        m_pkAuthPeer->EncodeHeader(128, 0, sizeof(std::uint32_t) + bodySize);
        m_pkAuthPeer->EncodeDWORD(static_cast<DWORD>(records.size()));
        m_pkAuthPeer->Encode(&records[0], bodySize);
    }
}


void CClientManager::QUERY_EMPIRE_SELECT(CPeer* peer, DWORD handle, TEmpireSelectPacket* packet)
{
    char query[8192];
    CDBManager& db = CDBManager::instance();

    dbsql::snprintf(query, sizeof(query), "UPDATE player_index%s SET empire=%u WHERE id=%lu",
             GetTablePostfix(), static_cast<unsigned int>(packet->bEmpire), static_cast<unsigned long>(packet->dwAccountID));
    delete db.DirectQuery(query);

    dbsql::snprintf(query, sizeof(query), "SELECT pid1, pid2, pid3, pid4 FROM player_index%s WHERE id=%lu",
             GetTablePostfix(), static_cast<unsigned long>(packet->dwAccountID));
    SQLMsg* message = db.DirectQuery(query);
    if (message->Get()->iNumRows != 0)
    {
        MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
        const long maps[4] = {0, 1, 21, 41};
        const DWORD positions[4][2] = {{0, 0}, {0x72934, 0xeb668},
                                      {0xd994, 0x268cc}, {0xecb80, 0x43f80}};
        const BYTE empire = packet->bEmpire;
        for (int i = 0; i < 3; ++i)
        {
            const int pid = atoi(row[i]);
            if (pid == 0)
                continue;

            dbsql::snprintf(query, sizeof(query), "UPDATE player%s SET map_index=%ld,x=%lu,y=%lu WHERE id=%lu",
                     GetTablePostfix(), maps[empire], positions[empire][0], positions[empire][1],
                     static_cast<unsigned long>(static_cast<std::uint32_t>(pid)));
            delete db.DirectQuery(query);
        }
    }

    delete message;
    peer->EncodeHeader(49, handle, 1);
    peer->EncodeBYTE(packet->bEmpire);
}

void CClientManager::QUERY_SMS(CPeer*, TPacketGDSMS* packet)
{
    const auto sender = dbsql::EscapeField(packet->szFrom);
    const auto receiver = dbsql::EscapeField(packet->szTo);
    const auto mobile = dbsql::EscapeField(packet->szMobile);
    const auto message = dbsql::EscapeField(packet->szMsg);
    if (!sender || !receiver || !mobile || !message)
        return;

    char query[8192];
    dbsql::snprintf(query, sizeof(query),
             "INSERT INTO sms_pool (server, sender, receiver, mobile, msg) VALUES(%d, '%s', '%s', '%s', '%s')",
             (m_iPlayerIDStart + 2) / 3, sender->c_str(), receiver->c_str(), mobile->c_str(), message->c_str());
    CDBManager::instance().AsyncQuery(query);
}

void CClientManager::VCard(TPacketGDVCard* packet)
{
    m_queue_vcard.push(*packet);
}

void CClientManager::VCardProcess()
{
    if (!m_pkAuthPeer)
        return;

    while (!m_queue_vcard.empty())
    {
        m_pkAuthPeer->EncodeHeader(131, 0, sizeof(TPacketGDVCard));
        m_pkAuthPeer->Encode(&m_queue_vcard.front(), sizeof(TPacketGDVCard));
        m_queue_vcard.pop();
    }
}

void CClientManager::BlockChat(TPacketBlockChat* packet)
{
    const auto name = dbsql::EscapeField(packet->szName);
    if (!name)
        return;
    char query[256];
    const char* format = g_stLocale == "sjis"
        ? "SELECT id FROM player%s WHERE name = '%s' collate sjis_japanese_ci"
        : "SELECT id FROM player%s WHERE name = '%s'";
    dbsql::snprintf(query, 255, format, GetTablePostfix(), name->c_str());

    SQLMsg* message = CDBManager::instance().DirectQuery(query);
    if (message->Get()->iNumRows != 0)
    {
        MYSQL_ROW row = mysql_fetch_row(message->Get()->pSQLResult);
        TPacketGDAddAffect affect;
        affect.dwPID = strtoul(row[0], NULL, 10);
        affect.elem.dwType = 223;
        affect.elem.bApplyOn = 0;
        affect.elem.lApplyValue = 0;
        affect.elem.dwFlag = 0;
        affect.elem.lDuration = packet->lDuration;
        affect.elem.lSPCost = 0;
        QUERY_ADD_AFFECT(NULL, &affect);
    }
    delete message;
}



bool CreateItemTableFromRes(MYSQL_RES*, std::vector<TPlayerItem>*, DWORD);

namespace
{
enum
{
    SafeboxLoad = 45,
    SafeboxChangeSize = 46,
    SafeboxWrongPassword = 47,
    SafeboxChangePasswordAnswer = 48,
    MallLoad = 51,
    SafeboxWindow = 3,
    MallWindow = 4,
    SafeboxUniqueItemType = 16
};
}

void CClientManager::QUERY_SAFEBOX_LOAD(CPeer* peer, DWORD handle, TSafeboxLoadPacket* packet, bool mall)
{
    ClientHandleInfo* info = new ClientHandleInfo(handle);
    strncpy(info->safebox_password, packet->szPassword, sizeof(info->safebox_password) - 1);
    info->safebox_password[sizeof(info->safebox_password) - 1] = '\0';
    info->account_id = packet->dwID;
    info->account_index = 0;
    info->ip[0] = mall;
    strncpy(info->login, packet->szLogin, sizeof(info->login) - 1);
    info->login[sizeof(info->login) - 1] = '\0';

    char query[8193];
    dbsql::snprintf(query, 8192, "SELECT account_id, size, password FROM safebox%s WHERE account_id=%u", GetTablePostfix(), packet->dwID);

    CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_LOAD, peer->GetHandle(), info);
}

void CClientManager::RESULT_SAFEBOX_LOAD(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    std::unique_ptr<ClientHandleInfo> info(static_cast<ClientHandleInfo*>(queryInfo->pvData));
    const DWORD handle = info->dwHandle;
    const bool mall = info->ip[0] != 0;
    const char* window = mall ? "MALL" : "SAFEBOX";

    if (info->account_index == 0)
    {
        std::unique_ptr<TSafeboxTable> safebox(new TSafeboxTable);
        memset(safebox.get(), 0, sizeof(TSafeboxTable));
        SQLResult* result = message->Get();

        if (result->iNumRows == 0)
        {
            if (strcmp("000000", info->safebox_password))
            {
                peer->EncodeHeader(SafeboxWrongPassword, handle, 0);
                return;
            }
        }
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
            const char* password = row[2] && *row[2] ? row[2] : "000000";
            if (strcmp(password, info->safebox_password))
            {
                peer->EncodeHeader(SafeboxWrongPassword, handle, 0);
                return;
            }

            safebox->dwID = row[0] ? atoi(row[0]) : 0;
            safebox->bSize = dbinput::ReadInteger<decltype(safebox->bSize)>(row[1]);
            if (mall)
                safebox->bSize = 1;
            sys_log(0, mall ? "MALL id[%d] size[%d]" : "SAFEBOX id[%d] size[%d]",
                    static_cast<int>(safebox->dwID), static_cast<int>(safebox->bSize));
        }

        if (!safebox->dwID)
            safebox->dwID = info->account_id;
        info->pSafebox = safebox.release();
        info->account_index = 1;

        char query[512];
        dbsql::sprintf(query,
                "SELECT id, `window`+0, pos, count, vnum, socket0, socket1, socket2,attrtype0, attrvalue0, "
                "attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, "
                "attrtype5, attrvalue5, attrtype6, attrvalue6 FROM item%s WHERE owner_id=%ld AND `window`='%s'",
                GetTablePostfix(), static_cast<long>(info->account_id), window);
        CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_LOAD, peer->GetHandle(), info.release());
        return;
    }

    if (!info->pSafebox)
    {
        sys_err("null safebox pointer!");
        return;
    }
    if (!message->Get()->pSQLResult)
    {
        sys_err("null safebox result");
        return;
    }

    static std::vector<TPlayerItem> items;
    CreateItemTableFromRes(message->Get()->pSQLResult, &items, info->account_id);

    std::set<TItemAward*>* awards = ItemAwardManager::instance().GetByLogin(info->login);
    if (awards && !m_vec_itemTable.empty())
    {
        CGrid grid(5, MAX(1, info->pSafebox->bSize) * 9);
        bool validItems = true;
        for (std::vector<TPlayerItem>::const_iterator it = items.begin(); it != items.end(); ++it)
        {
            std::map<DWORD, TItemTable*>::const_iterator proto = m_map_itemTableByVnum.find(it->vnum);
            if (proto == m_map_itemTableByVnum.end())
            {
                sys_err("invalid item vnum %lu in safebox: login %s", it->vnum, info->login);
                validItems = false;
                break;
            }

            grid.Put(it->pos, 1, proto->second->bSize);
        }

        if (validItems)
        {
            std::vector<std::pair<DWORD, DWORD> > finished;
            for (std::set<TItemAward*>::iterator it = awards->begin(); it != awards->end(); ++it)
            {
                TItemAward* award = *it;
                if (award->bTaken || award->bMall != mall)
                    continue;

                std::map<DWORD, TItemTable*>::const_iterator proto = m_map_itemTableByVnum.find(award->dwVnum);
                if (proto == m_map_itemTableByVnum.end())
                {
                    sys_err("invalid item vnum %lu in item_award: login %s", award->dwVnum, info->login);
                    continue;
                }

                int pos = grid.FindBlank(1, proto->second->bSize);
                if (pos == -1)
                    break;

                TPlayerItem item;
                memset(&item, 0, sizeof(item));
                DWORD socket2 = 0;
                if (proto->second->bType == SafeboxUniqueItemType)
                {
                    socket2 = award->dwSocket2 ? award->dwSocket2 : proto->second->alValues[0];
                }
                else if ((award->dwVnum == 50300 || award->dwVnum == 70037) && !award->dwSocket0)
                {
                    int index = number_ex(0, static_cast<int>(m_vec_skillTable.size()) - 1,
                            const_cast<char*>("ClientManager.cpp"), 0x2d7);
                    award->dwSocket0 = m_vec_skillTable[index].dwVnum;
                }

                if (m_dwNowItemID > m_dwItemIDStop)
                {
                    sys_err("Overflow Unique ID ");
                    break;
                }

                char query[257];
                dbsql::snprintf(query, 256,
                        "INSERT INTO item%s (id, owner_id, `window`, pos, vnum, count, socket0, socket1, socket2) "
                        "VALUES(%lu, %lu, '%s', %d, %lu, %lu, %lu, %lu, %lu)",
                        GetTablePostfix(), GainItemID(), info->account_id, window, pos, award->dwVnum,
                        award->dwCount, static_cast<unsigned long>(static_cast<std::uint32_t>(award->dwSocket0)),
                        static_cast<unsigned long>(static_cast<std::uint32_t>(award->dwSocket1)),
                        static_cast<unsigned long>(static_cast<std::uint32_t>(socket2)));
                std::unique_ptr<SQLMsg> inserted(CDBManager::instance().DirectQuery(query));
                SQLResult* result = inserted->Get();
                if (result->iAffectedRows <= 0 || result->iInsertID <= 0)
                    break;

                item.id = result->iInsertID;
                item.window = static_cast<BYTE>(mall ? MallWindow : SafeboxWindow);
                item.pos = static_cast<WORD>(pos);
                item.count = award->dwCount;
                item.vnum = award->dwVnum;
                item.alSockets[0] = award->dwSocket0;
                item.alSockets[1] = award->dwSocket1;
                item.alSockets[2] = socket2;
                items.push_back(item);
                finished.push_back(std::make_pair(award->dwID, item.id));
                grid.Put(pos, 1, proto->second->bSize);
            }

            for (std::vector<std::pair<DWORD, DWORD> >::const_iterator it = finished.begin(); it != finished.end(); ++it)
                ItemAwardManager::instance().Taken(it->first, it->second);
        }
    }

    if (items.size() > (std::numeric_limits<WORD>::max)())
    {
        sys_err("Safebox item count exceeds the packet limit");
        return;
    }
    info->pSafebox->wItemCount = static_cast<WORD>(items.size());
    DWORD itemBytes = sizeof(TPlayerItem) * items.size();
    peer->EncodeHeader(static_cast<BYTE>(mall ? MallLoad : SafeboxLoad), handle, sizeof(TSafeboxTable) + itemBytes);
    peer->Encode(info->pSafebox, sizeof(TSafeboxTable));
    if (!items.empty())
        peer->Encode(&items[0], itemBytes);
}

void CClientManager::QUERY_SAFEBOX_CHANGE_SIZE(CPeer* peer, DWORD handle, TSafeboxChangeSizePacket* packet)
{
    ClientHandleInfo* info = new ClientHandleInfo(handle);
    info->account_index = packet->bSize;

    char query[8193];
    if (packet->bSize == 1)
        dbsql::snprintf(query, 8192, "INSERT INTO safebox%s (account_id, size) VALUES(%u, %u)",
                GetTablePostfix(), packet->dwID, packet->bSize);
    else
        dbsql::snprintf(query, 8192, "UPDATE safebox%s SET size=%u WHERE account_id=%u",
                GetTablePostfix(), packet->bSize, packet->dwID);

    CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_CHANGE_SIZE, peer->GetHandle(), info);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_SIZE(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    DWORD handle = info->dwHandle;
    BYTE size = info->account_index;
    delete info;

    if (message->Get()->iNumRows > 0)
    {
        peer->EncodeHeader(SafeboxChangeSize, handle, sizeof(BYTE));
        peer->EncodeBYTE(size);
    }
}

void CClientManager::QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer* peer, DWORD handle, TSafeboxChangePasswordPacket* packet)
{
    ClientHandleInfo* info = new ClientHandleInfo(handle);
    strncpy(info->safebox_password, packet->szNewPassword, sizeof(info->safebox_password) - 1);
    info->safebox_password[sizeof(info->safebox_password) - 1] = '\0';
    strncpy(info->login, packet->szOldPassword, sizeof(info->login) - 1);
    info->login[sizeof(info->login) - 1] = '\0';
    info->account_id = packet->dwID;

    char query[8193];
    dbsql::snprintf(query, 8192, "SELECT password FROM safebox%s WHERE account_id = %u",
            GetTablePostfix(), packet->dwID);
    CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_CHANGE_PASSWORD, peer->GetHandle(), info);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    DWORD handle = info->dwHandle;
    SQLResult* result = message->Get();

    if (result->iNumRows > 0)
    {
        MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
        bool matches = row[0] && *row[0] ? !strcasecmp(row[0], info->login) : !strcmp("000000", info->login);
        if (matches)
        {
            char query[8193];
            const auto escaped = dbsql::EscapeField(info->safebox_password);
            if (!escaped)
            {
                peer->EncodeHeader(SafeboxChangePasswordAnswer, handle, 1);
                peer->EncodeBYTE(0);
                delete info;
                return;
            }
            dbsql::snprintf(query, 8192, "UPDATE safebox%s SET password='%s' WHERE account_id = %lu",
                    GetTablePostfix(), escaped->c_str(), info->account_id);
            CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_CHANGE_PASSWORD_SECOND, peer->GetHandle(), info);
            return;
        }
    }

    delete info;
    peer->EncodeHeader(SafeboxChangePasswordAnswer, handle, sizeof(BYTE));
    peer->EncodeBYTE(0);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* queryInfo = static_cast<CQueryInfo*>(message->pvUserData);
    ClientHandleInfo* info = static_cast<ClientHandleInfo*>(queryInfo->pvData);
    DWORD handle = info->dwHandle;
    delete info;

    peer->EncodeHeader(SafeboxChangePasswordAnswer, handle, sizeof(BYTE));
    peer->EncodeBYTE(1);
}

void CClientManager::QUERY_SAFEBOX_SAVE(CPeer* peer, TSafeboxTable* safebox)
{
    char query[8192];
    dbsql::snprintf(query, sizeof(query), "UPDATE safebox%s SET gold='%lu' WHERE account_id=%lu",
            GetTablePostfix(), static_cast<unsigned long>(safebox->dwGold), static_cast<unsigned long>(safebox->dwID));
    CDBManager::instance().ReturnQuery(query, QID_SAFEBOX_SAVE, peer->GetHandle(), NULL);
}


namespace
{
    typedef std::pair<DWORD, DWORD> TItemPricelistReqInfo;
    const BYTE MYSHOP_PRICELIST_RESPONSE = 157;
}

void CClientManager::RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* message)
{
    CQueryInfo* query = static_cast<CQueryInfo*>(message->pvUserData);
    TItemPricelistReqInfo* request = static_cast<TItemPricelistReqInfo*>(query->pvData);

    TItemPriceListTable table;
    table.dwOwnerID = request->second;
    table.byCount = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(message->Get()->pSQLResult)))
    {
        TItemPriceInfo& price = table.aPriceInfo[table.byCount];
        price.dwVnum = static_cast<DWORD>(atoi(row[0]));
        price.dwPrice = static_cast<DWORD>(atoi(row[1]));
        ++table.byCount;
    }

    PutItemPriceListCache(&table);

    TPacketMyshopPricelistHeader header;
    header.dwOwnerID = request->second;
    header.byCount = table.byCount;
    const DWORD priceSize = sizeof(TItemPriceInfo) * header.byCount;
    peer->EncodeHeader(MYSHOP_PRICELIST_RESPONSE, request->first, sizeof(header) + priceSize);
    peer->Encode(&header, sizeof(header));
    peer->Encode(table.aPriceInfo, priceSize);

    sys_log(0, "Load MyShopPricelist handle[%d] pid[%d] count[%d]",
            static_cast<int>(request->first), static_cast<int>(request->second), table.byCount);

    delete request;
}

void CClientManager::RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* message)
{
    CQueryInfo* query = static_cast<CQueryInfo*>(message->pvUserData);
    TItemPriceListTable* update = static_cast<TItemPriceListTable*>(query->pvData);

    TItemPriceListTable table;
    table.dwOwnerID = update->dwOwnerID;
    table.byCount = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(message->Get()->pSQLResult)))
    {
        TItemPriceInfo& price = table.aPriceInfo[table.byCount];
        price.dwVnum = static_cast<DWORD>(atoi(row[0]));
        price.dwPrice = static_cast<DWORD>(atoi(row[1]));
        ++table.byCount;
    }

    PutItemPriceListCache(&table);
    GetItemPriceListCache(update->dwOwnerID)->UpdateList(update);
    delete update;
}

CItemPriceListTableCache* CClientManager::GetItemPriceListCache(DWORD id)
{
    TItemPriceListCacheMap::iterator it = m_mapItemPriceListCache.find(id);
    return it == m_mapItemPriceListCache.end() ? NULL : it->second;
}

void CClientManager::PutItemPriceListCache(const TItemPriceListTable* table)
{
    CItemPriceListTableCache* cache = GetItemPriceListCache(table->dwOwnerID);
    if (!cache)
    {
        cache = new CItemPriceListTableCache;
        m_mapItemPriceListCache.insert(TItemPriceListCacheMap::value_type(table->dwOwnerID, cache));
    }
    cache->Put(const_cast<TItemPriceListTable*>(table), true);
}

void CClientManager::MyshopPricelistUpdate(const TPacketMyshopPricelistHeader* packet)
{
    const TItemPriceInfo* prices = reinterpret_cast<const TItemPriceInfo*>(packet + 1);
    CItemPriceListTableCache* cache = GetItemPriceListCache(packet->dwOwnerID);
    if (cache)
    {
        TItemPriceListTable table;
        table.dwOwnerID = packet->dwOwnerID;
        table.byCount = packet->byCount;
        thecore_memcpy(table.aPriceInfo, prices, sizeof(TItemPriceInfo) * table.byCount);
        cache->UpdateList(&table);
    }
    else
    {
        TItemPriceListTable* update = new TItemPriceListTable;
        update->dwOwnerID = packet->dwOwnerID;
        update->byCount = packet->byCount;
        thecore_memcpy(update->aPriceInfo, prices, sizeof(TItemPriceInfo) * packet->byCount);

        char query[8193];
        dbsql::snprintf(query, 8192, "SELECT item_vnum, price FROM myshop_pricelist%s WHERE owner_id=%lu",
                 GetTablePostfix(), static_cast<unsigned long>(packet->dwOwnerID));
        CDBManager::instance().ReturnQuery(query, QID_ITEMPRICE_LOAD_FOR_UPDATE, 0, update);
    }
}

void CClientManager::MyshopPricelistRequest(CPeer* peer, DWORD handle, DWORD playerID)
{
    CItemPriceListTableCache* cache = GetItemPriceListCache(playerID);
    if (cache)
    {
        sys_log(0, "Cache MyShopPricelist handle[%d] pid[%d]",
                static_cast<int>(handle), static_cast<int>(playerID));
        const TItemPriceListTable* table = cache->Get(false);
        TPacketMyshopPricelistHeader header;
        header.dwOwnerID = table->dwOwnerID;
        header.byCount = table->byCount;
        const DWORD priceSize = sizeof(TItemPriceInfo) * header.byCount;
        peer->EncodeHeader(MYSHOP_PRICELIST_RESPONSE, handle, sizeof(header) + priceSize);
        peer->Encode(&header, sizeof(header));
        peer->Encode(table->aPriceInfo, priceSize);
    }
    else
    {
        sys_log(0, "Query MyShopPricelist handle[%d] pid[%d]",
                static_cast<int>(handle), static_cast<int>(playerID));
        char query[8193];
        dbsql::snprintf(query, 8192, "SELECT item_vnum, price FROM myshop_pricelist%s WHERE owner_id=%lu", GetTablePostfix(), playerID);
        TItemPricelistReqInfo* request = new TItemPricelistReqInfo(handle, playerID);
        CDBManager::instance().ReturnQuery(query, QID_ITEMPRICE_LOAD, peer->GetHandle(), request);
    }
}


namespace
{
#pragma pack(push, 1)
struct SetupMapLocation
{
    BYTE maps[32];
    char host[16];
    WORD port;
};

struct SetupP2P
{
    char host[16];
    WORD port;
    BYTE channel;
};

#pragma pack(pop)

static_assert(sizeof(SetupMapLocation) == 50, "2009 map location record");
static_assert(sizeof(SetupP2P) == 19, "2009 P2P setup record");

template <size_t Size>
void CopySetupText(char (&dest)[Size], const char* source)
{
    strncpy(dest, source, Size - 1);
    dest[Size - 1] = '\0';
}

SetupMapLocation GetSetupMapLocation(CPeer* peer)
{
    SetupMapLocation location;
    CopySetupText(location.host, peer->GetPublicIP());
    location.port = peer->GetListenPort();
    thecore_memcpy(location.maps, peer->GetMaps(), sizeof(location.maps));
    return location;
}

template <typename Record>
void EncodeBootTable(CPeer* peer, const Record* records, size_t count)
{
    peer->EncodeWORD(sizeof(Record));
    peer->EncodeWORD(static_cast<WORD>(count));
    if (count)
        peer->Encode(records, sizeof(Record) * count);
}

template <typename Record>
void EncodeBootVector(CPeer* peer, const std::vector<Record>& records)
{
    EncodeBootTable(peer, records.empty() ? NULL : &records[0], records.size());
}
}

void CClientManager::QUERY_SETUP(CPeer* peer, DWORD, const char* payload)
{
    const TPacketGDSetup* setup = reinterpret_cast<const TPacketGDSetup*>(payload);
    const TPacketLoginOnSetup* loginPacket =
        reinterpret_cast<const TPacketLoginOnSetup*>(payload + sizeof(*setup));
    if (setup->bAuthServer)
    {
        if (m_pkAuthPeer || peer->GetChannel() || m_authPeerAddress != peer->GetHost())
        {
            sys_err("Rejected duplicate or invalid auth peer setup from %s", peer->GetHost());
            return;
        }

        m_pkAuthPeer = peer;
        SendAllLoginToBilling();
        return;
    }

    if (peer == m_pkAuthPeer || peer->GetChannel() || setup->bChannel == 0)
    {
        sys_err("Rejected repeated game peer setup from %s", peer->GetHost());
        return;
    }
    peer->SetPublicIP(setup->szPublicIP);
    peer->SetChannel(setup->bChannel);
    peer->SetListenPort(setup->wListenPort);
    peer->SetP2PPort(setup->wP2PPort);
    peer->SetMaps(const_cast<BYTE*>(setup->abMaps));

    const BYTE channel = peer->GetChannel();
    const BYTE warChannel = 99;
    const SetupMapLocation local = GetSetupMapLocation(peer);
    std::vector<SetupMapLocation> locations;
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        CPeer* other = *it;
        const BYTE otherChannel = other->GetChannel();
        if (other == peer || !otherChannel)
            continue;

        const bool listed = otherChannel == channel || otherChannel == warChannel
            || (channel == warChannel && otherChannel == 1);
        if (listed)
            locations.push_back(GetSetupMapLocation(other));

        if (channel == warChannel || otherChannel == channel || (channel == 1 && listed))
        {
            other->EncodeHeader(254, 0, sizeof(BYTE) + sizeof(local));
            other->EncodeBYTE(1);
            other->Encode(&local, sizeof(local));
        }
    }

    locations.push_back(local);
    const BYTE locationCount = static_cast<BYTE>(locations.size());
    peer->EncodeHeader(254, 0, sizeof(BYTE) + sizeof(local) * locations.size());
    peer->EncodeBYTE(locationCount);
    peer->Encode(&locations[0], sizeof(local) * locations.size());

    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        CPeer* other = *it;
        if (other == peer || !other->GetChannel())
            continue;

        const bool useNewPeerAddress = number_ex(0, 1, const_cast<char*>("ClientManager.cpp"), 0x49e) != 0;
        CPeer* addressPeer = useNewPeerAddress ? peer : other;
        CPeer* receivingPeer = useNewPeerAddress ? other : peer;
        SetupP2P p2p;
        CopySetupText(p2p.host, addressPeer->GetPublicIP());
        p2p.port = addressPeer->GetP2PPort();
        p2p.channel = addressPeer->GetChannel();
        receivingPeer->EncodeHeader(255, 0, sizeof(p2p));
        receivingPeer->Encode(&p2p, sizeof(p2p));
    }

    std::vector<BillingRepairRecord> repairs;
    for (DWORD i = 0; i < setup->dwLoginCount; ++i, ++loginPacket)
    {
        CLoginData* byKey = GetLoginData(loginPacket->dwLoginKey);
        CLoginData* byName = GetLoginDataByLogin(loginPacket->szLogin);
        CLoginData* byAccount = GetLoginDataByAID(loginPacket->dwID);
        if (!loginPacket->dwID || !loginPacket->dwLoginKey || !*loginPacket->szLogin ||
            byKey != byName || byKey != byAccount || FindLogonAccount(loginPacket->szLogin))
            continue;

        std::unique_ptr<CLoginData> newLogin;
        if (!byKey)
            newLogin = std::make_unique<CLoginData>();
        CLoginData* login = byKey ? byKey : newLogin.get();
        login->SetKey(loginPacket->dwLoginKey);
        const DWORD keys[4] = {
            loginPacket->adwClientKey[0], loginPacket->adwClientKey[1],
            loginPacket->adwClientKey[2], loginPacket->adwClientKey[3]
        };
        login->SetClientKey(keys);
        login->SetIP(loginPacket->szHost);
        TAccountTable& account = login->GetRef();
        account.id = loginPacket->dwID;
        trim_and_lower(account.login, loginPacket->szLogin, LOGIN_MAX_LEN);
        CopySetupText(account.social_id, loginPacket->szSocialID);
        CopySetupText(account.passwd, "TEMP");
        if (newLogin)
            InsertLoginData(newLogin.release());

        if (!InsertLogonAccount(loginPacket->szLogin, peer->GetHandle(), loginPacket->szHost))
        {
            sys_err("SETUP: login failed for account %lu", static_cast<DWORD>(loginPacket->dwID));
            continue;
        }

        login->SetPlay(true);
        if (m_pkAuthPeer)
        {
            BillingRepairRecord repair;
            repair.loginKey = login->GetKey();
            CopySetupText(repair.login, loginPacket->szLogin);
            CopySetupText(repair.host, loginPacket->szHost);
            repairs.push_back(repair);
        }
    }

    if (m_pkAuthPeer && !repairs.empty())
    {
        m_pkAuthPeer->EncodeHeader(128, 0, sizeof(std::uint32_t) + sizeof(BillingRepairRecord) * repairs.size());
        m_pkAuthPeer->EncodeDWORD(static_cast<DWORD>(repairs.size()));
        m_pkAuthPeer->Encode(&repairs[0], sizeof(BillingRepairRecord) * repairs.size());
    }

    SendPartyOnSetup(peer);
    CGuildManager::instance().OnSetup(peer);
    CPrivManager::instance().SendPrivOnSetup(peer);
    SendEventFlagsOnSetup(peer);
    marriage::CManager::instance().OnSetup(peer);
}

void CClientManager::QUERY_PLAYER_COUNT(CPeer* peer, TPlayerCountPacket* packet)
{
    peer->SetUserCount(packet->dwCount);
}

void CClientManager::QUERY_BOOT(CPeer* peer, TPacketGDBoot* packet)
{
    static_assert(sizeof(TMobTable) == 255, "2009 boot mob record");
    static_assert(sizeof(TItemTable) == 154, "2009 boot item record");
    static_assert(sizeof(TShopTable) == 449, "2009 boot shop record");
    static_assert(sizeof(TSkillTable) == 1475, "2009 boot skill record");
    static_assert(sizeof(TRefineTable) == 53, "2009 boot refine record");
    static_assert(sizeof(TItemAttrTable) == 69, "2009 boot attribute record");
    static_assert(sizeof(TBanwordTable) == 25, "2009 boot banword record");
    static_assert(sizeof(building::TLand) == 36, "2009 boot land record");
    static_assert(sizeof(building::TObjectProto) == 96, "2009 boot object proto record");
    static_assert(sizeof(building::TObject) == 40, "2009 boot object record");
    static_assert(sizeof(tAdminInfo) == 104, "2009 boot admin record");

    std::vector<tAdminInfo> admins;
    std::vector<std::string> hosts;
    __GetHostInfo(&hosts);
    __GetAdminInfo(packet->szIP, &admins);

    // Version 5 carries two legacy Monarch sections even with the feature disabled.
    const BYTE neutralLegacyInfo[304] = {};
    const WORD legacyCandidateSize = 68;
    const BYTE packetVersion = 5;
    const std::uint32_t packetSize =
        0x17b
        + sizeof(TMobTable) * m_vec_mobTable.size()
        + sizeof(TItemTable) * m_vec_itemTable.size()
        + sizeof(TShopTable) * m_iShopTableSize
        + sizeof(TSkillTable) * m_vec_skillTable.size()
        + sizeof(TRefineTable) * m_iRefineTableSize
        + sizeof(TItemAttrTable) * (m_vec_itemAttrTable.size() + m_vec_itemRareTable.size())
        + sizeof(TBanwordTable) * m_vec_banwordTable.size()
        + sizeof(building::TLand) * m_vec_kLandTable.size()
        + sizeof(building::TObjectProto) * m_vec_kObjectProto.size()
        + sizeof(building::TObject) * m_map_pkObjectTable.size()
        + 16 * hosts.size()
        + sizeof(tAdminInfo) * admins.size();

    sys_log(0, "BOOT: PACKET: %d", static_cast<int>(packetSize));
    sys_log(0, "BOOT: VERSION: %d", packetVersion);
    sys_log(0, "sizeof(TMobTable) = %d", static_cast<int>(sizeof(TMobTable)));
    sys_log(0, "sizeof(TItemTable) = %d", static_cast<int>(sizeof(TItemTable)));
    sys_log(0, "sizeof(TItemAttrTable) = %d", static_cast<int>(sizeof(TItemAttrTable)));
    sys_log(0, "sizeof(TShopTable) = %d", static_cast<int>(sizeof(TShopTable)));
    sys_log(0, "sizeof(TSkillTable) = %d", static_cast<int>(sizeof(TSkillTable)));
    sys_log(0, "sizeof(TRefineTable) = %d", static_cast<int>(sizeof(TRefineTable)));
    sys_log(0, "sizeof(TItemRareTable) = %d", static_cast<int>(sizeof(TItemAttrTable)));
    sys_log(0, "sizeof(TBanwordTable) = %d", static_cast<int>(sizeof(TBanwordTable)));
    sys_log(0, "sizeof(TLand) = %d", static_cast<int>(sizeof(building::TLand)));
    sys_log(0, "sizeof(TObjectProto) = %d", static_cast<int>(sizeof(building::TObjectProto)));
    sys_log(0, "sizeof(TObject) = %d", static_cast<int>(sizeof(building::TObject)));

    peer->EncodeHeader(43, 0, packetSize);
    peer->Encode(&packetSize, sizeof(packetSize));
    peer->Encode(&packetVersion, sizeof(packetVersion));

    EncodeBootVector(peer, m_vec_mobTable);
    EncodeBootVector(peer, m_vec_itemTable);
    EncodeBootTable(peer, m_pShopTable, m_iShopTableSize);
    EncodeBootVector(peer, m_vec_skillTable);
    EncodeBootTable(peer, m_pRefineTable, m_iRefineTableSize);
    EncodeBootVector(peer, m_vec_itemAttrTable);
    EncodeBootVector(peer, m_vec_itemRareTable);
    EncodeBootVector(peer, m_vec_banwordTable);
    EncodeBootVector(peer, m_vec_kLandTable);
    EncodeBootVector(peer, m_vec_kObjectProto);
    peer->EncodeWORD(sizeof(building::TObject));
    peer->EncodeWORD(static_cast<WORD>(m_map_pkObjectTable.size()));
    for (std::map<DWORD, building::TObject*>::iterator it = m_map_pkObjectTable.begin();
         it != m_map_pkObjectTable.end(); ++it)
        peer->Encode(it->second, sizeof(building::TObject));

    const std::int32_t now = static_cast<std::int32_t>(time(0));
    peer->Encode(&now, sizeof(now));
    const DWORD rangeStart = packet->dwItemIDRange[0];
    const DWORD rangeEnd = packet->dwItemIDRange[1];
    const std::uint32_t maxItemID = __GetMaxItemID(peer, rangeStart, rangeEnd);
    peer->Encode(&maxItemID, sizeof(maxItemID));
    if (maxItemID)
        peer->SetItemRange(rangeStart, rangeEnd);

    peer->EncodeWORD(16);
    peer->EncodeWORD(static_cast<WORD>(hosts.size()));
    for (size_t i = 0; i < hosts.size(); ++i)
    {
        char host[16];
        CopySetupText(host, hosts[i].c_str());
        peer->Encode(host, sizeof(host));
    }

    peer->EncodeWORD(sizeof(tAdminInfo));
    peer->EncodeWORD(static_cast<WORD>(admins.size()));
    for (size_t i = 0; i < admins.size(); ++i)
        peer->Encode(&admins[i], sizeof(admins[i]));

    peer->EncodeWORD(sizeof(neutralLegacyInfo));
    peer->EncodeWORD(1);
    peer->Encode(neutralLegacyInfo, sizeof(neutralLegacyInfo));
    peer->EncodeWORD(legacyCandidateSize);
    peer->EncodeWORD(0);
    peer->EncodeWORD(0xffff);
}

bool CClientManager::__IsItemRangeCollision(CPeer* current, DWORD start, DWORD end)
{
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        if (*it != current && (*it)->IsItemRangeCollision(start, end))
            return true;
    }

    current->IsItemRangeCollision(m_dwItemIDStart, m_dwItemIDStop);
    return false;
}

DWORD CClientManager::__GetMaxItemID(CPeer* current, DWORD start, DWORD end)
{
    if ((!start && !end) || __IsItemRangeCollision(current, start, end))
        return 0;

    char query[256];
    dbsql::sprintf(query, "SELECT MAX(id) FROM item%s WHERE id >= %lu AND id <= %lu",
            GetTablePostfix(), start, end);
    CDBManager& db = CDBManager::instance();
    std::unique_ptr<SQLMsg> result(db.DirectQuery(query, SQL_PLAYER));
    if (!result->Get()->iNumRows)
    {
        sys_err("DirectQuery failed");
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result->Get()->pSQLResult);
    const DWORD maximum = row[0] ? static_cast<std::uint32_t>(atoi(row[0])) : start;
    if (maximum == end)
    {
        sys_err("Range not enough between %lu ~ %lu\n", start, end);
        return 0;
    }

    DWORD next = maximum + 1;
    result.reset();
    for (TItemCacheMap::iterator it = m_map_itemCache.begin(); it != m_map_itemCache.end(); ++it)
    {
        const DWORD id = it->second->Get(false)->id;
        if (start < id && id < end && next <= id)
        {
            next = id + 1;
            if (end <= next)
                return 0;
        }
    }

    dbsql::sprintf(query, "SELECT COUNT(*) FROM item%s WHERE id >= %lu AND id <= %lu",
            GetTablePostfix(), next, end);
    result.reset(db.DirectQuery(query, SQL_PLAYER));
    if (!result->Get()->pSQLResult)
        return 0;

    row = mysql_fetch_row(result->Get()->pSQLResult);
    const int duplicates = atoi(row[0]);
    if (duplicates)
    {
        sys_err("ITEM ID exist in range %lu ~ %lu (duplicates: %d)\n", next, end, duplicates);
        return 0;
    }

    result.reset();
    sys_log(0, "ITEM_ID_RANGE: [UsingMax ~ End] %lu ~%lu (Remained count: %d)\n",
            next, end, static_cast<int>(static_cast<std::uint32_t>(end - next)));
    return next;
}

void CClientManager::SendPartyOnSetup(CPeer* peer)
{
    TPartyMap& parties = m_map_pkChannelParty[peer->GetChannel()];
    for (TPartyMap::iterator party = parties.begin(); party != parties.end(); ++party)
    {
        const auto leaderId = static_cast<std::uint32_t>(party->first);
        const TPacketPartyCreate create = {leaderId};
        peer->EncodeHeader(59, 0, sizeof(create));
        peer->Encode(&create, sizeof(create));

        for (TPartyMember::iterator member = party->second.begin(); member != party->second.end(); ++member)
        {
            const auto memberId = static_cast<std::uint32_t>(member->first);
            const TPacketPartyAdd add = {leaderId, memberId, member->second.bRole};
            peer->EncodeHeader(61, 0, sizeof(add));
            peer->Encode(&add, sizeof(add));

            const TPacketPartySetMemberLevel level = {leaderId, memberId, member->second.bLevel};
            peer->EncodeHeader(65, 0, sizeof(level));
            peer->Encode(&level, sizeof(level));
        }
    }
}


DWORD CClientManager::GetItemID()
{
    return m_dwNowItemID;
}


namespace
{
enum
{
    ReloadProtoPacket = 120,
    CreateObjectPacket = 140,
    DeleteObjectPacket = 141,
    UpdateLandPacket = 142,
    ReloadAdminPacket = 158,
    AdminHostBytes = 16
};
}

void CClientManager::CreateObject(TPacketGDCreateObject* packet)
{
    char query[513];
    dbsql::snprintf(query, 512,
            "INSERT INTO object%s (land_id, vnum, map_index, x, y, x_rot, y_rot, z_rot) "
            "VALUES(%lu, %lu, %ld, %ld, %ld, %f, %f, %f)",
            GetTablePostfix(), static_cast<unsigned long>(packet->dwLandID), static_cast<unsigned long>(packet->dwVnum),
            static_cast<long>(packet->lMapIndex), static_cast<long>(packet->x), static_cast<long>(packet->y),
            packet->xRot, packet->yRot, packet->zRot);

    std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query));
    SQLResult* result = message->Get();
    if (result->iInsertID < 1)
    {
        sys_err("cannot insert object");
        return;
    }

    building::TObject* object = new building::TObject;
    object->dwID = result->iInsertID;
    object->dwVnum = packet->dwVnum;
    object->dwLandID = packet->dwLandID;
    object->lMapIndex = packet->lMapIndex;
    object->x = packet->x;
    object->y = packet->y;
    object->xRot = packet->xRot;
    object->yRot = packet->yRot;
    object->zRot = packet->zRot;
    object->lLife = 0;

    ForwardPacket(CreateObjectPacket, object, sizeof(*object));
    m_map_pkObjectTable.insert(std::make_pair(object->dwID, object));
}

void CClientManager::DeleteObject(DWORD id)
{
    char query[128];
    dbsql::snprintf(query, sizeof(query), "DELETE FROM object%s WHERE id=%lu", GetTablePostfix(), id);

    std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query));
    if (message->Get()->iAffectedRows < 1)
    {
        sys_err("no object by id %lu", id);
        return;
    }

    std::map<DWORD, building::TObject*>::iterator it = m_map_pkObjectTable.find(id);
    if (it != m_map_pkObjectTable.end())
    {
        delete it->second;
        m_map_pkObjectTable.erase(it);
    }

    const auto wireId = static_cast<std::uint32_t>(id);
    ForwardPacket(DeleteObjectPacket, &wireId, sizeof(wireId));
}

void CClientManager::UpdateLand(std::uint32_t const* fields)
{
    const DWORD landID = fields[0];
    const DWORD guildID = fields[1];

    for (std::vector<building::TLand>::iterator it = m_vec_kLandTable.begin(); it != m_vec_kLandTable.end(); ++it)
    {
        if (it->dwID != landID)
            continue;

        char query[257];
        dbsql::snprintf(query, 256, "UPDATE land%s SET guild_id=%lu WHERE id=%lu",
                GetTablePostfix(), guildID, landID);
        CDBManager::instance().AsyncQuery(query);
        it->dwGuildID = guildID;
        ForwardPacket(UpdateLandPacket, &*it, sizeof(*it));
        return;
    }
}

bool CClientManager::__GetAdminInfo(const char* ip, std::vector<tAdminInfo>* admins)
{
    if (ip && strcmp(ip, "ALL") != 0 && !IsIPv4Address(ip))
        return false;

    char query[512];
    dbsql::sprintf(query,
            "SELECT mID,mAccount,mName,mContactIP,mServerIP,mAuthority FROM gmlist "
            "WHERE mServerIP='ALL' or mServerIP='%s'",
            ip ? ip : "ALL");

    std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query, SQL_COMMON));
    SQLResult* result = message->Get();
    if (!result->iNumRows)
    {
        sys_err("__GetAdminInfo() ==> DirectQuery failed( %s )", query);
        return false;
    }

    admins->resize(result->iNumRows);
    MYSQL_ROW row;
    for (size_t i = 0; (row = mysql_fetch_row(result->pSQLResult)); ++i)
    {
        tAdminInfo admin;
        admin.m_ID = atoi(row[0]);
        strncpy(admin.m_szAccount, row[1], sizeof(admin.m_szAccount));
        strncpy(admin.m_szName, row[2], sizeof(admin.m_szName));
        strncpy(admin.m_szContactIP, row[3], sizeof(admin.m_szContactIP));
        strncpy(admin.m_szServerIP, row[4], sizeof(admin.m_szServerIP));

        const std::string authority = row[5];
        if (authority == "IMPLEMENTOR")
            admin.m_Authority = 5;
        else if (authority == "GOD")
            admin.m_Authority = 4;
        else if (authority == "HIGH_WIZARD")
            admin.m_Authority = 3;
        else if (authority == "LOW_WIZARD")
            admin.m_Authority = 1;
        else if (authority == "WIZARD")
            admin.m_Authority = 2;
        else
            continue;

        (*admins)[i] = admin;
    }

    return true;
}

bool CClientManager::__GetHostInfo(std::vector<std::string>* hosts)
{
    const char* query = "SELECT mIP FROM gmhost";
    std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query, SQL_COMMON));
    SQLResult* result = message->Get();
    if (!result->iNumRows)
    {
        sys_err("__GetHostInfo() ==> DirectQuery failed( %s )", query);
        return false;
    }

    hosts->resize(result->iNumRows);
    MYSQL_ROW row;
    for (size_t i = 0; (row = mysql_fetch_row(result->pSQLResult)); ++i)
    {
        (*hosts)[i] = row[0];
    }

    return true;
}

void CClientManager::ReloadAdmin(CPeer*, TPacketReloadAdmin* packet)
{
    std::vector<std::string> hosts;
    std::vector<tAdminInfo> requestedAdmins;
    __GetHostInfo(&hosts);
    if (packet)
        __GetAdminInfo(packet->szIP, &requestedAdmins);

    const auto maxCount = (std::numeric_limits<WORD>::max)();
    if (hosts.size() > maxCount || requestedAdmins.size() > maxCount)
    {
        sys_err("Admin reload exceeds the packet count limit");
        return;
    }

    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        CPeer* peer = *it;
        if (!peer->GetChannel())
            continue;

        std::vector<tAdminInfo> peerAdmins;
        if (!packet)
        {
            const char* ip = peer->GetPublicIP();
            if (!IsIPv4Address(ip))
                continue;
            __GetAdminInfo(ip, &peerAdmins);
            if (peerAdmins.size() > maxCount)
            {
                sys_err("Admin reload exceeds the packet count limit for %s", ip);
                continue;
            }
        }

        const auto& admins = packet ? requestedAdmins : peerAdmins;
        const DWORD bytes = 4 * sizeof(WORD) + AdminHostBytes * hosts.size() + sizeof(tAdminInfo) * admins.size();
        peer->EncodeHeader(ReloadAdminPacket, 0, bytes);
        peer->EncodeWORD(AdminHostBytes);
        peer->EncodeWORD(static_cast<WORD>(hosts.size()));
        for (std::vector<std::string>::const_iterator host = hosts.begin(); host != hosts.end(); ++host)
        {
            char ip[AdminHostBytes];
            strncpy(ip, host->c_str(), sizeof(ip));
            peer->Encode(ip, sizeof(ip));
        }

        peer->EncodeWORD(sizeof(tAdminInfo));
        peer->EncodeWORD(static_cast<WORD>(admins.size()));
        for (std::vector<tAdminInfo>::const_iterator admin = admins.begin(); admin != admins.end(); ++admin)
            peer->Encode(&*admin, sizeof(*admin));
    }
}

void CClientManager::QUERY_RELOAD_PROTO()
{
    if (!InitializeTables())
    {
        sys_err("QUERY_RELOAD_PROTO: cannot load tables");
        return;
    }

    constexpr auto maxCount = (std::numeric_limits<WORD>::max)();
    if (m_vec_skillTable.size() > maxCount || m_vec_banwordTable.size() > maxCount ||
        m_vec_itemTable.size() > maxCount || m_vec_mobTable.size() > maxCount)
    {
        sys_err("Proto reload exceeds the packet count limit");
        return;
    }

    DWORD skillBytes = sizeof(TSkillTable) * m_vec_skillTable.size();
    DWORD banwordBytes = sizeof(TBanwordTable) * m_vec_banwordTable.size();
    DWORD itemBytes = sizeof(TItemTable) * m_vec_itemTable.size();
    DWORD mobBytes = sizeof(TMobTable) * m_vec_mobTable.size();
    for (TPeerList::iterator it = m_peerList.begin(); it != m_peerList.end(); ++it)
    {
        CPeer* peer = *it;
        if (!peer->GetChannel())
            continue;

        peer->EncodeHeader(ReloadProtoPacket, 0, 4 * sizeof(WORD) + skillBytes + banwordBytes + itemBytes + mobBytes);
        peer->EncodeWORD(static_cast<WORD>(m_vec_skillTable.size()));
        peer->Encode(m_vec_skillTable.data(), skillBytes);
        peer->EncodeWORD(static_cast<WORD>(m_vec_banwordTable.size()));
        peer->Encode(m_vec_banwordTable.data(), banwordBytes);
        peer->EncodeWORD(static_cast<WORD>(m_vec_itemTable.size()));
        peer->Encode(m_vec_itemTable.data(), itemBytes);
        peer->EncodeWORD(static_cast<WORD>(m_vec_mobTable.size()));
        peer->Encode(m_vec_mobTable.data(), mobBytes);
    }
}
