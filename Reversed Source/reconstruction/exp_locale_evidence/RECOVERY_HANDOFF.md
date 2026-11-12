# EXP bounded integration handoff

Target SHA-256: 61fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c.

## Ready candidates
Insert constants-exp-declarations.inc into original game/src/constants.h and constants-exp-definitions.inc into constants.cpp after that header is included. All five const DWORD[100] tables have external declarations; exp_table is a mutable pointer to const DWORD, initially NULL in original .bss. Do not choose an implicit locale default. HK[0]=22 and NewCIBN[0]=300 are original data.

Insert char-get-next-exp.inc into CHARACTER in char.h, with constants.h included outside the class. DWARF char.h:584 proves DWORD GetNextExp() is nonconst. Five inline ranges from original char.cpp are saved in get-next-exp-inline-proof.json. PointsPacket instructions at 0812a309 initialize 0x77359400 (2000000000), zero-extend the level byte at +0x502, compare to 98, and skip the exp_table lookup above 98; other levels load exp_table[level]. Level 99..255 needs no configured table; 0..98 requires a nonnull table. No new guard or setter is introduced.

## Regression and ABI
Copy exp_data_golden.h, exp_data_type_checks.h, exp_data_regression.cpp, exp_next_regression.cpp and exp_data_abi.cpp into the existing legacy test directory, and integrate Makefile.exp-data.inc. Golden bytes come directly from original ELF data, independently of C++ numeric formatting. Data regression checks 2000 bytes and initial null pointer; type checks preserve all array bounds, const pointees, and pointer mutability. Getter regression uses real CHARACTER and existing full fixture graph, covers all 256 level bytes with each table, and null table only for safe levels 99+. ABI checks DWORD/pointer width and exact nonconst member signature; no host STL/class layout claims.

No builds or target execution were performed by this agent. Parent owns integration and validation. The source/test candidates are frozen.

## Locale selection remains incomplete
All 25 LocaleService bodies/call graphs, plus __CheckPlayerSlot and four CTableBySkill methods, are saved in addressed raw exports. Original addressed-ghidra.json remains unchanged. Data may integrate independently; these exports are evidence, not implemented/reviewed recovery claims.

CIBN selects gb2312; CIBN_OLD calls CIBN and changes paths; JAPAN selects sjis; HK selects hk; NewCIBN selects newcibn. DEFAULT and other initializers do not directly select EXP. LocaleService_TransferDefaultSetting sets euckr only when exp_table is null, alongside name validators, multibyte callback, eight skill table checks and combat table defaults. LocaleService_Init uses service-name emptiness as its one-time guard, dispatches locale initializers and calls __CheckPlayerSlot; it does not itself invoke TransferDefaultSetting.

Full initialization additionally needs exact path/string globals and quest object set handling, name/multibyte validators, skill power allocation/deletion semantics, combat/chain default tables, service dispatch string decoding, and player-slot checks. The old DeleteSkillPowerByLevelFromType decompilation has suspicious null-dependent behavior that requires instruction review before implementation. No partial locale setter, guessed CTableBySkill layout, normalized EXP values, or narrowed initializer is supplied.

## Provenance
data.json records all symbol addresses, section flags, original bytes and per-symbol SHA-256. api-metadata.json records DWARF signatures/types and inline ranges. program-info.json pins Ghidra target identity. Original raw exports retain tool schemas. manifest.json hashes the frozen candidates and evidence; build validation is explicitly separate from function completeness.
