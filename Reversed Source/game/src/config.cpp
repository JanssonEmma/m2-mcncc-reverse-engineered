#include "desc_manager.h"
#include "desc.h"
#include "char.h"
#include "locale.hpp"
#include "locale_service.h"
#include "cmd.h"
#include "constants.h"
#include "db.h"
#include "ip_ban.h"
#include "log.h"
#include "matrix_for_runup.h"
#include "war_map.h"
#include "utils.h"
#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>
#include <strings.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdint>
// Partial original configuration storage; parser and remaining globals are unresolved.
#include "config.h"
#include <cstdio>
#include <cstdlib>
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
bool g_bTrafficProfileOn=false;
bool g_bAuthServer=false;
bool g_bNoMoreClient = false;
bool g_bEmpireWhisper = true;
bool g_bPKServer;
bool no_wander;
std::string g_stAdminPageIP[3];
extern "C" int log_keep_days;
int g_iUserLimit = 32768;
// Original zero-initialized billing and Passpod switches.
bool g_bBilling=false;
std::string g_stBlockDate = "30000705";
bool g_bNoPasspod=false;
bool guild_mark_server = true;
int test_server=0;
BYTE g_bChannel;
WORD mother_port = 50080;
WORD p2p_port = 50900;
WORD db_port;
char db_addr[16];
char g_szPublicIP[16];
char g_szInternalIP[16];
char teen_addr[16];
WORD teen_port;
std::string g_stAuthMasterIP;
WORD g_wAuthMasterPort;

// ELF initialized int at0823f21c.
int passes_per_sec=25;
int save_event_second_cycle = passes_per_sec * 120;
int ping_event_second_cycle = passes_per_sec * 60;
int SPEEDHACK_LIMIT_COUNT = 50;
int SPEEDHACK_LIMIT_BONUS = 80;
bool g_bSkillDisable = false;
bool g_bNoRegen = false;
bool distribution_test_server = false;
bool china_event_server = false;

// 080dac10 and config static initialization: initially empty table suffix.
std::string g_table_postfix="";
std::string g_stHostname="";
const char* get_table_postfix() { return g_table_postfix.c_str(); }

// Original8-byte BSSsymbol083231a4; DWORD[2] spelling inferred from ABI/uses.
DWORD g_dwItemIDRange[2]={0,0};

// Original config.cpp globals; exact dynamic defaults proven at080db859..080db8e1.
std::string g_stQuestDir="./quest";
std::string g_stDefaultQuestObjectDir="./quest/object";
std::set<std::string> g_setQuestObjectDir;

// Target's private config.cpp map registry, original storage symbol at 0832325c.
static std::set<int> s_set_map_allows;

bool is_string_true(const char* value)
{
    const char initial = value[0];
    if (initial >= '0' && initial <= '9')
        return std::atoi(value) > 0;
    return initial == 'T' || initial == 't';
}

bool map_allow_find(int index)
{
    if (g_bAuthServer)
        return false;

    return s_set_map_allows.find(index) != s_set_map_allows.end();
}

void map_allow_log()
{
    for (std::set<int>::const_iterator it = s_set_map_allows.begin();
         it != s_set_map_allows.end(); ++it)
        sys_log(0, "MAP_ALLOW: %d", *it);
}

void map_allow_add(int index)
{
    if (map_allow_find(index))
    {
        std::fprintf(stdout, "MAP Setting is repeted.\n");
        std::fprintf(stdout, "MAP Setting is repeted.\n");
        std::fprintf(stdout, "MAP Setting is repeted.\n");
        std::fprintf(stdout, "Program Exit\n");
        std::exit(1);
    }

    std::fprintf(stdout, "MAP ALLOW %d\n", index);
    s_set_map_allows.insert(index);
}

void map_allow_copy(BYTE* output, int size)
{
    int count = 0;
    std::set<int>::const_iterator it = s_set_map_allows.begin();
    while (it != s_set_map_allows.end())
    {
        const std::set<int>::const_iterator current = it;
        ++count;
        ++it;
        *output++ = static_cast<BYTE>(*current);
        if (count > size)
            return;
    }
}

bool g_bCheckClientVersion;
std::string g_stClientVersion;
std::string g_stSMSPrefix = "\270\336\306\2762/";

static std::set<DWORD> s_set_dwProcessCRC;
static std::set<DWORD> s_set_dwFileCRC;

void LoadValidCRCList()
{
    s_set_dwProcessCRC.clear();
    s_set_dwFileCRC.clear();
    std::FILE* file=std::fopen("CRC","r");
    if (!file) return;
    char line[256];
    DWORD process, fileCRC;
    while (std::fgets(line,sizeof(line),file)) {
        if (!line[0]) continue;
        std::sscanf(line," %lu %lu ",&process,&fileCRC);
        s_set_dwProcessCRC.insert(process);
        s_set_dwFileCRC.insert(fileCRC);
        std::fprintf(stderr,"CLIENT_CRC: %lu %lu\n",process,fileCRC);
    }
    std::fclose(file);
}

bool IsValidFileCRC(DWORD crc)
{
    return s_set_dwFileCRC.find(crc)!=s_set_dwFileCRC.end();
}

bool IsValidProcessCRC(DWORD crc)
{
    return s_set_dwProcessCRC.find(crc)!=s_set_dwProcessCRC.end();
}

bool LoadClientVersion()
{
    std::FILE* file=std::fopen("VERSION","r");
    if (!file) return false;
    char version[256];
    std::fgets(version,sizeof(version),file);
    char* newline=std::strchr(version,'\n');
    if (newline) *newline=0;
    std::fprintf(stderr,"VERSION: \"%s\"\n",version);
    g_stClientVersion.assign(version);
    std::fclose(file);
    return true;
}

void CheckClientVersion()
{
    g_bCheckClientVersion=LoadClientVersion();
    if (!g_bCheckClientVersion) return;
    const DESC_MANAGER::DESC_SET& clients=DESC_MANAGER::instance().GetClientSet();
    DESC_MANAGER::DESC_SET::const_iterator it=clients.begin();
    while (it!=clients.end()) {
        DESC* desc=*it++;
        if (desc->GetCharacter() && g_stClientVersion.compare(desc->GetClientVersion())!=0) {
            desc->GetCharacter()->ChatPacket(2,locale_find("\305\254\266\363\300\314\276\360\306\256\040\271\366\300\374\300\314\040\306\262\267\301\040\267\316\261\327\276\306\277\364\040\265\313\264\317\264\331\056\040\301\244\273\363\300\373\300\270\267\316\040\306\320\304\241\040\310\304\040\301\242\274\323\307\317\274\274\277\344\056",g_iUseLocale));
            desc->DelayedDisconnect(10);
        }
    }
}

int g_iFullUserCount = 1200;

int g_iBusyUserCount = 650;

// 080d81f0
bool GetIPInfo()
{
    ifaddrs* interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0)
        return false;

    for (const ifaddrs* interface = interfaces; interface; interface = interface->ifa_next) {
        if (!interface->ifa_netmask || !interface->ifa_addr || interface->ifa_addr->sa_family != AF_INET)
            continue;
        const auto* address = reinterpret_cast<const sockaddr_in*>(interface->ifa_addr);
        const std::uint32_t ip = ntohl(address->sin_addr.s_addr);
        if (!ip || ip == INADDR_LOOPBACK)
            continue;
        char text[INET_ADDRSTRLEN];
        if (!inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text)))
            continue;
        const bool internal = std::strncmp(text, "202", 3) == 0;
        char* target = internal ? g_szInternalIP : g_szPublicIP;
        std::strncpy(target, text, 15);
        target[15] = '\0';
        std::fprintf(stderr, "%s_IP: %s interface %s\n", internal ? "INTERNAL" : "PUBLIC",
                     text, interface->ifa_name);
    }
    freeifaddrs(interfaces);
    return g_szPublicIP[0] != '\0';
}

// 080dafe0
void LoadStateUserCount()
{
    std::FILE* file = std::fopen("state_user_count", "r");
    if (!file)
        return;
    if (!LC_IsHongKong())
        std::fscanf(file, " %d %d ", &g_iFullUserCount, &g_iBusyUserCount);
    std::fclose(file);
}

namespace {
struct SQLConfig {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
};

[[noreturn]] void ConfigError(const char* message)
{
    std::fputs(message, stderr);
    std::fputc('\n', stderr);
    std::exit(1);
}

bool ReadConfigEntry(std::FILE* file, std::string& key, std::string& value)
{
    char line[257];
    char token[256];
    char text[256];
    while (std::fgets(line, sizeof(line) - 1, file)) {
        char* colon = std::strchr(line, ':');
        if (!colon)
            continue;
        // parse_token starts the value two bytes after the separator.
        if (!colon[1])
            colon[2] = '\0';
        parse_token(line, token, text);
        key.assign(token);
        value.assign(text);
        return true;
    }
    return false;
}

void ReadSQLConfig(const std::string& value, SQLConfig& config, const char* syntax)
{
    std::istringstream fields(value);
    if (!(fields >> config.host >> config.user >> config.password >> config.database))
        ConfigError(syntax);
}

void ReadSkillPower(const char* values, int (&table)[41])
{
    std::istringstream fields(values ? values : "");
    for (int& level : table) {
        if (!(fields >> level))
            ConfigError("[SKILL_PERCENT] cannot start server, short of skill information from locale table");
    }
    std::fprintf(stderr, "[SKIKK_PERCENT] Load Ok%d\n", 41);
}

void LoadSkillPowerTables(CAccountDB& accounts)
{
    std::fprintf(stderr, "CommonSQL.LoadSkillLevels\n");
    std::unique_ptr<SQLMsg> base(accounts.DirectQuery("SELECT mValue from locale where mKey='SKILL_POWER_BY_LEVEL'"));
    SQLResult* result = base ? base->Get() : nullptr;
    MYSQL_ROW row = result && result->pSQLResult ? mysql_fetch_row(result->pSQLResult) : nullptr;
    if (!row)
        ConfigError("[SKILL_PERCENT] cannot start server, no skill information from locale table");

    int defaultTable[41];
    ReadSkillPower(row[0], defaultTable);
    for (int type = 0; type < 8; ++type) {
        char query[128];
        std::snprintf(query, sizeof(query),
                      "SELECT mValue from locale where mKey='SKILL_POWER_BY_LEVEL_TYPE%d' ORDER BY CAST( mValue AS unsigned )",
                      type);
        std::unique_ptr<SQLMsg> override(accounts.DirectQuery(query));
        SQLResult* typeResult = override ? override->Get() : nullptr;
        if (!typeResult || !typeResult->pSQLResult)
            ConfigError("[SKILL_PERCENT] cannot start server, no skill information from locale table");
        int overrideTable[41];
        int* table = defaultTable;
        if (typeResult->iNumRows) {
            row = mysql_fetch_row(typeResult->pSQLResult);
            if (!row)
                ConfigError("[SKILL_PERCENT] cannot start server, row == NULL");
            ReadSkillPower(row[0], overrideTable);
            table = overrideTable;
        }
        CTableBySkill::instance().SetSkillPowerByLevelFromType(type, table);
    }
    std::fprintf(stderr, "CommonSQL.LoadSkillLevels.OK\n");
}

void LoadCommandPrivileges()
{
    std::FILE* file = std::fopen("CMD", "r");
    if (!file)
        return;
    char line[256];
    while (std::fgets(line, sizeof(line), file)) {
        std::istringstream fields(line);
        std::string command, privilege;
        if (!(fields >> command >> privilege))
            ConfigError("CMD syntax error: <cmd> <DISABLE | LOW_WIZARD | WIZARD | HIGH_WIZARD | GOD>");
        const char* names[] = {"LOW_WIZARD", "WIZARD", "HIGH_WIZARD", "GOD", "IMPLEMENTOR", "DISABLE"};
        bool matched = false;
        for (int i = 0; i < 6; ++i) {
            if (strcasecmp(privilege.c_str(), names[i]) == 0) {
                interpreter_set_privilege(command.c_str(), i + 1);
                matched = true;
                break;
            }
        }
        if (!matched)
            ConfigError("CMD syntax error: <cmd> <DISABLE | LOW_WIZARD | WIZARD | HIGH_WIZARD | GOD>");
    }
    std::fclose(file);
}

void CopyServiceAddress(char (&destination)[16], const std::string& source)
{
    std::strncpy(destination, source.c_str(), sizeof(destination) - 1);
    destination[15] = '\0';
    for (char& ch : destination) {
        if (ch == ' ')
            ch = '\0';
    }
}
} // namespace

// 080d8300
void config_init(const std::string& localeServiceName)
{
    const std::string filename = localeServiceName.empty() ? "CONFIG" : "CONFIG." + localeServiceName;
    std::FILE* config = std::fopen(filename.c_str(), "r");
    if (!config) {
        std::fprintf(stderr, "Can not open [%s]\n", filename.c_str());
        std::exit(1);
    }
    if (!GetIPInfo())
        ConfigError("Can not get public ip address");
    std::FILE* initial = std::fopen(filename.c_str(), "r");
    if (!initial)
        ConfigError("Cannot reopen configuration for SQL settings");

    SQLConfig common, player, log, matrix;
    std::string key, value;
    while (ReadConfigEntry(initial, key, value)) {
        if (key == "block_login")
            g_stBlockDate = value;
        else if (key == "adminpage_ip1")
            g_stAdminPageIP[0] = value;
        else if (key == "adminpage_ip2")
            g_stAdminPageIP[1] = value;
        else if (key == "adminpage_ip3")
            g_stAdminPageIP[2] = value;
        else if (key == "hostname") {
            g_stHostname = value;
            std::fprintf(stderr, "hostname %s\n", g_stHostname.c_str());
        } else if (key == "channel")
            g_bChannel = static_cast<BYTE>(std::atoi(value.c_str()));
        else if (key == "player_sql")
            ReadSQLConfig(value, player, "player_sql syntax: logsql <host user password db>");
        else if (key == "common_sql")
            ReadSQLConfig(value, common, "common_sql syntax: logsql <host user password db>");
        else if (key == "log_sql")
            ReadSQLConfig(value, log, "log_sql syntax: logsql <host user password db>");
        else if (key == "matrix_sql")
            ReadSQLConfig(value, matrix, "matrix_sql_for_runup syntax: matrix_sql <host user password db>");
    }
    std::fclose(initial);
    if (common.host.empty())
        ConfigError("LOAD_COMMON_SQL_INFO_FAILURE: COMMON_SQL: HOST USER PASSWORD DATABASE");
    if (player.host.empty())
        ConfigError("LOAD_PLAYER_SQL_INFO_FAILURE: PLAYER_SQL: HOST USER PASSWORD DATABASE");
    if (LC_IsHongKong() && matrix.host.empty())
        ConfigError("LOAD_MATRIX_SQL_INFO_FAILURE: MATRIX_SQL: HOST USER PASSWORD DATABASE");

    CAccountDB& accounts = CAccountDB::instance();
    accounts.Connect(common.host.c_str(), common.user.c_str(), common.password.c_str(), common.database.c_str());
    if (!accounts.IsConnected())
        ConfigError("cannot start server while no player sql connected");

    {
        std::unique_ptr<SQLMsg> rows(accounts.DirectQuery("SELECT mKey,mValue FROM locale"));
        SQLResult* result = rows ? rows->Get() : nullptr;
        if (!result || !result->pSQLResult || !result->iNumRows)
            ConfigError("cannot start server, no locale information from locale table");
        while (MYSQL_ROW row = mysql_fetch_row(result->pSQLResult)) {
            if (row[0] && row[1] && strcasecmp(row[0], "LOCALE") == 0) {
                LocaleService_Init(row[1]);
                std::fprintf(stderr, "Set Locale = %s\n", row[1]);
                if (!LC_InitLocalization(row[1]))
                    ConfigError("[Load Locale] Wrong locale key from db");
            }
        }
    }
    std::fprintf(stderr, "[SetLocale] %s\n", g_stLocale.c_str());
    accounts.SetLocale(g_stLocale);

    DBManager& players = DBManager::instance();
    std::fprintf(stderr, "PlayerSQL.Connect\n");
    players.Connect(player.host.c_str(), player.user.c_str(), player.password.c_str(), player.database.c_str());
    std::fprintf(stderr, "PlayerSQL.Connect.Done\n");
    if (!players.IsConnected())
        ConfigError("PlayerSQL.ConnectError");
    std::fprintf(stderr, "PlayerSQL.OK\n");

    if (!g_bAuthServer) {
        LogManager& logs = LogManager::instance();
        logs.Connect(log.host.c_str(), log.user.c_str(), log.password.c_str(), log.database.c_str());
        if (!logs.IsConnected())
            ConfigError("LogSQL.ConnectError");
        std::fprintf(stderr, "LogSQL.OK\nLogSQL.Insert(bootlog)\n");
        std::string hostname(g_stHostname.size() * 2 + 1, '\0');
        logs.EscapeString(&hostname[0], g_stHostname.c_str(), g_stHostname.size());
        logs.Query("INSERT INTO bootlog (time, hostname, channel) VALUES(NOW(), '%s', %d)",
                   hostname.c_str(), g_bChannel);
    }
    LoadSkillPowerTables(accounts);
    log_keep_days = 2;
    std::fprintf(stderr, "End one\n");
    while (ReadConfigEntry(config, key, value)) {
        const int numeric = std::atoi(value.c_str());
        if (key == "empire_whisper") g_bEmpireWhisper = numeric != 0;
        else if (key == "mark_server") guild_mark_server = is_string_true(value.c_str());
        else if (key == "port") mother_port = static_cast<WORD>(numeric);
        else if (key == "log_keep_days") log_keep_days = std::max(1, std::min(numeric, 90));
        else if (key == "passes_per_sec") passes_per_sec = numeric;
        else if (key == "p2p_port") p2p_port = static_cast<WORD>(numeric);
        else if (key == "db_port") db_port = static_cast<WORD>(numeric);
        else if (key == "db_addr") CopyServiceAddress(db_addr, value);
        else if (key == "save_event_second_cycle")
            save_event_second_cycle = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(numeric) * static_cast<std::uint32_t>(passes_per_sec));
        else if (key == "ping_event_second_cycle")
            ping_event_second_cycle = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(numeric) * static_cast<std::uint32_t>(passes_per_sec));
        else if (key == "table_postfix") g_table_postfix = value;
        else if (key == "test_server") {
            std::puts("-----------------------------------------------\nTEST_SERVER\n-----------------------------------------------");
            test_server = numeric;
        } else if (key == "pk_server") g_bPKServer = numeric != 0;
        else if (key == "distribution_test_server") distribution_test_server = numeric != 0;
        else if (key == "china_event_server") china_event_server = numeric != 0;
        else if (key == "shutdowned") g_bNoMoreClient = true;
        else if (key == "no_regen") g_bNoRegen = true;
        else if (key == "traffic_profile") g_bTrafficProfileOn = true;
        else if (key == "map_allow") {
            std::string map;
            for (const unsigned char ch : value) {
                if ((ch & 0xe0) <= 0x90 && std::isspace(ch)) {
                    if (!map.empty()) {
                        map_allow_add(std::atoi(map.c_str()));
                        map.clear();
                    }
                } else {
                    map += static_cast<char>(ch);
                }
            }
            if (!map.empty())
                map_allow_add(std::atoi(map.c_str()));
        } else if (key == "item_id_range") {
            char first[256];
            char last[256];
            two_arguments(value.data(), first, last);
            if (!first[0] || !last[0])
                ConfigError("ITEM_ID_RANGE syntax: ITEM_ID_RANGE <start> <end>");
            g_dwItemIDRange[0] = static_cast<std::uint32_t>(std::atoi(first));
            g_dwItemIDRange[1] = static_cast<std::uint32_t>(std::atoi(last));
            if (g_dwItemIDRange[1] < g_dwItemIDRange[0]) {
                std::fprintf(stderr, "invalid ITEM_ID_RANGE %lu ~ %lu",
                             static_cast<unsigned long>(g_dwItemIDRange[0]),
                             static_cast<unsigned long>(g_dwItemIDRange[1]));
                std::exit(1);
            }
        } else if (key == "no_wander") no_wander = true;
        else if (key == "user_limit") g_iUserLimit = numeric;
        else if (key == "hostname") g_stHostname = value;
        else if (key == "skill_disable") g_bSkillDisable = numeric != 0;
        else if (key == "auth_server") {
            std::istringstream fields(value);
            std::string host, port;
            fields >> host >> port;
            const bool master = strcasecmp(host.c_str(), "master") == 0;
            if (host.empty() || (!master && port.empty()))
                ConfigError("invalid auth_master syntax");
            g_bAuthServer = true;
            LoadBanIP("BANIP");
            if (master) {
                std::fprintf(stderr, "AUTH_SERVER: I am the master\n");
            } else {
                g_stAuthMasterIP = host;
                g_wAuthMasterPort = static_cast<WORD>(std::atoi(port.c_str()));
                std::fprintf(stderr, "AUTH_SERVER: master %s %u\n", host.c_str(),
                             static_cast<unsigned>(g_wAuthMasterPort));
            }
        } else if (key == "billing") g_bBilling = true;
        else if (key == "quest_dir") {
            sys_log(0, "QUEST_DIR SETTING : %s", value.c_str());
            g_stQuestDir = value;
        } else if (key == "quest_object_dir") {
            std::istringstream paths(value);
            std::string path;
            while (paths >> path)
                g_setQuestObjectDir.insert(path);
        } else if (key == "teen_addr") CopyServiceAddress(teen_addr, value);
        else if (key == "teen_port") teen_port = static_cast<WORD>(numeric);
        else if (key == "speedhack_limit_count") SPEEDHACK_LIMIT_COUNT = numeric;
        else if (key == "speedhack_limit_bonus") SPEEDHACK_LIMIT_BONUS = numeric;
    }
    std::fclose(config);
    if (g_setQuestObjectDir.empty())
        g_setQuestObjectDir.insert(g_stDefaultQuestObjectDir);
    LocaleService_TransferDefaultSetting();
    if (!db_port)
        ConfigError("DB_PORT not configured");
    if (!g_bChannel)
        ConfigError("CHANNEL not configured");
    if (g_stHostname.empty())
        ConfigError("HOSTNAME must be configured.");
    LocaleService_LoadEmpireTextConvertTables();
    LoadCommandPrivileges();
    LoadValidCRCList();
    LoadStateUserCount();
    CWarMapManager::instance().LoadWarMapInfo(nullptr);
    if (LC_IsHongKong() && g_bAuthServer) {
        MatrixDBManager& matrixDb = MatrixDBManager::instance();
        if (matrix.host.empty() || !matrixDb.InitMatrix(matrix.host.c_str(), matrix.user.c_str(),
                                                        matrix.password.c_str(), matrix.database.c_str()))
            ConfigError("cannot start server while no matrix sql");
        std::fprintf(stderr, "INIT MATRIX\n");
    } else {
        std::fprintf(stderr, "NOT INIT MATRIX\n");
    }
    std::fprintf(stderr, "Success Config_Init()\n");
}
