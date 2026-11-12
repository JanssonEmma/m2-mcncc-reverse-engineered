// Partial original configuration storage; parser and remaining globals are unresolved.
#include "config.h"
bool g_bTrafficProfileOn=false;
bool g_bAuthServer=false;
int test_server=0;

// ELF initialized int at0823f21c.
int passes_per_sec=25;

// 080dac10 and config static initialization: initially empty table suffix.
std::string g_table_postfix="";
const char* get_table_postfix() { return g_table_postfix.c_str(); }

// Original8-byte BSSsymbol083231a4; DWORD[2] spelling inferred from ABI/uses.
DWORD g_dwItemIDRange[2]={0,0};

// Original config.cpp globals; exact dynamic defaults proven at080db859..080db8e1.
std::string g_stQuestDir="./quest";
std::string g_stDefaultQuestObjectDir="./quest/object";
std::set<std::string> g_setQuestObjectDir;
