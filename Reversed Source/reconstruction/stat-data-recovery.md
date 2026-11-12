# Original stat enums and tables

Target SHA25661fa6e736a5b0ff446dfd817dd9a87a42d2bfbcf5da27b7975cb8ae1e2b67a1c. Complete old EMisc(40), EJobs(5), EApplyTypes(83) and EPointTypes(127) declarations are now in common/length.h and game/src/char.h. POINT_MAX_NUM255 belongs to EMisc; the named EPointTypes entries end at126. No modern enum extensions were imported.

constants.h/.cpp now contain the complete original S/TJobInitialPoints (sixteen signed int fields,64bytes), four mutable JobInitialPoints rows, S/TApplyInfo (BYTE at0,1byte), and82 const apply mapping records. Original typedef links and table qualifiers are DWARF/ELF-confirmed: job data writable, apply data read-only. All field offsets and table dimensions are gated in stat_data_abi.cpp.

stat_data_golden.h contains independent expected bytes and enum values extracted from saved original ELF/DWARF metadata, with source table SHA256 checked before generation. The runtime test compares all338 bytes of compiled tables, checks255 enumerator values, mutates/restores job data and rechecks bytes. This establishes compiled initial data/layout and mutability, not complete ApplyPoint/PointChange semantics. Special applies such as skill51 and unknown77 require the original switch behavior despite zero mapping entries.

Focused normal and O2 data regressions pass. Full checkpoint65 driver completed exit0 with62 exact runtime commands plus check-abi. The original enum fragment trailing commas were subsequently normalized during checkpoint66; modified party/character/constants sources passed strict32-bit C++03 syntax, and all63 runtime commands plus ABI checks passed again. Native full server linking, stat methods and original runtime execution remain unproved.

Complete dependency evidence is archived in stat_dependency_evidence/, normalized57 function exports in stat-dependency-addressed-ghidra.json, and bounded ordering/SCC plan in stat-dependency-RECOVERY_PLAN.md. PointChange/ComputeBattlePoints and mount/ComputePoints/ModifyPoints have real cycles that must be recovered together rather than replaced with narrowed point accumulators.
