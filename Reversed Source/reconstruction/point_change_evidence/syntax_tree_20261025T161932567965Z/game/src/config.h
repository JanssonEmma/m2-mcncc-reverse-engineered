// Partial configuration declarations; see traffic-profiler-recovery.md.
#ifndef MCNCC_GAME_CONFIG_H
#define MCNCC_GAME_CONFIG_H
#include <string>
#include "../../libthecore/include/typedef.h"
// One-byte target symbol; bool is reference-guided (no matching variable DIE).
extern bool g_bTrafficProfileOn;
extern bool g_bAuthServer;
extern int test_server; // Reference-guided declaration; binary uses a 32-bit comparison.
extern int passes_per_sec;
extern std::string g_table_postfix;
const char* get_table_postfix();
extern BYTE PK_PROTECT_LEVEL;
#endif
