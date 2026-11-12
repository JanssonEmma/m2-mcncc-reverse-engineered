// Insert in game/src/constants.h before final guard. Exact old100-DWORD symbols.
extern const DWORD exp_table_euckr[100];
extern const DWORD exp_table_gb2312[100];
extern const DWORD exp_table_sjis[100];
extern const DWORD exp_table_hk[100];
extern const DWORD exp_table_newcibn[100];
extern const DWORD* exp_table; // pointer mutable, pointee const; original .bss initial0
