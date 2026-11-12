#include "exp_data_type_checks.h"
#include "exp_data_golden.h"
#include <cassert>
#include <cstring>
#include <cstdio>
int main() {
    assert(sizeof(DWORD)==4);
    ExpCheckAllTypes();assert(exp_table==0); // Original .bss state, not automatic euckr selection.
    const DWORD* tables[]={exp_table_euckr,exp_table_gb2312,exp_table_sjis,exp_table_hk,exp_table_newcibn};
    for(unsigned i=0;i<5;++i) {
        assert(!std::memcmp(tables[i],exp_data_golden[i],400)); // Every original byte.
        exp_table=tables[i];assert(exp_table==tables[i]); // Test-only mutable-pointer proof.
    }
    assert(exp_table_euckr[0]==0&&exp_table_gb2312[0]==0&&exp_table_sjis[0]==0);
    assert(exp_table_hk[0]==22&&exp_table_newcibn[0]==300); // Preserve unusual original indexing.
    exp_table=0;
    std::puts("EXP data passed: five exact readonly100-DWORD arrays, original byte images and initially-null mutable const pointer");
}
