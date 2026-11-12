// Compile-only checks, no whole STL/class layout claim.
#include "exp_data_type_checks.h"
#include "../../game/src/char.h"
#define EXP_ABI(name,expr) typedef char exp_abi_##name[(expr)?1:-1]
EXP_ABI(dword4,sizeof(DWORD)==4);
EXP_ABI(pointer4,sizeof(exp_table)==4);
EXP_ABI(euckr400,sizeof(exp_table_euckr)==400);
EXP_ABI(gb2312_400,sizeof(exp_table_gb2312)==400);
EXP_ABI(sjis400,sizeof(exp_table_sjis)==400);
EXP_ABI(hk400,sizeof(exp_table_hk)==400);
EXP_ABI(newcibn400,sizeof(exp_table_newcibn)==400);
void exp_data_type_and_signature_checks() {
    ExpCheckAllTypes();
    DWORD (CHARACTER::*next)()=&CHARACTER::GetNextExp;(void)next;
}
