// Original complete packed common/tables.h record, target32 ABI only.
#include "../../common/tables.h"
#include <cstddef>
typedef char save_dword32[sizeof(DWORD)==4?1:-1];
typedef char save_long32[sizeof(long)==4?1:-1];
typedef char save_size106[sizeof(TQuestTable)==106?1:-1];
typedef char save_pid0[offsetof(TQuestTable,dwPID)==0?1:-1];
typedef char save_name4[offsetof(TQuestTable,szName)==4?1:-1];
typedef char save_state37[offsetof(TQuestTable,szState)==37?1:-1];
typedef char save_value102[offsetof(TQuestTable,lValue)==102?1:-1];
typedef char save_name33[sizeof(((TQuestTable*)0)->szName)==33?1:-1];
typedef char save_state65[sizeof(((TQuestTable*)0)->szState)==65?1:-1];
