// C++03 target signatures and scalar widths. Full modern STL logger layout is not the original GCC3 ABI.
#include "../../game/src/log.h"
typedef char charlog_dword4[(sizeof(DWORD)==4)?1:-1];
void charlog_signature_checks() {
    void (LogManager::*audit)(DWORD,DWORD,DWORD,DWORD,const char*,const char*)=&LogManager::CharLog;
    void (LogManager::*query)(const char*,...)=&LogManager::Query;
    (void)audit;(void)query;
}
