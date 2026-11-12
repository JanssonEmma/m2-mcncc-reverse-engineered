// Actual CHARACTER, inline getter and complete shared fixture graph; no reduced class.
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../game/src/config.h"
#include "exp_data_type_checks.h"
#include "exp_data_golden.h"
#include <cassert>
#include <cstdio>
int max_bytes_written,current_bytes_written,total_bytes_written;
extern "C" void desc_test_allow_clock(void);
static DWORD Read4(const unsigned char* p) {
    return static_cast<DWORD>(p[0])|static_cast<DWORD>(p[1])<<8|
        static_cast<DWORD>(p[2])<<16|static_cast<DWORD>(p[3])<<24;
}
int main() {
    assert(sizeof(DWORD)==4);ExpCheckAllTypes();assert(exp_table==0);
    DWORD (CHARACTER::*getter)()=&CHARACTER::GetNextExp;(void)getter;
    desc_test_allow_clock();test_server=0;quest::CQuestManager quests;CHARACTER ch;
    const DWORD* tables[]={exp_table_euckr,exp_table_gb2312,exp_table_sjis,exp_table_hk,exp_table_newcibn};
    for(unsigned t=0;t<5;++t) {
        exp_table=tables[t];
        for(unsigned level=0;level<256;++level) {
            ch.m_points.level=static_cast<BYTE>(level);
            const DWORD expected=level<99?Read4(exp_data_golden[t]+level*4):2000000000UL;
            assert(ch.GetNextExp()==expected&&ch.m_points.level==level&&exp_table==tables[t]);
        }
    }
    exp_table=0;
    for(unsigned level=99;level<256;++level) {
        ch.m_points.level=static_cast<BYTE>(level);assert(ch.GetNextExp()==2000000000UL&&exp_table==0);
    }
    // With NULL below99 the original dereferences NULL; do not execute that invalid precondition.
    std::puts("GetNextExp passed: actual CHARACTER, all five tables/all256 levels and original99+ cap without NULL-table access");
}
