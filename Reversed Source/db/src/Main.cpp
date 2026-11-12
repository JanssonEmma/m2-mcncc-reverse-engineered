#include "stdafx.h"
#include "Main.h"
#include "Config.h"
#include "Peer.h"
#include "DBManager.h"
#include "ClientManager.h"
#include "GuildManager.h"
#include "ItemAwardManager.h"
#include "HB.h"
#include "PrivManager.h"
#include "MoneyLog.h"
#include "Marriage.h"
#include "BlockCountry.h"
#include "SqlInput.h"
#include <charconv>
#include <signal.h>
#include <sstream>

extern "C" int log_keep_days;
void SetPlayerDBName(const char* c_pszPlayerDBName);
void SetTablePostfix(const char* c_pszTablePostfix);

std::string g_stTablePostfix;
std::string g_stLocaleNameColumn = "name";
std::string g_stLocale = "euckr";
std::string g_stPlayerDBName = "";

bool g_bHotBackup = false;
BOOL g_test_server = false;

int g_iPlayerCacheFlushSeconds = 60*7;
int g_iItemCacheFlushSeconds = 60*5;

int g_iLogoutSeconds = 60*10;

int g_log = 1;

int g_iItemPriceListTableCacheFlushSeconds = 540;

void emergency_sig(int sig)
{
	if (sig == SIGSEGV)
		sys_log(0, "DUMPING QURIES (SIGNAL: SIGSEGV)");
	else
		sys_log(0, "DUMPING QURIES (SIGNAL: SIGUSR1)");

	CDBManager::instance().FlushQueryQueue(SQL_PLAYER, "dump.txt");

	if (sig == SIGSEGV)
		abort();
}

int main()
{
	puts("METIN2_DB_CACHE: GERMANY");

	CConfig Config;
	CNetPoller poller;
	CDBManager DBManager;
	CClientManager ClientManager;
	PlayerHB player_hb;
	CGuildManager GuildManager;
	CPrivManager PrivManager;
	CMoneyLog MoneyLog;
	ItemAwardManager ItemAwardManager;
	marriage::CManager MarriageManager;
	CBlockCountry	BlockCountry;

	if (!Start())
		return 1;

	GuildManager.Initialize();
	MarriageManager.Initialize();
	BlockCountry.Load();

	sys_log(0, "Metin2DBCacheServer Start\n");

	CClientManager::instance().MainLoop();

	signal_timer_disable();

	DBManager.Quit();
	int iCount;

	while (1)
	{
		iCount = 0;

		iCount += CDBManager::instance().CountReturnQuery(SQL_PLAYER);
		iCount += CDBManager::instance().CountAsyncQuery(SQL_PLAYER);

		if (iCount == 0)
			break;

		usleep(1000);
		sys_log(0, "WAITING_QUERY_COUNT %d", iCount);
	}

	return 1;
}

void emptybeat(LPHEART, int)
{
}

namespace
{
bool ConnectConfiguredSlot(eSQL_SLOT slot, const char* key)
{
    char line[257];
    if (!CConfig::instance().GetValue(key, line, 256))
    {
        sys_err("%s not configured", key);
        return false;
    }

    std::istringstream fields(line);
    std::string host, database, user, password, portText, extra;
    if (!(fields >> host >> database >> user >> password))
    {
        sys_err("%s requires host, database, user, and password", key);
        return false;
    }

    int port = 0;
    if (fields >> portText)
    {
        const auto* end = portText.data() + portText.size();
        const auto result = std::from_chars(portText.data(), end, port);
        if (result.ec != std::errc{} || result.ptr != end || port < 0 || port > 65535 ||
            (fields >> extra))
        {
            sys_err("%s has an invalid MySQL port or extra fields", key);
            return false;
        }
    }

    sys_log(0, "connecting to MySQL server (%s)", key);
    for (int attempt = 0; attempt < 6; ++attempt)
    {
        if (CDBManager::instance().Connect(slot, host.c_str(), port, database.c_str(),
                                           user.c_str(), password.c_str()))
        {
            if (slot == SQL_PLAYER)
                SetPlayerDBName(database.c_str());

            sys_log(0, "%s connected", key);
            return true;
        }

        if (attempt < 5)
        {
            sys_log(0, "%s connection failed, retrying in 5 seconds", key);
            sleep(5);
        }
    }

    sys_err("%s connection failed after six attempts", key);
    return false;
}
}

int Start()
{
	if (!CConfig::instance().LoadFile("conf.txt"))
	{
		fprintf(stderr, "Loading conf.txt failed.\n");
		return false;
	}

	if (!CConfig::instance().GetValue("TEST_SERVER", &g_test_server))
	{
		fprintf(stderr, "Real Server\n");
	}
	else
		fprintf(stderr, "Test Server\n");

	if (!CConfig::instance().GetValue("LOG", &g_log))
	{
		fprintf(stderr, "Log Off");
		g_log = 0;
	}
	else
	{
		g_log = 1;
		fprintf(stderr, "Log On");
	}

	int tmpValue;

	int heart_beat = 50;
	if (!CConfig::instance().GetValue("CLIENT_HEART_FPS", &heart_beat) || heart_beat <= 0 || heart_beat > 1000)
	{
		fprintf(stderr, "Cannot find CLIENT_HEART_FPS configuration.\n");
		return false;
	}

	log_keep_days = 3;

	if (CConfig::instance().GetValue("LOG_KEEP_DAYS", &tmpValue))
	{
		tmpValue = MINMAX(3, tmpValue, 30);
		log_keep_days = tmpValue;
		fprintf(stderr, "Setting log keeping days to %d\n", tmpValue);
	}

	thecore_init(heart_beat, emptybeat);
	signal_timer_enable(60);

	char szBuf[256+1];

	if (CConfig::instance().GetValue("LOCALE", szBuf, 256))
	{
		g_stLocale = szBuf;

		if ("gb2312" == g_stLocale)
		{
			sys_log(0, "CIBN_LOCALE: DISABLE_HOTBACKUP");
			g_bHotBackup = false;
		}
	}

	int iDisableHotBackup;
	if (CConfig::instance().GetValue("DISABLE_HOTBACKUP", &iDisableHotBackup))
	{
		if (iDisableHotBackup)
		{
			sys_log(0, "CONFIG: DISABLE_HOTBACKUP");
			g_bHotBackup = false;
		}
	}

	if (!CConfig::instance().GetValue("TABLE_POSTFIX", szBuf, 256))
	{
		sys_err("TABLE_POSTFIX not configured use default");
		szBuf[0] = '\0';
	}

    const std::string_view postfix(szBuf);
    if (postfix.size() > 32 || postfix.find_first_not_of(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_") != std::string_view::npos)
    {
        sys_err("Invalid TABLE_POSTFIX");
        return false;
    }
    SetTablePostfix(szBuf);

	if (CConfig::instance().GetValue("PLAYER_CACHE_FLUSH_SECONDS", &tmpValue) && tmpValue > 0)
	{
		g_iPlayerCacheFlushSeconds = tmpValue;
		sys_log(0, "PLAYER_CACHE_FLUSH_SECONDS: %d", g_iPlayerCacheFlushSeconds);
	}

	if (CConfig::instance().GetValue("ITEM_CACHE_FLUSH_SECONDS", &tmpValue) && tmpValue > 0)
	{
		g_iItemCacheFlushSeconds = tmpValue;
		sys_log(0, "ITEM_CACHE_FLUSH_SECONDS: %d", g_iItemCacheFlushSeconds);
	}

	if (CConfig::instance().GetValue("ITEM_PRICELIST_CACHE_FLUSH_SECONDS", &tmpValue) && tmpValue > 0)
	{
		g_iItemPriceListTableCacheFlushSeconds = tmpValue;
		sys_log(0, "ITEM_PRICELIST_CACHE_FLUSH_SECONDS: %d", g_iItemPriceListTableCacheFlushSeconds);
	}
	if (CConfig::instance().GetValue("CACHE_FLUSH_LIMIT_PER_SECOND", &tmpValue) && tmpValue > 0)
	{
		CClientManager::instance().SetCacheFlushCountLimit(tmpValue);
	}

	int iIDStart;
	if (!CConfig::instance().GetValue("PLAYER_ID_START", &iIDStart))
	{
		sys_err("PLAYER_ID_START not configured");
		return false;
	}

	CClientManager::instance().SetPlayerIDStart(iIDStart);

	if (CConfig::instance().GetValue("NAME_COLUMN", szBuf, 256))
	{
        if (!dbsql::IsIdentifier(szBuf))
        {
            sys_err("Invalid NAME_COLUMN");
            return false;
        }
		g_stLocaleNameColumn = szBuf;
	}

	if (!ConnectConfiguredSlot(SQL_PLAYER, "SQL_PLAYER") ||
        !ConnectConfiguredSlot(SQL_ACCOUNT, "SQL_ACCOUNT") ||
        !ConnectConfiguredSlot(SQL_COMMON, "SQL_COMMON") ||
        !ConnectConfiguredSlot(SQL_HOTBACKUP, "SQL_HOTBACKUP"))
        return false;

	if (!CNetPoller::instance().Create())
	{
		sys_err("Cannot create network poller");
		return false;
	}

	sys_log(0, "ClientManager initialization.. ");

	if (!CClientManager::instance().Initialize())
	{
		sys_log(0, "   failed");
		return false;
	}

	sys_log(0, "   OK");

	if (!PlayerHB::instance().Initialize())
	{
		sys_err("cannot initialize player hotbackup");
		return false;
	}

#ifndef _WIN32
	signal(SIGUSR1, emergency_sig);
#endif
	signal(SIGSEGV, emergency_sig);
	return true;
}

void SetTablePostfix(const char* c_pszTablePostfix)
{
	if (!c_pszTablePostfix || !*c_pszTablePostfix)
		g_stTablePostfix = "";
	else
		g_stTablePostfix = c_pszTablePostfix;
}

const char * GetTablePostfix()
{
	return g_stTablePostfix.c_str();
}

void SetPlayerDBName(const char* c_pszPlayerDBName)
{
	if (! c_pszPlayerDBName || ! *c_pszPlayerDBName)
		g_stPlayerDBName = "";
	else
	{
        g_stPlayerDBName = "`";
        for (const char c : std::string_view(c_pszPlayerDBName))
        {
            if (c == '`')
                g_stPlayerDBName += '`';
            g_stPlayerDBName += c;
        }
        g_stPlayerDBName += "`.";
	}
}

const char * GetPlayerDBName()
{
	return g_stPlayerDBName.c_str();
}
