// Partial configuration declarations; see traffic-profiler-recovery.md.
#ifndef MCNCC_GAME_CONFIG_H
#define MCNCC_GAME_CONFIG_H
#include <string>
#include <set>
#include "../../libthecore/include/typedef.h"
// One-byte target symbol; bool is reference-guided (no matching variable DIE).
extern bool g_bTrafficProfileOn;
extern bool g_bAuthServer;
extern bool g_bBilling;
extern std::string g_stBlockDate;
extern bool g_bNoPasspod;
extern bool guild_mark_server;
extern int test_server; // Reference-guided declaration; binary uses a 32-bit comparison.
extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int SPEEDHACK_LIMIT_COUNT;
extern int SPEEDHACK_LIMIT_BONUS;
extern bool g_bSkillDisable;
extern std::string g_table_postfix;
extern std::string g_stHostname;
const char* get_table_postfix();
extern BYTE PK_PROTECT_LEVEL;
// Add to original game/src/config.h. Reference declarations + exact old callback DWARF.
extern std::string g_stLocaleFilename;
extern int (*check_name)(const char*);
extern int (*is_twobyte)(const char*);

extern DWORD g_dwItemIDRange[2];
// Add <set> to config.h. Full logical STL types, no old-container padding shim.
extern std::string g_stQuestDir;
extern std::string g_stDefaultQuestObjectDir;
extern std::set<std::string> g_setQuestObjectDir;

bool map_allow_find(int index);
extern bool g_bCheckClientVersion;
extern std::string g_stClientVersion;
extern std::string g_stSMSPrefix;
void LoadValidCRCList();
bool LoadClientVersion();
void CheckClientVersion();
bool IsValidFileCRC(DWORD crc);
bool IsValidProcessCRC(DWORD crc);
extern BYTE g_bChannel;
extern WORD p2p_port;
void map_allow_log();
void map_allow_add(int index);
void map_allow_copy(BYTE* output, int size);

bool GetIPInfo();
void LoadStateUserCount();
void config_init(const std::string& localeServiceName);
extern WORD mother_port;
extern char g_szPublicIP[16];
extern char g_szInternalIP[16];
extern char db_addr[16];
extern WORD db_port;
extern char teen_addr[16];
extern WORD teen_port;
extern std::string g_stAuthMasterIP;
extern WORD g_wAuthMasterPort;
extern bool g_bNoRegen;
extern bool distribution_test_server;
extern bool china_event_server;
extern bool g_bNoMoreClient;
extern bool g_bEmpireWhisper;
extern bool g_bPKServer;
extern bool no_wander;
extern std::string g_stAdminPageIP[3];
extern int g_iUserLimit;

extern int g_iFullUserCount;

extern int g_iBusyUserCount;

#endif
