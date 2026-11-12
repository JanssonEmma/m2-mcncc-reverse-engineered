// Partial configuration declarations; see traffic-profiler-recovery.md.
#ifndef MCNCC_GAME_CONFIG_H
#define MCNCC_GAME_CONFIG_H
#include <string>
#include <set>
#include "../../libthecore/include/typedef.h"
// One-byte target symbol; bool is reference-guided (no matching variable DIE).
extern bool g_bTrafficProfileOn;
extern bool g_bAuthServer;
extern int test_server; // Reference-guided declaration; binary uses a 32-bit comparison.
extern int passes_per_sec;
extern std::string g_table_postfix;
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

#endif
